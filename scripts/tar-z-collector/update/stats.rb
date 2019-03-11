require "uri"

require_relative "../common/format"
require_relative "../common/query"

# https://github.com/asciimoo/searx/wiki/Searx-instances
STATS_URLS = %w[
  https://stats.searx.xyz
  https://stats.searx.xyz/tor.html
]
.freeze

# <a href='https://searx.at'>https://searx.at</a>
# <a href="https://searx.at">https://searx.at</a>
# <a href=https://searx.at>https://searx.at</a>
SEARCH_URL_REGEXP = Regexp.new(
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

def get_search_urls_from_stats_url(stats_url:)
  STDERR.puts "- processing stats url: #{stats_url}"

  begin
    uri  = URI stats_url
    data = get_http_content :uri => uri

    search_urls = data.scan(SEARCH_URL_REGEXP).flatten.compact

  rescue StandardError => error
    STDERR.puts error
    return []
  end

  text = colorize_length :length => search_urls.length
  STDERR.puts "received #{text} search urls"

  search_urls
end

def get_search_urls
  search_urls = STATS_URLS
    .shuffle
    .flat_map { |stats_url| get_search_urls_from_stats_url :stats_url => stats_url }
    .sort
    .uniq

  text = colorize_length :length => search_urls.length
  STDERR.puts "-- received #{text} search urls from all stats urls"

  search_urls
end
