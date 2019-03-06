require "net/http"
require "net/ftp"

TIMEOUT = 60 # seconds

# -- http --

HTTP_OPTIONS =
  %i[
    open_timeout
    read_timeout
    write_timeout
    ssl_timeout
    continue_timeout
    keep_alive_timeout
  ]
  .map { |timeout| [timeout, TIMEOUT] }
  .to_h
  .freeze

def get_http_content(uri:)
  options = HTTP_OPTIONS.merge(
    :use_ssl => uri.scheme == "https"
  )

  begin
    response = Net::HTTP.start(uri.host, uri.port, options) { |http| http.get uri }
  rescue StandardError => error
    raise "http query failed, error: #{error}"
  end

  raise "http response failed, code: #{response.code}" unless response.is_a? Net::HTTPSuccess

  response.body
end

# -- ftp --

FTP_OPTIONS =
  %i[
    open_timeout
    read_timeout
    ssl_handshake_timeout
  ]
  .map { |timeout| [timeout, TIMEOUT] }
  .to_h
  .freeze

def process_ftp(uri:, &_block)
  options = FTP_OPTIONS.merge(
    :port => uri.port
  )

  begin
    Net::FTP.open(uri.host, options) do |ftp|
      ftp.login
      yield ftp
    end
  rescue StandardError => error
    raise "ftp query failed, error: #{error}"
  end
end

def get_file_from_ftp(uri:)
  process_ftp(:uri => uri) do |ftp|
    return ftp.getbinaryfile uri.path, nil
  end
end

def get_file_or_listing_from_ftp(uri:)
  process_ftp(:uri => uri) do |ftp|
    # We don't know whether uri path is file or listing.
    # We can try to get listing for the first time.

    path = uri.path

    begin
      ftp.chdir path
      data = ftp.list.join "\n"
      return data, true
    rescue StandardError # rubocop:disable Lint/HandleExceptions
    end

    data = ftp.getbinaryfile path, nil
    return data, false
  end
end
