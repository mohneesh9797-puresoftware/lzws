require "json"

require_relative "../../common/data"
require_relative "../../common/query"
require_relative "stats"

# We can try to receive all pages from single search endpoint one-by-one.
# But search endpoint will can return 429 error code (too many requests).
# We can make a queue of search endpoints and process endpoints one-by-one instead.
# This method provides better results.

def get_text
  # "tar.Z" index|directory|listing|ftp
  [
    "\"#{ARCHIVE_EXTENSION}\"",
    %w[index directory listing ftp].shuffle.join("|")
  ]
  .shuffle
  .join(" ")
end

def read_next_urls_from_endpoint(endpoint:, page:, urls:)
  STDERR.puts "- processing search endpoint: #{endpoint}, page: #{page}"

  begin
    uri = URI "#{endpoint}/search"
  rescue StandardError
    STDERR.puts "invalid api url"
    return nil
  end

  params    = { :q => get_text, :format => "json", :pageno => page }
  uri.query = URI.encode_www_form params

  begin
    data = get_http_content :uri => uri
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
    .map do |endpoint|
      {
        :endpoint => endpoint,
        :page     => 1,
        :urls     => []
      }
    end

  loop do
    new_queue = []

    queue.each do |data|
      new_urls = read_next_urls_from_endpoint(
        :endpoint => data[:endpoint],
        :page     => data[:page],
        :urls     => data[:urls]
      )
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
