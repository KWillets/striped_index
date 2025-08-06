#include <stdio.h>
#include <utility>
#include <algorithm>
/*
  stripe merge into full array A

  1. build stripe with pointers into A
     requires previous insertion points
  2. build list of insertion points in A (stripe <c, value> --> rank in A)
     
  3. scan A (backwards) and add rank in insertion list to each value, index
    or maybe some kind of fifo to move elements forward to new rank

  values are in sigma ascending runs, merge appropriately

  ie last elt moves n, until last insertion point is passed then n-1, etc.
  
*/
/*
  columnstore table with one column per stripe
  create table ix(
   i int, // encoding deltaval
   c1 char(1), // encoding rle
   s1 int, // encoding auto ?
   c2....
  ) order by i

  range reverse search: select min(i), max(i) where c1 = 't'
   and s1 >= last_min and i <= last_max

   multiple ranges vis join


   traverse c1--> c2
   select c2 as current_i from ix 
   where i = current_i

   recursive sql also works
   
   also works for c1..ck multikey
 */
class StripedIndex{
public:

  int k,n;
  
  int **charindex;

  int **stripes;

  StripedIndex(int kk, int nn): k(kk), n(nn) {

    charindex = new int *[k];

    for(int i=0; i<k; i++)
      charindex[i] = new int[256](); // todo destructor
    
    stripes = new int *[k];
  };

  void index(const char **str) {

    // count
    for(int i=0; i<n; i++)
      for(int j=0; j<k; j++)
	charindex[j][str[i][j]]++;

    // i-1 prefix sum
    for(int j=0; j<k; j++) {
      int *ix = charindex[j];

      ix[255]=n-ix[255];
      for(int i=254; i>=0; i--)
	ix[i]=ix[i+1]-ix[i];
    }
    
    const char **p = new const char *[n];
    const char **q = new const char *[n];

    index_stripe(k-1, str, q);

    for(int j=k-2; j >= 0; j--) {
      printf("stripe %i\n", j);
      std::swap(p,q);
      index_stripe(j,p,q);
    }
    delete[] p;
    delete[] q;
  };

  void index_stripe(int j, const char **p, const char **pdst) {
    stripes[j] = new int[n]();
    for(int i=0; i<n; i++) {
      char c = p[i][j];
      int dst =	charindex[j][c]++;
      stripes[j][dst] = i;
      pdst[dst] = p[i];
    }
  }

  void decode(int j, int i, char *out) {
    for(; j < k; j++) {
      int *s = stripes[j];
      int *ix = charindex[j];
      // lookup i
      if( out ) {
	int *p = std::upper_bound(ix, ix+256, i);
	int c = p-ix;
	*out++ = c;
      }
      //      printf("%c[%i] ", c, i);
      i = s[i];
    }
  }
  
  /* find [vlo,vhi] in the c bucket of stripe j, return range as indices, closed interval */
  bool newrange(int j, char c, int vlo, int vhi, int *ixlo, int*ixhi) {
    int *s = stripes[j];
    // set search range [lo,hi) to char c bucket
    int *lo = s + (c == 0 ? 0 : charindex[j][c-1]);
    int *hi = s + charindex[j][c];

    printf("searching for [%i,%i] in stripe %i[%li,%li)\n", vlo, vhi, j, lo-s, hi-s);
    
    // contract sorted range to values in [vlo, vhi]
    int* newlo = std::lower_bound(lo, hi, vlo); // first elt >= vlo
    int* newhi = std::upper_bound(newlo, hi, vhi); // first elt > vhi or end. can use newlo as start

    *ixlo = newlo-s;
    *ixhi = newhi-s-1; // half open to closed

    return newlo != newhi;
    }

  // create pretty graphviz
  void dump_index() {
    printf("digraph stripes { node [\nshape=record];\nrankdir=\"LR\";\n");

    for(int j=0; j<k; j++)
      dump_stripe(j);
    
    printf("stripe%i[label=\"", k);
    for(int i=0; i<n-1; i++)
      printf("<%i> $%i |",i, i);
    printf("<%i> $%i \"]",n-1, n-1);
    printf("\n}\n");
  };
  
  void dump_stripe(int j) {
    int * s=charindex[j];
    if(!s)
      return;
    
    printf("stripe%i[label=\"", j);
    char sep =' ';
    for(int c=0; c < 127; c++) {

      int lo= c ? s[c-1] : 0;
      if(lo < s[c]) {

	printf("%c ",sep);
	sep = '|';
	
	for(int i=lo; i<s[c]-1; i++)
	  printf(" <%i> %c %i |", i, c, stripes[j][i]);
	printf(" <%i> %c %i ", s[c]-1, c, stripes[j][s[c]-1]); 
      }
    };
    printf("\"];\n");
    for(int i=0; i<n; i++)
      printf("stripe%i:%i -> stripe%i:%i\n", j,i,j+1,stripes[j][i]);
  };


};


int main() {
  
  const char * strings[] = {"road", "bead", "boat","load"};
  
  StripedIndex si(4,4);
  printf("indexing\n");
  si.index(strings);
  printf("dumping\n");
  si.dump_index();
  printf("decode\n");
  char out[5];
  si.decode(0,0, out);
  printf("decoded %s\n", out);
  
  printf("search\n");
  // "at" search from stripe 2
  int tlo= si.charindex[2]['a'-1];
  int thi= si.charindex[2]['a']-1;
  int newhi, newlo;
  bool ok = si.newrange(1,'e',tlo, thi, &newlo, &newhi);

  if(ok) {
    printf("search new range is [%i, %i]\n", newlo, newhi);
  } else
    printf("search failed\n");

  printf("\n end \n\n");
  
}
