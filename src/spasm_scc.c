#include <assert.h>
#include "spasm.h"


/*
 * returns the non-trivial strongly connected component of A (which must be
 * square), seen as an directed graph.
 */
/* r must have size n+1 */

/* strategie : quand un noeud est retiré dans une SCC, mettre son index ou son lowlink à n+1 */

spasm_partition *spasm_strongly_connected_components(const spasm * A) {
	int n = A->n;
	int m = A->m;
	int *Ap = A->p;
	int *Aj = A->j;
	assert(n == m);

	spasm_partition *P = spasm_partition_alloc(n, n, n, n);
	int p_top = 0;
	int *p = P->p;
	int *rr = P->rr;

	int *pstack = spasm_malloc(n * sizeof(int));
	int *marks = spasm_malloc(n * sizeof(int));
	int *prev = spasm_malloc(n * sizeof(int));
	int *stack = spasm_malloc(n * sizeof(int));
	int *lowlink = spasm_malloc(n * sizeof(int));
	
	/* first pass */
	for (int i = 0; i < n; i++) {
		marks[i] = -1;
		prev[i] = -1;
		pstack[i] = Ap[i];
	}

	int n_scc = 0;
	int index = 0;
	rr[n_scc] = 0;
	for (int i = 0; i < n; i++) {
		int head, top;
		fprintf(stderr, "starting on %d\n", i);
		if (marks[i] >= 0)
			continue;
		
		/* DFS */
		head = 0;
		top = 0;
		stack[top] = i;
		int j = i;
		while (j >= 0) {
			/* get j from the top of the recursion stack */
			int px, px2;
			fprintf(stderr, "iterating, j = %d\n", j);
			if (marks[j] < 0) {
				/* init */
				fprintf(stderr, "index[%d] = %d\n", j, index);
				lowlink[j] = index;
				marks[j] = index++;
			}
			px2 = Ap[j + 1];
			for (px = pstack[j]; px < px2; px++) {
				int k = Aj[px];
				fprintf(stderr, "[px=%d] edge %d --> %d\n", px, j, k);

				if (marks[k] >= 0) {
					/* update */
					lowlink[j] = spasm_min(lowlink[j], lowlink[k]);
					continue;
				}
				/* push */
				pstack[j] = px + 1;
				stack[++top] = k;
				prev[k] = j;
				j = k;
				break;
			}
			if (px == px2) {
				fprintf(stderr, "poping %d\n", j);
				/* check if we have the root of a SCC */
				if (lowlink[j] == marks[j]) {
					fprintf(stderr, "SCC found (root %d): ", j);
					while (stack[top] != j) {
						int k = stack[top--];
						p[p_top++] = k;
						lowlink[k] = n;
						fprintf(stderr, "%d ", k);					
					}
					p[p_top++] = j;
					lowlink[j] = n;
					top--;

					rr[++n_scc] = p_top;
					fprintf(stderr, "\n");
				} else {
					fprintf(stderr, "lowlink[%d] = %d\n", j, lowlink[j]);
				}
				/* pop */
				int k = j;
				j = prev[j];
				if (j >= 0)
					lowlink[j] = spasm_min(lowlink[j], lowlink[k]);
			}
		}
	}
	assert (p_top == n);

	/* at this point, blocks are in reverse order, and inside blocks, nodes are in reverse order */
	int *q = P->q;
	int *cc = P->cc;
	for (int i = 0; i < n; i++)
		q[i] = p[n - 1 - i];

	for (int i = 0; i < n; i++)
		p[i] = q[i];

	for (int i = 0; i <= n_scc; i++)
		cc[i] = n - rr[n_scc - i];

	for (int i = 0; i <= n_scc; i++)
		rr[i] = cc[i];


	P->nr = n_scc;
	P->nc = n_scc;

	free(stack);
	free(pstack);
	free(marks);
	free(lowlink);
	spasm_partition_tighten(P);
	return P;
}
