def read_page_digests(path)
  File
    .read(path)
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
end

def write_page_digests(path, page_digests)
  data = page_digests.join "\n"
  File.write path, data

  nil
end
