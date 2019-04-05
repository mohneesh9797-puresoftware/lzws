require "colorize"
require "digest"
require "filesize"
require "uri"

require "English"

require_relative "../common/data"
require_relative "../common/format"
require_relative "../common/query"

TEMP_DIRECTORY = File.join("..", "..", "..", "tmp").freeze
ARCHIVE_NAME   = "archive#{ARCHIVE_POSTFIX}".freeze
ARCHIVE_PATH   = File.join(TEMP_DIRECTORY, ARCHIVE_NAME).freeze

# -- binaries --

LZWS_DICTIONARIES = %w[linked-list sparse-array].freeze
BIN_DIRECTORY     = File.join("..", "bin").freeze

LZWS_BINARIES = LZWS_DICTIONARIES
  .map do |dictionary|
    script = File.join BIN_DIRECTORY, "lzws.sh"
    "#{script} #{dictionary}"
  end
  .freeze
COMPRESS_BINARY = File.join(BIN_DIRECTORY, "compress.sh").freeze

ALL_BINARIES = (LZWS_BINARIES + [COMPRESS_BINARY]).freeze

# -- lzws options --

LZWS_MAX_CODE_BIT_LENGTH_OPTIONS = (9..16)
  .map { |length| "--max-code-bit-length=#{length}" }
  .freeze
LZWS_OTHER_OPTIONS = %w[--msb --raw --unaligned-bit-groups].freeze

# Variant with no other options need to be included too.
LZWS_OTHER_OPTION_COMBINATIONS = (0..LZWS_OTHER_OPTIONS.length)
  .flat_map { |length| LZWS_OTHER_OPTIONS.combination(length).to_a }
  .freeze

LZWS_OPTION_COMBINATIONS = LZWS_MAX_CODE_BIT_LENGTH_OPTIONS
  .product(LZWS_OTHER_OPTION_COMBINATIONS)
  .map(&:flatten)
  .freeze

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
      raise "unknown uri scheme: #{scheme}"
    end

  rescue StandardError => error
    STDERR.puts error
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
  result = `#{command} | sha256sum -b | awk '{printf "%s", $1}' ; exit ${PIPESTATUS}`
  return nil unless $CHILD_STATUS.success?

  result

rescue StandardError => error
  STDERR.puts error
  nil
end

def test_archive(path)
  decompressed_digests = ALL_BINARIES.map do |binary|
    get_command_digest "#{binary} -d < \"#{path}\""
  end

  if decompressed_digests.uniq.length != 1
    STDERR.puts "decompressed digests are not the same"
    STDERR.puts "archive is #{'volatile'.light_red}"
    return :volatile
  end

  decompressed_digest = decompressed_digests.first
  if decompressed_digest.nil?
    STDERR.puts "decompressed digests are nil"
    STDERR.puts "archive is invalid"
    return :invalid
  end

  STDERR.puts "archive decompressed"

  # Decompressed archive can be huge.
  # Re compression and decompression implemented without storing this archive.
  # So it is more CPU than I/O intensive.
  # It is safe to run this code on SSD.

  re_decompressed_digests = ALL_BINARIES.map do |binary|
    get_command_digest(
      "#{binary} -d < \"#{path}\" | " \
      "#{binary} | " \
      "#{binary} -d"
    )
  end

  if re_decompressed_digests.uniq.length != 1 || re_decompressed_digests.first != decompressed_digest
    STDERR.puts "re-decompressed digests are invalid"
    STDERR.puts "archive is #{'volatile'.light_red}"
    return :volatile
  end

  STDERR.puts "archive re compressed and decompressed"

  # Now we can re-compress/decompress archive using lzws options.
  # It should be possible to process lzws options in any combination.
  # So this test can take a long time.

  lzws_re_decompressed_digests = LZWS_BINARIES.flat_map do |binary|
    LZWS_OPTION_COMBINATIONS.map do |options|
      options_text = options.join " "
      get_command_digest(
        "#{binary} -d < \"#{path}\" | " \
        "#{binary} #{options_text} | " \
        "#{binary} #{options_text} -d"
      )
    end
  end

  if lzws_re_decompressed_digests.uniq.length != 1 || lzws_re_decompressed_digests.first != decompressed_digest
    STDERR.puts "lzws re-decompressed digests are invalid"
    STDERR.puts "archive is #{'volatile'.light_red}"
    return :volatile
  end

  STDERR.puts "archive re compressed and decompressed using lzws option combinations"
  STDERR.puts "archive is #{'valid'.light_green}"

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
      STDERR.puts "- #{percent}% testing archive, url: #{archive_url}"

      path = download_archive archive_url
      next if path.nil?

      begin
        size = File.size path
        digest = Digest::SHA256.file(path).to_s
        STDERR.puts "downloaded archive, size: #{Filesize.new(size).pretty}, digest: #{digest}"

        hash, hash_name = get_hash_by_archive_digest digest, valid_archives, invalid_archives, volatile_archives
        unless hash.nil?
          hash[digest] << archive_url
          STDERR.puts "digest is already in #{hash_name} archives"
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

  STDERR.puts(
    "-- processed #{archives_size_text} archives size, received " \
    "#{volatile_archives_text} volatile archives, " \
    "#{invalid_archives_text} invalid archives, " \
    "#{valid_archives_text} valid archives" \
  )

  nil
end
