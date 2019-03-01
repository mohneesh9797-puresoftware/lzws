#!/usr/bin/env ruby

require 'net/http'
require 'json'

SEARCH_TEXT = '".tar.Z" ftp'.freeze

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

  unless response.is_a? Net::HTTPSuccess
    raise "get response failed, code: #{response.code}"
  end

  response.body
end

search_endpoints = []

# <a href="https://searx.at">https://searx.at</a>
search_endpoints_regexp = %r{<a\s*href=['"](.+?)['"]>\1<\/a>}

STATS_ENDPOINTS.each do |endpoint|
  uri = URI endpoint
  data = get_response uri

  scan_result = data.scan search_endpoints_regexp
  next_search_endpoints = scan_result.flatten
  STDERR.puts "received #{next_search_endpoints.length} search endpoints from stats endpoint: #{endpoint}"

  search_endpoints += next_search_endpoints
end

search_endpoints = search_endpoints.sort.uniq
STDERR.puts "received #{search_endpoints.length} search endpoints"

urls = []

# We need to filter firectories only.
# File urls have point after "/".
url_regexp = %r{/[^.]*$}

search_endpoints.each do |endpoint|
  uri = URI("#{endpoint}/search")
  params = { q: SEARCH_TEXT, format: 'json' }
  page = 1

  loop do
    params[:pageno] = page
    uri.query = URI.encode_www_form(params)

    begin
      data = get_response uri
    rescue StandardError => error
      STDERR.puts error
      break
    end

    begin
      data = JSON.parse(data)
    rescue StandardError => error
      STDERR.puts "failed to parse json, error: #{error}"
      break
    end

    results = data['results']
    if results.nil?
      STDERR.puts "can't find results"
      break
    end

    if results.empty?
      STDERR.puts "search endpoint: #{endpoint} finished on page #{page}"
      break
    end

    STDERR.puts "received #{results.length} urls from search endpoint: #{endpoint}"

    results.each do |result|
      url = result['url']
      urls << url if url =~ url_regexp
    end

    page += 1
  end
end

urls = urls.sort.uniq
STDERR.puts "received #{urls.length} urls"

puts urls.join("\n")
