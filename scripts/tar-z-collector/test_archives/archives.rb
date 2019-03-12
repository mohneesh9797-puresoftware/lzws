require "digest"
require "uri"

require_relative "../common/format"
require_relative "../common/query"

TEMP_DIRECTORY    = File.join("..", "..", "..", "tmp").freeze
ARCHIVE_FILE_NAME = "archive.tar.z".freeze
ARCHIVE_PATH      = File.join(TEMP_DIRECTORY, ARCHIVE_FILE_NAME).freeze

def download_archive(archive_url)
  begin
    uri = URI archive_url

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

def test_archives(archive_urls, valid_archives, invalid_archives, volatile_archives)
  archive_urls
    .shuffle
    .each_with_index do |archive_url, index|
      percent = format_percent index, archive_urls.length
      STDERR.puts "- #{percent}% testing archive, url: #{archive_url}"

      archive_path = download_archive archive_url
      next if archive_path.nil?

      begin
        digest = Digest::SHA256.file archive_path
        puts digest
      ensure
        File.delete archive_path
      end
    end

  nil
end
