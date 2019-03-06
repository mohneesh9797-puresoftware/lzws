# Data Format:
# {url} #{digest}\n
# ...
#
# We need to read hash in the opposite direction.
# [digest] -> url
#
# So we will select only one page url for one digest.
# Other urls for same digest will be ignored.

def read_page_urls_hash(path)
  File
    .read(path)
    .split("\n")
    .map(&:strip)
    .reject(&:empty?)
    .map { |row| row.split(" ").map(&:strip).reject(&:empty?).reverse }
    .to_h
end
