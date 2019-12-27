require "colorize"
require "digest"
require "filesize"
require "ocg"
require "parallel"
require "uri"

require "English"

require_relative "../common/data"
require_relative "../common/format"
require_relative "../common/query"

TEMP_DIRECTORY = File.join(File.dirname(__FILE__), "..", "..", "..", "tmp").freeze
ARCHIVE_NAME   = "archive#{ARCHIVE_POSTFIX}".freeze
ARCHIVE_PATH   = File.join(TEMP_DIRECTORY, ARCHIVE_NAME).freeze

# -- binaries --

LZWS_DICTIONARIES = %w[linked-list sparse-array].freeze
BIN_DIRECTORY     = File.join(File.dirname(__FILE__), "..", "scripts").freeze

LZWS_BINARIES = LZWS_DICTIONARIES
  .map do |dictionary|
    script = File.join BIN_DIRECTORY, "lzws.sh"
    "#{script} #{dictionary}"
  end
  .freeze
COMPRESS_BINARY = File.join(BIN_DIRECTORY, "compress.sh").freeze

ALL_BINARIES = (LZWS_BINARIES + [COMPRESS_BINARY]).freeze

# -- lzws options --

BOOLS = [
  true,
  false
]
.freeze

LZWS_OPTION_COMBINATIONS = OCG.new(
  "max-code-bit-length"  => 9..16,
  "without-magic-header" => BOOLS,
  "msb"                  => BOOLS,
  "raw"                  => BOOLS,
  "unaligned-bit-groups" => BOOLS
)
.to_a

LZWS_OPTIONS = LZWS_OPTION_COMBINATIONS.map do |combination|
  combination.map do |name, value|
    next nil if value == false
    next "--#{name}" if value == true

    "--#{name}=#{value}"
  end
  .compact
  .join " "
end

LZWS_BINARIES_WITH_OPTIONS = OCG.new(
  :binary  => LZWS_BINARIES,
  :options => LZWS_OPTIONS
)
.to_a

def download_archive(url)
  begin
    uri    = URI url
    scheme = uri.scheme

    case scheme
    when "ftp"
      download_file_from_ftp uri, ARCHIVE_PATH
    when "http", "https"
      download_http_file uri, ARCHIVE_PATH
    else
      raise StandardError, "unknown uri scheme: #{scheme}"
    end

  rescue StandardError => error
    warn error
    return nil
  end

  ARCHIVE_PATH
end

def get_hash_by_archive_digest(digest, valid_archives, invalid_archives, volatile_archives)
  return [volatile_archives, "volatile".light_red] if volatile_archives.key? digest
  return [invalid_archives, "invalid"]             if invalid_archives.key? digest
  return [valid_archives, "valid".light_green]     if valid_archives.key? digest

  nil
end

def get_command_digest(command)
  result = `#{command} | sha256sum -b | awk '{printf "%s", $1}' ; exit $PIPESTATUS`
  return nil unless $CHILD_STATUS.success?

  result

rescue StandardError => error
  warn error
  nil
end

def threaded_map(items, item_threads_count, &block)
  threads_count = [Parallel.processor_count / item_threads_count, 1].max
  Parallel.map items, :in_threads => threads_count, &block
end

def test_archive(path)
  decompressed_digests = threaded_map(ALL_BINARIES, 1) do |binary|
    STDERR.print "."

    get_command_digest "#{binary} -d < \"#{path}\""
  end

  STDERR.print " "

  if decompressed_digests.uniq.length != 1
    warn "decompressed digests are not the same"
    warn "archive is #{'volatile'.light_red}"
    return :volatile
  end

  decompressed_digest = decompressed_digests.first
  if decompressed_digest.nil?
    warn "decompressed digests are nil"
    warn "archive is invalid"
    return :invalid
  end

  warn "archive decompressed"

  # Decompressed archive can be huge.
  # Re compression and decompression implemented without storing this archive.
  # So it is more CPU than I/O intensive.
  # It is safe to run this code on SSD.

  re_decompressed_digests = threaded_map(ALL_BINARIES, 3) do |binary|
    STDERR.print "."

    get_command_digest(
      "#{binary} -d < \"#{path}\" | " \
      "#{binary} | " \
      "#{binary} -d"
    )
  end

  STDERR.print " "

  if re_decompressed_digests.uniq.length != 1 || re_decompressed_digests.first != decompressed_digest
    warn "re-decompressed digests are invalid"
    warn "archive is #{'volatile'.light_red}"
    return :volatile
  end

  warn "archive re-compressed and decompressed"

  # Now we can re-compress/decompress archive using lzws options.
  # It should be possible to process lzws options in any combination.
  # So this test can take a long time.

  lzws_re_decompressed_digests = threaded_map(LZWS_BINARIES_WITH_OPTIONS, 3) do |object|
    STDERR.print "."

    binary  = object[:binary]
    options = object[:options]

    get_command_digest(
      "#{binary} -d < \"#{path}\" | " \
      "#{binary} #{options} | " \
      "#{binary} #{options} -d"
    )
  end

  STDERR.print "\n"

  if lzws_re_decompressed_digests.uniq.length != 1 || lzws_re_decompressed_digests.first != decompressed_digest
    warn "lzws re-decompressed digests are invalid"
    warn "archive is #{'volatile'.light_red}"
    return :volatile
  end

  warn "archive re-compressed and decompressed using lzws option combinations"
  warn "archive is #{'valid'.light_green}"

  :valid
end

def test_archives(archive_urls, valid_archives, invalid_archives, volatile_archives)
  volatile_archives_length = 0
  invalid_archives_length  = 0
  valid_archives_length    = 0

  archives_size = 0

  archive_urls
    .shuffle
    .each_with_index do |archive_url, index|
      percent = format_percent index, archive_urls.length
      warn "- #{percent}% testing archive, url: #{archive_url}"

      path = download_archive archive_url
      next if path.nil?

      begin
        size = File.size path
        digest = Digest::SHA256.file(path).to_s
        warn "downloaded archive, size: #{Filesize.new(size).pretty}, digest: #{digest}"

        hash, hash_name = get_hash_by_archive_digest digest, valid_archives, invalid_archives, volatile_archives
        unless hash.nil?
          hash[digest] << archive_url
          warn "digest is already in #{hash_name} archives"
          next
        end

        result = test_archive path
        case result
        when :volatile
          volatile_archives_length += 1
          hash = volatile_archives
        when :invalid
          invalid_archives_length += 1
          hash = invalid_archives
        else
          valid_archives_length += 1
          hash = valid_archives
        end

        hash[digest] = [archive_url]
        archives_size += size

      ensure
        File.delete path
      end
    end

  archives_size_text     = Filesize.new(archives_size).pretty
  volatile_archives_text = colorize_length volatile_archives_length
  invalid_archives_text  = colorize_length invalid_archives_length
  valid_archives_text    = colorize_length valid_archives_length

  warn(
    "-- processed #{archives_size_text} archives size, received " \
    "#{volatile_archives_text} volatile archives, " \
    "#{invalid_archives_text} invalid archives, " \
    "#{valid_archives_text} valid archives" \
  )

  nil
end
