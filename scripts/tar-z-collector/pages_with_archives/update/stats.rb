require_relative "../../common/query"

# https://github.com/asciimoo/searx/wiki/Searx-instances
ENDPOINTS = %w[
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

def get_search_endpoints_from_endpoint(endpoint)
  STDERR.puts "- processing stats endpoint: #{endpoint}"

  begin
    uri  = URI endpoint
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
  search_endpoints = ENDPOINTS
    .shuffle
    .map { |endpoint| get_search_endpoints_from_endpoint endpoint }
    .flatten
    .sort
    .uniq

  STDERR.puts "-- received #{search_endpoints.length} search endpoints"

  search_endpoints
end
