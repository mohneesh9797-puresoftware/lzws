require "xz"

XZ_COMPRESSION_LEVEL = 9

def read_list(path)
  XZ
    .decompress(File.read(path))
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
end

def write_list(path, list)
  data = XZ.compress list.join("\n"), :level => XZ_COMPRESSION_LEVEL
  File.write path, data

  nil
end
