require "digest"

require_relative "../../common/data"
require_relative "../../common/query"
require_relative "search"

# href='*.tar.Z'
# href="*.tar.Z"
# href=*.tar.Z
PAGE_WITH_ARCHIVES_REGEXP = Regexp.new(
  "
    href[[:space:]]*=[[:space:]]*
      (?:
          '
          [^']+
          #{ARCHIVE_POSTFIX_FOR_REGEXP}
          '
        |
          \"
          [^\"]+
          #{ARCHIVE_POSTFIX_FOR_REGEXP}
          \"
        |
          [^[:space:]>]+
          #{ARCHIVE_POSTFIX_FOR_REGEXP}
          [[:space:]>]
      )
  ",
  Regexp::IGNORECASE | Regexp::MULTILINE | Regexp::EXTENDED
)
.freeze

# -r--r--r--  1 257  7070  337967 Jul 29  1992 *.tar.Z
LISTING_WITH_ARCHIVES_REGEXP = Regexp.new(
  "
    [^[:space:]]+
    #{ARCHIVE_POSTFIX_FOR_REGEXP}
    (?:
      [[:space:]]
      |
      \\Z
    )
  ",
  Regexp::IGNORECASE | Regexp::MULTILINE | Regexp::EXTENDED
)
.freeze

def page_with_archives?(url:, url_hash:, invalid_url_hash:)
  STDERR.puts "- checking page, url: #{url}"

  begin
    uri = URI url

    case uri.scheme
    when "ftp"
      data, is_listing = get_file_or_listing_from_ftp :uri => uri
      regexp           = is_listing ? LISTING_WITH_ARCHIVES_REGEXP : PAGE_WITH_ARCHIVES_REGEXP

    when "http", "https"
      data   = get_http_content :uri => uri
      regexp = PAGE_WITH_ARCHIVES_REGEXP

    else
      raise "uknown uri scheme: #{scheme}"
    end

  rescue StandardError => error
    STDERR.puts error
    return nil
  end

  result = !(data =~ regexp).nil?
  digest = Digest::SHA256.hexdigest data

  STDERR.puts "page is #{result ? 'valid' : 'invalid'}"

  if result
    adding_to_hash     = url_hash
    removing_from_hash = invalid_url_hash
  else
    adding_to_hash     = invalid_url_hash
    removing_from_hash = url_hash
  end

  adding_to_hash[url] = digest
  removing_from_hash.delete url

  result
end

def filter_urls(url_hash:, invalid_url_hash:)
  urls_length         = 0
  invalid_urls_length = 0

  urls = get_urls
    .shuffle
    .reject { |url| url_hash.key?(url) || invalid_url_hash.key?(url) }

  STDERR.puts "-- filtering #{urls.length} urls"

  urls.each do |url|
    result = page_with_archives? :url => url, :url_hash => url_hash, :invalid_url_hash => invalid_url_hash
    next if result.nil?

    if result
      urls_length += 1
    else
      invalid_urls_length += 1
    end
  end

  STDERR.puts "-- filtered #{urls_length} urls, #{invalid_urls_length} invalid urls"

  nil
end
