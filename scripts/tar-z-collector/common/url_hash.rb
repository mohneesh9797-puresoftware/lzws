# Data Format:
# {url} #{digest}\n
# ...
#
# Several urls can have the same digest.
#
# We can read hash in the opposite direction.
# [digest] -> url
# So we will select only one url for one digest.
# Other urls for same digest will be ignored.

def read_url_hash(path:, reverse: false)
  File
    .read(path)
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
    .map do |row|
      values = row
        .split(" ")
        .map(&:strip)
        .reject(&:empty?)
      reverse ? values.reverse : values
    end
    .to_h
end

def write_url_hash(path:, url_hash:)
  data = url_hash
    .map { |values| values.join " " }
    .sort
    .join("\n")
  File.write path, data

  nil
end
