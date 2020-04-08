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

SEPARATOR = " ".freeze

def read_digest_hash(file_path)
  list = read_list file_path

  list.each_with_object({}) do |item, hash|
    values = item.split(SEPARATOR).map(&:strip).reject(&:empty?)
    url    = values[0]
    digest = values[1]

    if hash.key? digest
      hash[digest] << url
    else
      hash[digest] = [url]
    end
  end
end

def write_digest_hash(file_path, hash)
  list = hash
    .flat_map do |digest, urls|
      urls.map { |url| url + SEPARATOR + digest }
    end
    .sort

  write_list file_path, list

  nil
end
