# Striped Index -- Fulltext Indexing at Gigabytes per Second

A compressed self-index for large numbers of short strings, which trades a minor increase in search overhead for a major (GB/s) increase in indexing speed. It is ideal for situations where an inverted index falls short, and arbitrary character sequences may need to be located. Datasets with many short strings are common in databases and data warehouses, possibly more prevalent than single large texts, but specialized algorithms for this regime have not been developed.

This index provides substring search capability comparable to a suffix tree or the FM index, in storage near the optimal compressed size, with less overhead than previous approaches -- indexing in minutes rather than days. 

As with other self-indexes, it provides four main functions:
 A. Index -- process a dataset into indexed form
 B. Count -- count the number of occurrences of a pattern in the dataset
 C. Locate -- list the records which contain the pattern
 D. Decode -- decode the text context of each pattern occurrence 

The index can reside in secondary storage with blockwise compression. Search operations do not need the entire index in RAM. The index is immutable, so updates to the source data cannot be reflected in the index except by rebuilding. It is more suitable for workloads such as analytics where data remains static.

Possible applications include:
 1. Fixed string search, eg "all records which contain 'exp('"
 2. Regular expression search, eg "all records containing a match for '2021-0[123]-[03][0-9]'"
 3. Search-as-you-type, where each keystroke extends the search incrementally.
 4. log search
 5. AI observability 



## Background

A number of compressed full text indexes have been developed which provide rapid string search at the cost of an initial indexing computation. The FM Index was developed to provide substring lookups in text which has undergone the Burrows-Wheeler Transform (BWT), originally a compression technique. The Suffix Array provides a similar function. In either structure, to find all occurrences of a substring one only needs to find the range of sorted suffixes which are prefixed by it.
 
The FM Index applies a technique, backwards search, which utilizes links between suffixes which are inherent to the BWT format. These same links are used to decode the BWT-transformed text back to the original.

The bulk of work in any index of this type is a suffix sort, where all suffixes of a text are placed into lexicographical order. Suffix sorting has linear complexity, but in practice it has a large constant factor, consuming many instructions per character. The cost of indexing may outweigh the benefits.




Saving each stage of LSD Radix Sort decomposes the suffix array into *stripes* which each index only one column (offset from the end) of the input. Radixwise backward search is similar to the FM Index, with the additional cost of starting from every column. 

Each stripe is in lexical order, with each element pointing to its suffix in the next stripe, allowing search and decoding via character indices.
![striped](https://github.com/user-attachments/assets/5c5f3423-c26a-4c9f-8629-3473be09cbda)
<svg width="672" height="615" viewBox="0 0 504 461.25" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="translate(85.875,292.7249984741211) scale(1)">

Figure 1. A Striped Index for "cat$0cab$1abe$2bat$3". Each word is represented by a linked list which may be followed to reconstruct each character by position. Character labels are not stored explicitly and are looked up by range in charindex.

Historically, suffix sorting algorithms have handled the multiple string case by concatenating them into a single string, separated by metacharacter terminators $0..$n-1, and feeding that into an algorithm built for a single string. These implementations, though often linear-time, still take hundreds of instructions per character; throughput of a few GB per hour is typical.

The Striped Index takes advantage of the fact that comparisons stop at terminators which have a pre-assigned order and act as tiebreakers in string comparisons. During sorting, these "fixed points" provide an initial ordering of the input records (technically it's the ordering of the empty, length 0 suffix that every record has).

Producing the stripes is a linear-time process which is largely LSD Radix Sort. Working from right to left, elements representing suffixes are sorted according to 1) their initial characters and 2) the position of their next suffix (the one with the initial character removed) in the stripe to the right. Unlike radix sort however we record each suffix as a pointer (index) into the next stripe; each suffix points to the next, eg "cat" -> "at" -> "t", and we use this structure for search and decoding. 

This linked list structure was originally based on the LF array that BWT-based compressors build during decoding; it's quick to follow for decoding but also a bit easier to envision compared to the BWT L array, which 

As with radix sort we keep a running permutation of the input string pointers.

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

### Larger than Memory Case

In-memory indexing uses an array of pointers which reference the input in its original order. This array is permuted with each stripe, resulting in an effectively random access pattern between the pointers and the strings they reference. Another possibility when a large enough RAM is not available is to permute the strings instead of the pointers, trading a larger amount of I/O to obtain a sequential access pattern at each stage. For each stripe, input strings are read sequentially and split into 256 output streams, which must be concatenated to form the next input. This technique works with a fixed amount of memory but a great deal of I/O (about k(k-1)/2 bytes of writing for each record of length k). Another possibility is a "fat pointer" that caches a chunk of each record alongside the pointer to the original data; even the in-memory case may benefit.

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
