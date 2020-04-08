require "xz"

COMPRESS_OPTIONS = {
  :level   => 9,
  :check   => :sha256,
  :extreme => true
}
.freeze

TERMINATOR = "\n".freeze

def read_list(file_path)
  XZ.decompress(File.read(file_path))
    .split(TERMINATOR)
    .map(&:strip)
    .reject(&:empty?)
end

def write_list(file_path, list)
  data = XZ.compress list.join(TERMINATOR), **COMPRESS_OPTIONS
  File.write file_path, data

  nil
end
