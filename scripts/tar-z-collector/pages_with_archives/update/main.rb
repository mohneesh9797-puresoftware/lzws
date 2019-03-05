#!/usr/bin/env ruby

require_relative "filter"
require_relative "urls_hash"

urls_hash_path         = ARGV[0]
invalid_urls_hash_path = ARGV[1]

urls_hash         = read_urls_hash urls_hash_path
invalid_urls_hash = read_urls_hash invalid_urls_hash_path

filter_urls urls_hash, invalid_urls_hash

write_urls_hash urls_hash_path, urls_hash
write_urls_hash invalid_urls_hash_path, invalid_urls_hash
