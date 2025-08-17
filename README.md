# striped_index
A faster fulltext index for large numbers of short (~80 character) strings. This index decomposes the suffix array into stripes which each index only one column (offset from the end) of the input strings. Each stripe is in lexical order, with each element pointing to its suffix in the next stripe, allowing search and decoding via character indices.
![striped](https://github.com/user-attachments/assets/5c5f3423-c26a-4c9f-8629-3473be09cbda)
<svg width="672" height="615" viewBox="0 0 504 461.25" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="translate(85.875,292.7249984741211) scale(1)">
<title>stripes</title>
<polygon fill="white" stroke="none" points="-4,4 -4,-128.2 328,-128.2 328,4 -4,4"/>
<!-- stripe0 -->
<g id="node1" class="node">
<title>stripe0</title>
<polygon fill="none" stroke="black" points="0,-12.5 0,-111.7 54,-111.7 54,-12.5 0,-12.5"/>
<text text-anchor="middle" x="27" y="-95.1" font-family="Times,serif" font-size="14.00">a 3</text>
<polyline fill="none" stroke="black" points="0,-86.9 54,-86.9"/>
<text text-anchor="middle" x="27" y="-70.3" font-family="Times,serif" font-size="14.00">b 2</text>
<polyline fill="none" stroke="black" points="0,-62.1 54,-62.1"/>
<text text-anchor="middle" x="27" y="-45.5" font-family="Times,serif" font-size="14.00">c 0</text>
<polyline fill="none" stroke="black" points="0,-37.3 54,-37.3"/>
<text text-anchor="middle" x="27" y="-20.7" font-family="Times,serif" font-size="14.00">c 1</text>
</g>
<!-- stripe1 -->
<g id="node2" class="node">
<title>stripe1</title>
<polygon fill="none" stroke="black" points="90,-0.5 90,-99.7 144,-99.7 144,-0.5 90,-0.5"/>
<text text-anchor="middle" x="117" y="-83.1" font-family="Times,serif" font-size="14.00">a 0</text>
<polyline fill="none" stroke="black" points="90,-74.9 144,-74.9"/>
<text text-anchor="middle" x="117" y="-58.3" font-family="Times,serif" font-size="14.00">a 2</text>
<polyline fill="none" stroke="black" points="90,-50.1 144,-50.1"/>
<text text-anchor="middle" x="117" y="-33.5" font-family="Times,serif" font-size="14.00">a 3</text>
<polyline fill="none" stroke="black" points="90,-25.3 144,-25.3"/>
<text text-anchor="middle" x="117" y="-8.7" font-family="Times,serif" font-size="14.00">b 1</text>
</g>
<!-- stripe0&#45;&gt;stripe1 -->
<g id="edge1" class="edge">
<title>stripe0:0-&gt;stripe1:3</title>
<path fill="none" stroke="black" d="M54,-99.3C90.89,-99.3 57.95,-31.37 79.02,-15.98"/>
<polygon fill="black" stroke="black" points="79.86,-19.38 88.54,-13.31 77.97,-12.64 79.86,-19.38"/>
</g>
<!-- stripe0&#45;&gt;stripe1 -->
<g id="edge2" class="edge">
<title>stripe0:1-&gt;stripe1:2</title>
<path fill="none" stroke="black" d="M54,-74.5C72.23,-74.5 69.74,-51.13 79.55,-41.64"/>
<polygon fill="black" stroke="black" points="80.46,-45.04 88.58,-38.23 77.99,-38.49 80.46,-45.04"/>
</g>
<!-- stripe0&#45;&gt;stripe1 -->
<g id="edge3" class="edge">
<title>stripe0:2-&gt;stripe1:0</title>
<path fill="none" stroke="black" d="M54,-49.7C72.44,-49.7 69.66,-73.58 79.48,-83.28"/>
<polygon fill="black" stroke="black" points="78,-86.46 88.59,-86.76 80.5,-79.92 78,-86.46"/>
</g>
<!-- stripe0&#45;&gt;stripe1 -->
<g id="edge4" class="edge">
<title>stripe0:3-&gt;stripe1:1</title>
<path fill="none" stroke="black" d="M54,-24.9C72.44,-24.9 69.66,-48.78 79.48,-58.48"/>
<polygon fill="black" stroke="black" points="78,-61.66 88.59,-61.96 80.5,-55.12 78,-61.66"/>
</g>
<!-- stripe2 -->
<g id="node3" class="node">
<title>stripe2</title>
<polygon fill="none" stroke="black" points="180,-12.5 180,-111.7 234,-111.7 234,-12.5 180,-12.5"/>
<text text-anchor="middle" x="207" y="-95.1" font-family="Times,serif" font-size="14.00">b 1</text>
<polyline fill="none" stroke="black" points="180,-86.9 234,-86.9"/>
<text text-anchor="middle" x="207" y="-70.3" font-family="Times,serif" font-size="14.00">e 2</text>
<polyline fill="none" stroke="black" points="180,-62.1 234,-62.1"/>
<text text-anchor="middle" x="207" y="-45.5" font-family="Times,serif" font-size="14.00">t 0</text>
<polyline fill="none" stroke="black" points="180,-37.3 234,-37.3"/>
<text text-anchor="middle" x="207" y="-20.7" font-family="Times,serif" font-size="14.00">t 3</text>
</g>
<!-- stripe1&#45;&gt;stripe2 -->
<g id="edge5" class="edge">
<title>stripe1:0-&gt;stripe2:0</title>
<path fill="none" stroke="black" d="M144,-87.3C156.25,-87.3 160.8,-93.63 168.84,-97.1"/>
<polygon fill="black" stroke="black" points="168.03,-100.51 178.52,-99.01 169.38,-93.64 168.03,-100.51"/>
</g>
<!-- stripe1&#45;&gt;stripe2 -->
<g id="edge6" class="edge">
<title>stripe1:1-&gt;stripe2:2</title>
<path fill="none" stroke="black" d="M144,-62.5C156.34,-62.5 160.79,-55.74 168.81,-52.05"/>
<polygon fill="black" stroke="black" points="169.45,-55.49 178.52,-50.01 168.01,-48.64 169.45,-55.49"/>
</g>
<!-- stripe1&#45;&gt;stripe2 -->
<g id="edge7" class="edge">
<title>stripe1:2-&gt;stripe2:3</title>
<path fill="none" stroke="black" d="M144,-37.7C156.34,-37.7 160.79,-30.94 168.81,-27.25"/>
<polygon fill="black" stroke="black" points="169.45,-30.69 178.52,-25.21 168.01,-23.84 169.45,-30.69"/>
</g>
<!-- stripe1&#45;&gt;stripe2 -->
<g id="edge8" class="edge">
<title>stripe1:3-&gt;stripe2:1</title>
<path fill="none" stroke="black" d="M144,-12.9C171,-12.9 155.13,-57.57 169.4,-70.83"/>
<polygon fill="black" stroke="black" points="167.97,-74.04 178.57,-74 170.26,-67.43 167.97,-74.04"/>
</g>
<!-- stripe3 -->
<g id="node4" class="node">
<title>stripe3</title>
<polygon fill="none" stroke="black" points="270,-24.5 270,-123.7 324,-123.7 324,-24.5 270,-24.5"/>
<text text-anchor="middle" x="297" y="-107.1" font-family="Times,serif" font-size="14.00">$0</text>
<polyline fill="none" stroke="black" points="270,-98.9 324,-98.9"/>
<text text-anchor="middle" x="297" y="-82.3" font-family="Times,serif" font-size="14.00">$1</text>
<polyline fill="none" stroke="black" points="270,-74.1 324,-74.1"/>
<text text-anchor="middle" x="297" y="-57.5" font-family="Times,serif" font-size="14.00">$2</text>
<polyline fill="none" stroke="black" points="270,-49.3 324,-49.3"/>
<text text-anchor="middle" x="297" y="-32.7" font-family="Times,serif" font-size="14.00">$3</text>
</g>
<!-- stripe2&#45;&gt;stripe3 -->
<g id="edge9" class="edge">
<title>stripe2:0-&gt;stripe3:1</title>
<path fill="none" stroke="black" d="M234,-99.3C246.34,-99.3 250.79,-92.54 258.81,-88.85"/>
<polygon fill="black" stroke="black" points="259.45,-92.29 268.52,-86.81 258.01,-85.44 259.45,-92.29"/>
</g>
<!-- stripe2&#45;&gt;stripe3 -->
<g id="edge10" class="edge">
<title>stripe2:1-&gt;stripe3:2</title>
<path fill="none" stroke="black" d="M234,-74.5C246.34,-74.5 250.79,-67.74 258.81,-64.05"/>
<polygon fill="black" stroke="black" points="259.45,-67.49 268.52,-62.01 258.01,-60.64 259.45,-67.49"/>
</g>
<!-- stripe2&#45;&gt;stripe3 -->
<g id="edge11" class="edge">
<title>stripe2:2-&gt;stripe3:0</title>
<path fill="none" stroke="black" d="M234,-49.7C261,-49.7 245.13,-94.37 259.4,-107.63"/>
<polygon fill="black" stroke="black" points="257.97,-110.84 268.57,-110.8 260.26,-104.23 257.97,-110.84"/>
</g>
<!-- stripe2&#45;&gt;stripe3 -->
<g id="edge12" class="edge">
<title>stripe2:3-&gt;stripe3:3</title>
<path fill="none" stroke="black" d="M234,-24.9C246.25,-24.9 250.8,-31.23 258.84,-34.7"/>
<polygon fill="black" stroke="black" points="258.03,-38.11 268.52,-36.61 259.38,-31.24 258.03,-38.11"/>
</g>
</g>
</svg>


Figure 1. A Striped Index for "cat$0cab$1abe$2bat$3". Character labels are not stored explicitly and are looked up by range in charindex.

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

To reduce memory footprint the input strings can also be transposed to individual columns; the only requirement during indexing is that one column at a time fits in memory or is randomly accessible (since the access pattern is permuted). 

Once built a stripe is a rank/select data structure; any structure that implements select ([]) and finding the rank of a value in a sorted range will work; there are a number of succinct or low-overhead techniques. In particular if compression is used, an index of some sort is typically needed for selection.

Another possibility is to store a stripe as a column in a columnstore data warehouse; these systems implement selection and rank with varying degrees of efficiency.

## Search

The main tradeoff with this technique is that search must proceed from all the stripes instead of one array, since the columns are separated. Hence the emphasis on short records; iterating through a large number of stripes and initiating a search from each one could be prohibitive.

Right-to-left search is similar to the FM Index, where the last character is looked up in charindex as a range in stripe j, and the second-to-last is then looked up in stripe j-1 and narrowed to the value range in stripe j, and so on iteratively to the beginning of the search string. 

## Compressibility

The simplest implementation of this technique is 4n or 8n (depending on integer size) bytes, but these arrays typically have a bias towards runs of consecutive values, which in BWT format manifest as runs of identical characters. These repetitions are due to characters being highly predicted by their context to the right, in stripe j+1. For instance "uality.." will often be preceded by 'q', and conversely consecutive entries for "quality" will often point to consecutive "uality"'s.

Empirically, it's common to see 75+% of stripe entries be the preceding entry plus one, so the stripes are highly compressible; it's only a matter of choosing a data structure with the right tradeoffs.
