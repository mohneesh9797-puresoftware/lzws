#!/usr/bin/env ruby

require "json"
require "digest"

require "../query"

# https://github.com/asciimoo/searx/wiki/Searx-instances
STATS_ENDPOINTS = %w[
  https://stats.searx.xyz
  https://stats.searx.xyz/tor.html
]
.freeze

TARGET_ARCHIVE_EXTENSION = "tar.Z".freeze

def get_search_text
  # "tar.Z" index|directory|listing|ftp
  [
    "\"#{TARGET_ARCHIVE_EXTENSION}\"",
    %w[index directory listing ftp].shuffle.join("|")
  ]
  .shuffle
  .join(" ")
end

# -- stats --

# <a href="https://searx.at">https://searx.at</a>
SEARCH_ENDPOINT_REGEXP = Regexp.new(
  "
    <a
      [[:space:]]*
      href[[:space:]]*=[[:space:]]*
        ['\"]
          ([^'\"]+)
        ['\"]
      [[:space:]]*
    >
      [[:space:]]*
      \\1
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

    search_endpoints = data.scan(SEARCH_ENDPOINT_REGEXP).flatten

  rescue StandardError => error
    STDERR.puts error
    return []
  end

  STDERR.puts "received #{search_endpoints.length} search endpoints"

  search_endpoints
end

def get_search_endpoints
  STDERR.puts "-- searching among stats endpoints"

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

def get_urls_from_search_endpoint(search_endpoint)
  STDERR.puts "- listing search endpoint: #{search_endpoint}"

  begin
    uri = URI "#{search_endpoint}/search"
  rescue StandardError
    STDERR.puts "invalid api url"
    return []
  end

  urls   = []
  params = { :q => get_search_text, :format => "json" }
  page   = 1

  loop do
    params[:pageno] = page
    uri.query       = URI.encode_www_form params

    begin
      data = get_http_content uri
    rescue StandardError => error
      STDERR.puts error
      break
    end

    begin
      data = JSON.parse data
    rescue StandardError # Ignoring big error message.
      STDERR.puts "failed to parse json"
      break
    end

    results = data["results"]
    if results.nil?
      STDERR.puts "can't find results"
      break
    end

    new_urls = results
      .map { |result| result["url"] }
      .reject { |url| urls.include? url }

    if new_urls.empty?
      STDERR.puts "finished on page: #{page}"
      break
    end

    STDERR.puts "received #{new_urls.length} urls from page: #{page}"

    urls += new_urls
    page += 1
  end

  # Ignoring invalid urls.
  urls = urls.select do |url|
    begin
      URI url
    rescue StandardError
      false
    else
      true
    end
  end

  STDERR.puts "received #{urls.length} urls"

  urls
end

def get_urls
  urls = get_search_endpoints
    .shuffle
    .map { |search_endpoint| get_urls_from_search_endpoint search_endpoint }
    .flatten
    .sort
    .uniq

  STDERR.puts "-- received #{urls.length} urls from all search endpoints"

  urls
end

# -- filter --

# href="*.tar.Z"
PAGE_WITH_ARCHIVES_REGEXP = Regexp.new(
  "
    href[[:space:]]*=[[:space:]]*
      ['\"]
        [^'\"]+?
        \.
        #{Regexp.quote(TARGET_ARCHIVE_EXTENSION)}
      ['\"]
  ",
  Regexp::IGNORECASE | Regexp::MULTILINE | Regexp::EXTENDED
)
.freeze

# -r--r--r--  1 257  7070  337967 Jul 29  1992 *.tar.Z
LISTING_WITH_ARCHIVES_REGEXP = Regexp.new(
  "
    \.
    #{Regexp.quote(TARGET_ARCHIVE_EXTENSION)}
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

  get_urls
    .shuffle
    .reject { |url| urls_hash.key?(url) || invalid_urls_hash.key?(url) }
    .each do |url|
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
