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

  void index(const unsigned char **str) {

    printf("counts n=%i k=%i\n", n, k);
    
    // count
    for(int i=0; i<n; i++)
      for(int j=0; j<k; j++)
	charindex[j][str[i][j]]++;

    printf("prefix sum %i stripes\n", k);
    
    // i-1 prefix sum
    for(int j=0; j<k; j++) {
      int *ix = charindex[j];

      ix[255]=n-ix[255];
      for(int i=254; i>=0; i--)
	ix[i]=ix[i+1]-ix[i];
    }
    
    const unsigned char **p = new const unsigned char *[n];
    const unsigned char **q = new const unsigned char *[n];

    index_stripe(k-1, str, q);

    for(int j=k-2; j >= 0; j--) {
      printf("stripe %i\n", j);
      std::swap(p,q);
      index_stripe(j,p,q);
    }
    delete[] p;
    delete[] q;
  };

  void index_stripe(int j, const unsigned char **p, const unsigned char **pdst) {
    stripes[j] = new int[n]();
    for(int i=0; i<n; i++) {
      unsigned char c = p[i][j];
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
	//printf("%c", c);
      }
      //printf("[%i] ", i);

      i = s[i];
    }
    *out = 0;
  }
  
  /* find [vlo,vhi] in the c bucket of stripe j, return range as indices, closed interval */
  bool newrange(int j, char c, int *vlo, int *vhi) {
    int *s = stripes[j];
    // set search range [lo,hi) to char c bucket
    int clo = (c == 0 ? 0 : charindex[j][c-1]);
    int chi = charindex[j][c];

    printf("newrange: %c[%i, %i)\n", c, clo, chi);

    if(*vhi < 0) { // starting qry
      *vlo = clo;
      *vhi = chi-1;
    } else {
      // contract sorted range to values in [vlo, vhi]
      int *lo = s + clo;
      int *hi = s + chi;
      printf("searching for [%i,%i] in stripe %i[%li,%li)\n", *vlo, *vhi, j, lo-s, hi-s);
      int *newlo = std::lower_bound(lo, hi, *vlo); // first elt >= vlo
      int *newhi = std::upper_bound(newlo, hi, *vhi); // first elt > vhi or end. can use newlo as start

      *vlo = newlo-s;
      *vhi = newhi-s-1; // half open to closed
    }
    return *vlo <= *vhi;
    }

  bool search_from(int j, char *qry, int qrylen, int *lo, int *hi) {

    *lo= *hi= -1;

    printf("search from %i for %s\n", j, qry);

    int first = j-qrylen;
    for(; j > first && newrange(j, qry[--qrylen], lo, hi); j--)
      ;

    if( j == first )
      return true;
    else {
      *lo=*hi=-1;
      return false;
    }
  }

  void search(char *qry, int *los, int *his) {
    int qrylen = strlen(qry);
    printf("search start %s %i\n", qry, qrylen);
    for(int j= qrylen-1; j<k; j++) {
      bool found = search_from(j, qry, qrylen, los, his);
      if(found)
	printf("search match at %i[%i, %i]\n", j, *los, *his);
      los++; his++;
    }
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

  void dump_stats() {
    int *freq = new int[100]();
    for(int j=0; j<k; j++)
      for(int i=1; i<n; i++) {
	int d = stripes[j][i]-stripes[j][i-1];
	if(d>=0 and d<100)
	  freq[d]++;
      };
    for(int d=1; d<100; d++)
      printf("freq[%i]=%i\n", d, freq[d]);
  }


};


int main() {
  
  const unsigned char * strings[1000];
  unsigned char strheap[1000*100];

  FILE *f = fopen("sample.txt", "r");

  
  int i=0;
  for(  i = 0; i < 1000; i++) {
    unsigned char *p = strheap + i*100;
    if( !fgets((char *) p, 100, f))
      break;

    //for(int l = strlen((char *)p)-1; l < 81; l++)
    //  p[l]=' ';

    strings[i]=p;
  }

  int n = i;
  printf("%i records read\n", n);

  StripedIndex si(80,n);
  printf("indexing\n");
  si.index(strings);
  //printf("dumping\n");
  //  si.dump_index();
  si.dump_stats();
  printf("decode\n");
  char out[100];
  si.decode(0, n/2, out);
  printf("decoded [%s]\n", out);
  si.decode(0, 0, out);
  printf("decoded [%s]\n", out);
  si.decode(0, 250, out);
  printf("decoded [%s]\n", out);
  
  printf("search\n");
  //  search from stripe 
  char qry[] = "7 eleven";

  int lo= -1;
  int hi= -1;
  bool found = si.search_from(strlen(qry)-1, qry, strlen(qry), &lo, &hi);
  //int j;
  //for(j=strlen(qry)-1; j >= 0 && si.newrange(j, qry[j],&lo, &hi); j--)
  //  ;

  if(found) {
    printf("found at [%i, %i]\n", lo,hi);

    printf("%i matches\n", hi-lo+1);
    for(int i=lo; i<=hi; i++) {
      si.decode(0, i, out);
      printf("match [%s]\n", out);
    }
  }

  char qry2[] = "7 eleven";
  int* los = new int[80]();
  int* his = new int[80]();

  si.search(qry2, los, his);

  for(int j=0; j<80; j++) {
    if(los[j] >= 0) {
      printf("search found %s at %i[ %i, %i]\n", qry2, j, los[j],his[j]);
    }
  }
  
  printf("\n end \n\n");
  
}
