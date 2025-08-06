# striped_index
A faster fulltext index for large numbers of short strings. This index decomposes the suffix array into stripes which each index only one column (offset from the end) of the input strings. Each stripe is in lexical order, with each element pointing to its suffix in the next stripe, allowing search and decoding via character indices.
