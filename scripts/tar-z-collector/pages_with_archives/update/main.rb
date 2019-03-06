#!/usr/bin/env ruby

require_relative "../../common/url_hash"
require_relative "filter"

url_hash_path         = ARGV[0]
invalid_url_hash_path = ARGV[1]

url_hash         = read_url_hash :path => url_hash_path
invalid_url_hash = read_url_hash :path => invalid_url_hash_path

filter_urls :url_hash => url_hash, :invalid_url_hash => invalid_url_hash

write_url_hash :path => url_hash_path, :url_hash => url_hash
write_url_hash :path => invalid_url_hash_path, :url_hash => invalid_url_hash
