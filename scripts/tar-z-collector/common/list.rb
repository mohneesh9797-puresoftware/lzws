require "xz"

COMPRESS_OPTIONS = {
  :level   => 9,
  :check   => :sha256,
  :extreme => true
}
.freeze

def read_list(path)
  XZ.decompress(File.read(path))
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
end

def write_list(path, list)
  data = XZ.compress list.join("\n"), **COMPRESS_OPTIONS
  File.write path, data

  nil
end
