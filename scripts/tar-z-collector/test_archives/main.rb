#!/usr/bin/env ruby

require_relative "../common/digest_hash"
require_relative "../common/format"
require_relative "../common/list"
require_relative "archives"

archive_urls_path      = ARGV[0]
valid_archives_path    = ARGV[1]
invalid_archives_path  = ARGV[2]
volatile_archives_path = ARGV[3]

archive_urls      = read_list        archive_urls_path
valid_archives    = read_digest_hash valid_archives_path
invalid_archives  = read_digest_hash invalid_archives_path
volatile_archives = read_digest_hash volatile_archives_path

valid_archive_urls    = valid_archives.values.flatten
invalid_archive_urls  = invalid_archives.values.flatten
volatile_archive_urls = volatile_archives.values.flatten

archive_urls -= valid_archive_urls
archive_urls -= invalid_archive_urls
archive_urls -= volatile_archive_urls

text = colorize_length archive_urls.length
warn "-- processing #{text} archive urls"

begin
  test_archives archive_urls, valid_archives, invalid_archives, volatile_archives
ensure
  # You can stop test at any time and it will sync all results.

  write_digest_hash valid_archives_path,    valid_archives
  write_digest_hash invalid_archives_path,  invalid_archives
  write_digest_hash volatile_archives_path, volatile_archives
end
