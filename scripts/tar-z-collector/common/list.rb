require "xz"

def read_list(path:)
  XZ
    .decompress(File.read(path))
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
end

def write_list(path:, list:)
  data = XZ.compress list.join("\n")
  File.write path, data

  nil
end
