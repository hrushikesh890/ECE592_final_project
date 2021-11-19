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

	write_cr3((unsigned long)PT_START);

	if(nbytes == 0){
		restore(mask);
		return (char *)SYSERR;
	}

	//nbytes = (uint32) roundmb(nbytes);
	uint32 npages = ceil(nbytes/4096);
	proctab[currpid].pages_alloc += npages;
	int i;
	for(i=9;i<1024;i++){
		if(pd_base[i].pd_pres == 0){
			pd_base[i].pd_pres = 1;
			pd_base[i].pd_write = 1;
			pt_t* pt_base = (pt_t*)(getptmem(4096));
			pd_base[i].pd_base = (uint32) (((uint32) pt_base >> 12) & (0x0FFFFF));

			for(i=0;i<1024;i++){
				pt_base[i].pt_pres = 0;
				pt_base[i].pt_write = 1;
				pt_base[i].pt_valid = 1;
			}
		}
	}

	write_cr3((unsigned long) proctab[currpid].pdbr);
}
