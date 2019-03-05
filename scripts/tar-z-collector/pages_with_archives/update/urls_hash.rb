# Data Format:
# {url} #{digest}\n
# ...
#
# Several urls can have the same digest.

def read_urls_hash(path)
  File.read(path)
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
    .map { |row| row.split(" ").map(&:strip).reject(&:empty?) }
    .to_h
end

def write_urls_hash(path, urls_hash)
  data = urls_hash
    .map { |values| values.join(" ") }
    .sort
    .join("\n")
  File.write path, data

  nil
end
