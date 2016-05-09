#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include "spasm.h"
int main() {
  spasm_triplet *A;
  int m, nz, k;
  int *Aj;

  A = spasm_load_sms(stdin, 42013);
  
  m = A->m;
  nz = A->nz;
  Aj = A->j;
  
  for (k = 0; k < nz; k++) {
      Aj[k] = m - Aj[k];
  }

  spasm_save_triplet(stdout, A);
  spasm_triplet_free(A);
  return 0;
}