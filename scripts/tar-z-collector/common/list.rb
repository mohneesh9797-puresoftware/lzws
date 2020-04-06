require "xz"

COMPRESS_OPTIONS = {
  :level   => 9,
  :check   => :sha256,
  :extreme => true
}
.freeze

TERMINATOR = "\n".freeze

def read_list(path)
  XZ.decompress(File.read(path))
    .split(TERMINATOR)
    .map(&:strip)
    .reject(&:empty?)
end

def write_list(path, list)
  data = XZ.compress list.join(TERMINATOR), **COMPRESS_OPTIONS
  File.write path, data

  nil
end
