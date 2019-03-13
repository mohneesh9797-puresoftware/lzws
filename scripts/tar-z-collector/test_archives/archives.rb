require "colorize"
require "digest"
require "uri"

require "English"

require_relative "../common/data"
require_relative "../common/format"
require_relative "../common/query"

TEMP_DIRECTORY = File.join("..", "..", "..", "tmp").freeze
ARCHIVE_NAME   = "archive#{ARCHIVE_POSTFIX}".freeze
ARCHIVE_PATH   = File.join(TEMP_DIRECTORY, ARCHIVE_NAME).freeze

LZWS_DICTIONARIES = %w[linked-list sparse-array].freeze
BIN_DIRECTORY     = File.join("..", "bin").freeze

COMPRESS_BINARY = File.join(BIN_DIRECTORY, "compress.sh").freeze
LZWS_BINARIES   = LZWS_DICTIONARIES
  .map do |dictionary|
    script = File.join BIN_DIRECTORY, "lzws.sh"
    "#{script} #{dictionary}"
  end
  .freeze

ALL_BINARIES = ([COMPRESS_BINARY] + LZWS_BINARIES).freeze

def download_archive(url)
  begin
    uri = URI url

    case uri.scheme
    when "ftp"
      download_file_from_ftp uri, ARCHIVE_PATH
    when "http", "https"
      download_http_file uri, ARCHIVE_PATH
    else
      raise "uknown uri scheme: #{scheme}"
    end

  rescue StandardError => error
    STDERR.puts error
    return nil
  end

  ARCHIVE_PATH
end

def get_hash_by_archive_digest(digest, valid_archives, invalid_archives, volatile_archives)
  return [valid_archives, "valid".light_green]     if valid_archives.key? digest
  return [invalid_archives, "invalid"]             if invalid_archives.key? digest
  return [volatile_archives, "volatile".light_red] if volatile_archives.key? digest

  nil
end

def test_archive(path)
  unpacked_digests = ALL_BINARIES.map do |binary|
    begin
      unpacked_digest = `#{binary} -d < "#{path}" | sha256sum -b | awk '{printf "%s", $1}' ; exit ${PIPESTATUS}`
      next nil unless $CHILD_STATUS.success?

      unpacked_digest

    rescue StandardError => error
      STDERR.puts error
      nil
    end
  end

  # Archive is volatile when unpacked digests are not the same.
  return :volatile if unpacked_digests.uniq.length != 1

  # Archive is invalid when all unpacked digests are nil.
  unpacked_digest = unpacked_digests.first
  return :invalid if unpacked_digest.nil?

  STDERR.puts "archive unpacked"

  :valid
end

def get_hash_and_message_by_test_result(result, valid_archives, invalid_archives, volatile_archives)
  return [invalid_archives, "invalid"]             if result == :invalid
  return [volatile_archives, "volatile".light_red] if result == :volatile

  [valid_archives, "valid".light_green]
end

def test_archives(archive_urls, valid_archives, invalid_archives, volatile_archives)
  archive_urls
    .shuffle
    .each_with_index do |url, index|
      percent = format_percent index, archive_urls.length
      STDERR.puts "- #{percent}% testing archive, url: #{url}"

      path = download_archive url
      next if path.nil?

      begin
        size   = File.size path
        digest = Digest::SHA256.file path
        STDERR.puts "downloaded archive, size: #{size}, digest: #{digest}"

        hash, hash_name = get_hash_by_archive_digest digest, valid_archives, invalid_archives, volatile_archives
        unless hash.nil?
          hash[digest] << url
          STDERR.puts "archive digest is already in #{hash_name} archives"
          next
        end

        result        = test_archive path
        hash, message = get_hash_and_message_by_test_result result, valid_archives, invalid_archives, volatile_archives
        hash[digest]  = [url]
        STDERR.puts "archive is #{message}"

      ensure
        File.delete path
      end
    end

  nil
end
