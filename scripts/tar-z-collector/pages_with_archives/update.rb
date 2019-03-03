#!/usr/bin/env ruby

require "json"

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
  .shuffle.join " "
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

def get_search_endpoints_from_stats(stats_endpoint)
  uri = URI stats_endpoint
  data = get_http_content uri

  search_endpoints = data.scan(SEARCH_ENDPOINT_REGEXP).flatten
  STDERR.puts "received #{search_endpoints.length} search endpoints from stats endpoint: #{stats_endpoint}"

  search_endpoints
end

def get_search_endpoints
  search_endpoints = STATS_ENDPOINTS
    .shuffle
    .map { |stats_endpoint| get_search_endpoints_from_stats stats_endpoint }
    .flatten
    .sort
    .uniq

  STDERR.puts "received #{search_endpoints.length} search endpoints"

  search_endpoints
end

# -- search --

def get_urls_from_search_endpoint(search_endpoint)
  STDERR.puts "-----"
  STDERR.puts "processing search endpoint: #{search_endpoint}"

  urls = []

  uri = URI "#{search_endpoint}/search"
  params = { :q => get_search_text, :format => "json" }
  page = 1

  loop do
    params[:pageno] = page
    uri.query = URI.encode_www_form params

    begin
      data = get_http_content uri
    rescue StandardError => error
      STDERR.puts error
      break
    end

    begin
      data = JSON.parse data
    rescue StandardError # ignoring big error message
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

  STDERR.puts "-----"
  STDERR.puts "received #{urls.length} urls from all search endpoints"

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

def page_with_archives?(url)
  uri = URI url

  case uri.scheme
  when "ftp"
    begin
      data, is_listing = get_file_or_listing_from_ftp uri
    rescue StandardError => error
      STDERR.puts error
      return false, true
    end

    regexp = is_listing ? LISTING_WITH_ARCHIVES_REGEXP : PAGE_WITH_ARCHIVES_REGEXP
    [data =~ regexp, false]

  when "http", "https"
    begin
      data = get_http_content uri
    rescue StandardError => error
      STDERR.puts error
      return false, true
    end

    [data =~ PAGE_WITH_ARCHIVES_REGEXP, false]

  else
    STDERR.puts "uknown page uri scheme: #{scheme}"
    return false, true
  end
end

def check_page_with_archives(url)
  STDERR.puts "-----"
  STDERR.puts "processing page with archives, url: #{url}"

  result, is_error = page_with_archives? url
  STDERR.puts "page is #{result ? 'valid' : 'invalid'}"

  [result, is_error]
end

def get_filtered_urls(urls)
  filtered_invalid_urls = []

  filtered_urls = urls
    .shuffle
    .select do |url|
      result, is_error = check_page_with_archives url
      filtered_invalid_urls << url unless is_error
      result
    end

  STDERR.puts "-----"
  STDERR.puts "received #{filtered_urls.length} filtered urls"

  [filtered_urls, filtered_invalid_urls]
end

# -- files --

def read_urls(path)
  File
    .read(path)
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
end

def write_urls(path, urls)
  File.write path, urls.join("\n")
end

urls_path         = ARGV[0]
invalid_urls_path = ARGV[1]

urls         = read_urls urls_path
invalid_urls = read_urls invalid_urls_path

new_urls                             = get_urls - urls - invalid_urls
filtered_urls, filtered_invalid_urls = get_filtered_urls new_urls

urls         = (urls + filtered_urls).sort.uniq
invalid_urls = (invalid_urls + filtered_invalid_urls).sort.uniq

write_urls urls_path, urls
write_urls invalid_urls_path, invalid_urls
