# Striped Index -- fulltext indexing at gigabytes per second
A faster fulltext index for large numbers of short (~80 character) strings. This index decomposes the suffix array into stripes which each index only one column (offset from the end) of the input strings. Each stripe is in lexical order, with each element pointing to its suffix in the next stripe, allowing search and decoding via character indices.
![striped](https://github.com/user-attachments/assets/5c5f3423-c26a-4c9f-8629-3473be09cbda)
<svg width="672" height="615" viewBox="0 0 504 461.25" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="translate(85.875,292.7249984741211) scale(1)">

Figure 1. A Striped Index for "cat$0cab$1abe$2bat$3". Each word is represented by a linked list which may be followed to reconstruct each character by position. Character labels are not stored explicitly and are looked up by range in charindex.

Conventional suffix sorting algorithms handle the multiple string case by concatenating them into a single string, separated by metacharacter terminators $0..$n-1, and feeding that into an algorithm built for a single string, which incurs significant overhead. The Striped Index takes advantage of the fact that comparisons stop at terminators and works backwards ("induced sorting") from their ordering. The main difference is that suffixes are not merged into a single sorted array but are instead kept in columnwise "stripes". 

Producing the stripes is a linear-time process which is largely counting sort; elements representing suffixes are placed into buckets according to 1) their initial characters and 2) the position of the next suffix (the one with the initial character removed) in the next stripe. Proceeding from stripe j+1 to j, we scan j+1 from 0 to n-1 and place each i in j according to the j-th character of the string it represents (we keep a running permutation of the input string pointers for this lookup). 

The inner loop of this bucketing process runs at GB/s; it is mainly bounded by the speed of random pointer dereferencing. 

    for(int i=0; i<n; i++) {
      char c = p[i][j];
      int dst = charindex[j][c]++;
      stripes[j][dst] = i;
      pdst[dst] = p[i];
      }

Figure 2: The inner indexing loop.

## Manageability

One positive aspect of breaking the index into stripes is manageability at large scale, because each stripe is a fraction of the overall index, built one at a time, and immutable thereafter. Once a stripe is populated it can be compressed, flushed to disk, and otherwise removed from concern until search.

To reduce memory footprint the input strings can also be transposed to individual columns in a separate process; the only requirement during indexing is that one column at a time fits in memory or is randomly accessible (since the access pattern is permuted). 

Once built a stripe is a rank/select data structure; any structure that implements select ([]) and finding the rank of a value in a sorted range will work; there are a number of succinct or low-overhead techniques. In particular if compression is used, an index of some sort is typically needed for selection.

Another possibility is to store a stripe as a column in a columnstore data warehouse; these systems implement compression, selection and rank with varying degrees of efficiency.

## Search

The main tradeoff with this technique is that search must proceed from all the stripes instead of one array, since the columns are separated. Hence the emphasis on short records; iterating through a large number of stripes and initiating a search from each one could be prohibitive.

    for(int j= qrylen-1; j<k; j++) {
      bool found = search_from(j, qry, qrylen, los, his);
      if(found)
        printf("search match at %i[%i, %i]\n", j, *los, *his);
      los++; his++;
    }

Figure 3: Searching from each possible offset. Found ranges go into an array.

Right-to-left search is similar to the FM Index, where the last character is looked up in charindex as a range in stripe j, and the second-to-last is then looked up in stripe j-1 and narrowed to the value range in stripe j, and so on iteratively to the beginning of the search string. 

## Compressibility

The simplest implementation of this technique is 4n or 8n (depending on integer size) bytes, but these arrays typically have a bias towards runs of consecutive values, which in BWT format manifest as runs of identical characters. These repetitions are due to characters being highly predicted by their context to the right, in stripe j+1. For instance "uality.." will often be preceded by 'q', and conversely consecutive entries for "quality" will often point to consecutive "uality"'s.

Empirically, it's common to see 75+% of stripe entries be the preceding entry plus one, so the stripes are highly compressible; it's only a matter of choosing a data structure with the right tradeoffs.
