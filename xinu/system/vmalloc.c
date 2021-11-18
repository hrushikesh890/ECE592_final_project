#include <xinu.h>

// char* vmalloc(uint32 nbytes)
// {
// 	intmask mask;
// 	struct vmemblk *prev, *curr, *leftover;
	
// 	mask = disable();

// 	if(nbytes == 0){
// 		restore(mask);
// 		return (char *)SYSERR;
// 	}

// 	nbytes = (uint32) roundmb(nbytes);
// 	prev = &ffslist;
// 	curr = memlist.mnext;

// 	while(curr != NULL){

// 		if (curr->mlength == nbytes) {
// 			prev->mnext = curr->mnext;
// 			memlist.mlength -= nbytes;
// 			restore(mask);
// 			return (char *)(curr);

// 		}

// 		else if (curr->mlength > nbytes) {
// 			leftover = (struct memblk *)((uint32) curr +
// 					nbytes);
// 			prev->mnext = leftover;
// 			leftover->mnext = curr->mnext;
// 			leftover->mlength = curr->mlength - nbytes;
// 			memlist.mlength -= nbytes;
// 			restore(mask);
// 			return (char *)(curr);
// 		}
// 	}

// }

char* vmalloc(uint32 nbytes){
	intmask mask;
	pd_t* pd_base = proctab[currpid].pdbr;

	mask = disable();

	if(nbytes == 0){
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);
	uint32 npages = nbytes/4096;
	proctab[currpid].pages_alloc += npages;
	write_cr3((unsigned long) pd_base);
	int i;
	for(i=0;i<1024;i++){
		if(pd_base[i].pd_pres == 0){
			pd_base[i].pd_pres = 1;
			pd_base[i].pd_write = 1;
			//pt_t* pt_base = (pt_t*)(getffsmem(nbytes));

			// for(i=0;i<1024;i++){
			// 	pt_base[i].pt_pres = 0;
			// 	pt_base[i].pt_write = 1;
			// }
		}
	}
}
