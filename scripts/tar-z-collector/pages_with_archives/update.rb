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
          (.+?)
        ['\"]
      [[:space:]]*
    >
      [[:space:]]*
      \\1
      [[:space:]]*
    </a>
  ",
  Regexp::MULTILINE | Regexp::EXTENDED
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

  STDERR.puts "received #{urls.length} urls from search endpoint: #{search_endpoint}"

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
  STDERR.puts "received #{urls.length} urls"

  urls
end

# -- filter --

# href="*.tar.Z"
PAGE_WITH_ARCHIVES_REGEXP = Regexp.new(
  "
    href[[:space:]]*=[[:space:]]*
      ['\"]
        .+?
        #{Regexp.quote(TARGET_ARCHIVE_EXTENSION)}
      ['\"]
  ",
  Regexp::MULTILINE | Regexp::EXTENDED
)
.freeze

def page_with_archives?(url)
  uri = URI url

  begin
    data = get_http_content uri
  rescue StandardError => error
    STDERR.puts error
    return false
  end

  data =~ PAGE_WITH_ARCHIVES_REGEXP
end

def check_page_with_archives(url)
  STDERR.puts "-----"
  STDERR.puts "processing page with archives, url: #{url}"

  result = page_with_archives? url
  STDERR.puts "page is #{result ? 'valid' : 'invalid'}"

  result
end

def get_filtered_urls(urls)
  filtered_urls = urls
    .shuffle
    .select { |url| check_page_with_archives url }

  STDERR.puts "-----"
  STDERR.puts "received #{filtered_urls.length} filtered urls"

  filtered_urls
end

old_urls = STDIN
  .read
  .split("\n")
  .map(&:strip)
  .reject(&:empty?)

new_urls      = get_urls - old_urls
filtered_urls = get_filtered_urls new_urls

all_urls = (old_urls + filtered_urls).sort.uniq
puts all_urls.join("\n")