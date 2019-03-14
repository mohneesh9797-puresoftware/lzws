require "json"
require "uri"

require_relative "../common/data"
require_relative "../common/format"
require_relative "../common/query"

# We can try to receive all pages from single search url one-by-one.
# But search url will can return 429 error code (too many requests).
# We can make a queue of search urls and process urls one-by-one instead.
# This method provides better results.

def get_text
  # "tar.Z" index|directory|listing|ftp|file|archive
  [
    "\"#{ARCHIVE_EXTENSION}\"",
    %w[index directory listing ftp file archive].shuffle.join("|")
  ]
  .shuffle
  .join(" ")
end

def read_new_page_urls_from_search_url(url, text, page_number, page_urls)
  STDERR.puts "- processing search url: #{url}, page number: #{page_number}"

  begin
    uri = URI "#{url}/search"
  rescue StandardError
    STDERR.puts "invalid api url"
    return nil
  end

  params    = { :q => text, :format => "json", :pageno => page_number }
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

  new_page_urls = results
    .map { |result| result["url"] }
    .reject { |page_url| page_urls.include? page_url }

  if new_page_urls.empty?
    STDERR.puts "finished"
    return nil
  end

  text = colorize_length new_page_urls.length
  STDERR.puts "received #{text} page urls"

  new_page_urls
end

def get_page_urls(search_urls)
  page_urls = []

  queue = search_urls
    .shuffle
    .map do |search_url|
      {
        :url         => search_url,
        :text        => get_text,
        :page_number => 1,
        :page_urls   => []
      }
    end

  loop do
    new_queue = []

    queue.each do |data|
      new_page_urls = read_new_page_urls_from_search_url data[:url], data[:text], data[:page_number], data[:page_urls]
      next if new_page_urls.nil?

      data[:page_number] += 1
      data[:page_urls]   += new_page_urls
      page_urls          += new_page_urls

      new_queue << data
    end

    break if new_queue.empty?

    queue = new_queue
  end

  page_urls = page_urls.sort.uniq

  text = colorize_length page_urls.length
  STDERR.puts "-- received #{text} page urls from all search urls"

  page_urls
end
