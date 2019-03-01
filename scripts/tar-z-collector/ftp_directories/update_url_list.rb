#!/usr/bin/env ruby

require 'net/http'
require 'json'

SEARCH_TEXT = '"tar.Z" "ftp"'.freeze

# https://github.com/asciimoo/searx/wiki/Searx-instances
STATS_ENDPOINTS = %w[
  https://stats.searx.xyz
  https://stats.searx.xyz/tor.html
].freeze

TIMEOUT = 10 # seconds
HTTP_OPTIONS = {
  open_timeout: TIMEOUT,
  ssl_timeout: TIMEOUT,
  read_timeout: TIMEOUT,
  write_timeout: TIMEOUT
}.freeze

def get_response(uri)
  options = HTTP_OPTIONS.merge(
    use_ssl: uri.scheme == 'https'
  )

  begin
    response = Net::HTTP.start(uri.host, uri.port, options) do |http|
      http.get uri
    end
  rescue StandardError => error
    raise "get response failed, error: #{error}"
  end

  raise "get response failed, code: #{response.code}" unless response.is_a? Net::HTTPSuccess

  response.body
end

# -- stats --

def get_search_endpoints_from_stats(stats_endpoint)
  uri = URI stats_endpoint
  data = get_response uri

  # <a href="https://searx.at">https://searx.at</a>
  endpoint_regexp = %r{<a\s*href=['"](.+?)['"]>\1<\/a>}
  search_endpoints = data.scan(endpoint_regexp).flatten

  STDERR.puts "received #{search_endpoints.length} search endpoints from stats endpoint: #{stats_endpoint}"

  search_endpoints
end

def get_search_endpoints
  search_endpoints = []

  STATS_ENDPOINTS.shuffle.each do |stats_endpoint|
    search_endpoints += get_search_endpoints_from_stats stats_endpoint
  end

  search_endpoints = search_endpoints.sort.uniq
  STDERR.puts "received #{search_endpoints.length} search endpoints"

  search_endpoints
end

# -- search --

def get_urls_from_search_endpoint(search_endpoint)
  urls = []

  uri = URI "#{search_endpoint}/search"
  params = { q: SEARCH_TEXT, format: 'json' }
  page = 1

  loop do
    params[:pageno] = page
    uri.query = URI.encode_www_form params

    begin
      data = get_response uri
    rescue StandardError => error
      STDERR.puts error
      break
    end

    begin
      data = JSON.parse data
    rescue StandardError # ignoring big error message
      STDERR.puts "failed to parse json, search endpoint: #{search_endpoint}"
      break
    end

    results = data['results']
    if results.nil?
      STDERR.puts "can't find results, search endpoint: #{search_endpoint}"
      break
    end

    new_urls = results
      .map { |result| result['url'] }
      .reject { |url| urls.include?(url) }

    if new_urls.empty?
      STDERR.puts "finished search endpoint: #{search_endpoint}, page: #{page}"
      break
    end

    STDERR.puts "received #{new_urls.length} urls from search endpoint: #{search_endpoint}, page: #{page}"

    urls += new_urls
    page += 1
  end

  # We need to filter firectories only.
  # File urls have point after "/".
  url_regexp = %r{/[^.]*$}

  filtered_urls = urls.select { |url| url =~ url_regexp }
  STDERR.puts "received #{filtered_urls.length} filtered urls from search endpoint: #{search_endpoint}"

  filtered_urls
end

def get_urls
  urls = []

  get_search_endpoints.shuffle.each do |search_endpoint|
    urls += get_urls_from_search_endpoint search_endpoint
  end

  urls = urls.sort.uniq
  STDERR.puts "received #{urls.length} urls"

  urls
end

puts get_urls.join("\n")
