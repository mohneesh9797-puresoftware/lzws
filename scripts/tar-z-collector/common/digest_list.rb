def read_digest_list(path)
  File
    .read(path)
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
end

def write_digest_list(path, digest_list)
  data = digest_list.join "\n"
  File.write path, data

  nil
end
