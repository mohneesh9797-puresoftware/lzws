require "net/http"
require "net/ftp"
require "open-uri"
require "uri"

TOR_HOST = "127.0.0.1".freeze
TOR_SOCKS_PORT = 9050
TOR_HTTP_PORT = 8118

TOR_SOCKS_SERVER = "#{TOR_HOST}:#{TOR_SOCKS_PORT}".freeze
TOR_SOCKS_PROXY = "socks://#{TOR_SOCKS_SERVER}".freeze
TOR_HTTP_PROXY = "http://#{TOR_HOST}:#{TOR_HTTP_PORT}".freeze

ENV["SOCKS_SERVER"] = TOR_SOCKS_SERVER
ENV["ftp_proxy"] = TOR_SOCKS_PROXY
ENV["https_proxy"] = ENV["http_proxy"] = TOR_HTTP_PROXY

TIMEOUT = 20 # seconds

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

def get_http_content(uri)
  options = HTTP_OPTIONS.merge(
    :use_ssl => uri.scheme == "https"
  )

  begin
    response = Net::HTTP.start(uri.host, uri.port, options) do |http|
      http.get uri
    end
  rescue StandardError => error
    raise StandardError, "http query failed, error: #{error}"
  end

  raise StandardError, "http response failed, code: #{response.code}" unless response.is_a? Net::HTTPSuccess

  response.body || ""
end

OPEN_OPTIONS =
  %i[
    read_timeout
    open_timeout
  ]
  .map { |timeout| [timeout, TIMEOUT] }
  .to_h
  .freeze

def download_http_file(uri, file_path)
  io = Kernel.open uri, "rb", OPEN_OPTIONS

  begin
    IO.copy_stream io, file_path
  rescue StandardError => error
    raise StandardError, "http query failed, error: #{error}"
  ensure
    io.close
  end

  nil
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

def process_ftp(uri, &_block)
  options = FTP_OPTIONS.merge(
    :port => uri.port
  )

  begin
    Net::FTP.open(uri.host, options) do |ftp|
      ftp.login
      yield ftp
    end
  rescue StandardError => error
    raise StandardError, "ftp query failed, error: #{error}"
  end
end

def download_file_from_ftp(uri, file_path)
  process_ftp(uri) do |ftp|
    ftp.getbinaryfile uri.path, file_path
  end

  nil
end

def get_content_or_listing_from_ftp(uri)
  process_ftp(uri) do |ftp|
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
