def read_list(path:)
  File
    .read(path)
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
end

def write_list(path:, list:)
  data = list.join "\n"
  File.write path, data

  nil
end
