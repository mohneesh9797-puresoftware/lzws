require_relative "list"

# List format:
# url_1 digest_1
# url_2 digest_1
# url_3 digest_2
# ...

# Different urls can have same digest
# We need to convert it to hash:
# digest_1 => [url_1, url_2]
# digest_2 => [url_3]
# ...

def read_digest_hash(path)
  list = read_list path

  list.each_with_object({}) do |item, hash|
    values = item.split(" ").map(&:strip).reject(&:empty?)
    url    = values[0]
    digest = values[1]

    if hash.key? digest
      hash[digest] << url
    else
      hash[digest] = [url]
    end
  end
end

def write_digest_hash(path, hash)
  list = hash
    .flat_map do |digest, urls|
      urls.map { |url| "#{url} #{digest}" }
    end
    .sort

  write_list path, list

  nil
end
