#!/usr/bin/env ruby

require "json"
require "digest"

require_relative "../common/data"
require_relative "../common/query"

# -- stats --

# https://github.com/asciimoo/searx/wiki/Searx-instances
STATS_ENDPOINTS = %w[
  https://stats.searx.xyz
  https://stats.searx.xyz/tor.html
]
.freeze

# <a href='https://searx.at'>https://searx.at</a>
# <a href="https://searx.at">https://searx.at</a>
# <a href=https://searx.at>https://searx.at</a>
SEARCH_ENDPOINT_REGEXP = Regexp.new(
  "
    <a
      [[:space:]]*
      href[[:space:]]*=[[:space:]]*
        (?:
            '
            ([^']+)
            '
          |
            \"
            ([^\"]+)
            \"
          |
            ([^[:space:]>]+)
        )
      [[:space:]]*
    >
      [[:space:]]*
      (?:
        \\1
        |
        \\2
        |
        \\3
      )
      [[:space:]]*
    </a>
  ",
  Regexp::IGNORECASE | Regexp::MULTILINE | Regexp::EXTENDED
)
.freeze

def get_search_endpoints_from_stats_endpoint(stats_endpoint)
  STDERR.puts "- processing stats endpoint: #{stats_endpoint}"

  begin
    uri  = URI stats_endpoint
    data = get_http_content uri

    search_endpoints = data.scan(SEARCH_ENDPOINT_REGEXP).flatten.compact

  rescue StandardError => error
    STDERR.puts error
    return []
  end

  STDERR.puts "received #{search_endpoints.length} search endpoints"

  search_endpoints
end

def get_search_endpoints
  search_endpoints = STATS_ENDPOINTS
    .shuffle
    .map { |stats_endpoint| get_search_endpoints_from_stats_endpoint stats_endpoint }
    .flatten
    .sort
    .uniq

  STDERR.puts "-- received #{search_endpoints.length} search endpoints"

  search_endpoints
end

# -- search --

# We can try to receive all pages from single search endpoint one-by-one.
# But this search endpoint will return 429 error code (too many requests).
# We can make a queue of search endpoints and process endpoints one-by-one instead.
# This method provides better results.

def get_search_text
  # "tar.Z" index|directory|listing|ftp
  [
    "\"#{ARCHIVE_EXTENSION}\"",
    %w[index directory listing ftp].shuffle.join("|")
  ]
  .shuffle
  .join(" ")
end

def get_urls_from_search_endpoint(search_endpoint, page, urls)
  STDERR.puts "- processing search endpoint: #{search_endpoint}, page: #{page}"

  begin
    uri = URI "#{search_endpoint}/search"
  rescue StandardError
    STDERR.puts "invalid api url"
    return nil
  end

  params    = { :q => get_search_text, :format => "json", :pageno => page }
  uri.query = URI.encode_www_form params

  begin
    data = get_http_content uri
  rescue StandardError => error
    STDERR.puts error
    return nil
  end

  begin
    data = JSON.parse data
  rescue StandardError # Ignoring big error message.
    STDERR.puts "failed to parse json"
    return nil
  end

  results = data["results"]
  if results.nil?
    STDERR.puts "can't find results"
    return nil
  end

  new_urls = results
    .map { |result| result["url"] }
    .reject { |url| urls.include? url }

  if new_urls.empty?
    STDERR.puts "finished"
    return nil
  end

  STDERR.puts "received #{new_urls.length} urls"

  new_urls
end

def get_urls
  urls  = []
  queue = get_search_endpoints
    .shuffle
    .map do |search_endpoint|
      {
        :search_endpoint => search_endpoint,
        :page            => 1,
        :urls            => []
      }
    end

  loop do
    new_queue = []

    queue.each do |data|
      new_urls = get_urls_from_search_endpoint data[:search_endpoint], data[:page], data[:urls]
      next if new_urls.nil?

      data[:page] += 1
      data[:urls] += new_urls
      urls        += new_urls

      new_queue << data
    end

    break if new_queue.empty?

    queue = new_queue
  end

  urls = urls.sort.uniq

  STDERR.puts "-- received #{urls.length} urls from all search endpoints"

  urls
end

# -- filter --

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

def page_with_archives?(url, urls_hash, invalid_urls_hash)
  STDERR.puts "- checking page, url: #{url}"

  begin
    uri = URI url

    case uri.scheme
    when "ftp"
      data, is_listing = get_file_or_listing_from_ftp uri
      regexp           = is_listing ? LISTING_WITH_ARCHIVES_REGEXP : PAGE_WITH_ARCHIVES_REGEXP

    when "http", "https"
      data   = get_http_content uri
      regexp = PAGE_WITH_ARCHIVES_REGEXP

    else
      raise "uknown uri scheme: #{scheme}"
    end

  rescue StandardError => error
    STDERR.puts error
    return false
  end

  result = data =~ regexp
  digest = Digest::SHA256.hexdigest data

  STDERR.puts "page is #{result ? 'valid' : 'invalid'}"

  if result
    adding_to_hash     = urls_hash
    removing_from_hash = invalid_urls_hash
  else
    adding_to_hash     = invalid_urls_hash
    removing_from_hash = urls_hash
  end

  adding_to_hash[url] = digest
  removing_from_hash.delete url

  result
end

def filter_urls(urls_hash, invalid_urls_hash)
  filtered_urls_length         = 0
  filtered_invalid_urls_length = 0

  urls = get_urls
    .shuffle
    .reject { |url| urls_hash.key?(url) || invalid_urls_hash.key?(url) }

  STDERR.puts "-- filtering #{urls.length} urls"

  urls.each do |url|
    if page_with_archives? url, urls_hash, invalid_urls_hash
      filtered_urls_length += 1
    else
      filtered_invalid_urls_length += 1
    end
  end

  STDERR.puts "-- filtered #{filtered_urls_length} urls, #{filtered_invalid_urls_length} invalid urls"

  nil
end

# -- files --

# Data Format:
# {url} #{digest}\n
# ...
#
# Several urls can have the same digest.

def read_urls_hash(path)
  File.read(path)
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
    .map { |row| row.split(" ").map(&:strip).reject(&:empty?) }
    .to_h
end

def write_urls_hash(path, urls_hash)
  data = urls_hash
    .map { |values| values.join(" ") }
    .sort
    .join("\n")
  File.write path, data

  nil
end

urls_hash_path         = ARGV[0]
invalid_urls_hash_path = ARGV[1]

urls_hash         = read_urls_hash urls_hash_path
invalid_urls_hash = read_urls_hash invalid_urls_hash_path

filter_urls urls_hash, invalid_urls_hash

write_urls_hash urls_hash_path, urls_hash
write_urls_hash invalid_urls_hash_path, invalid_urls_hash
