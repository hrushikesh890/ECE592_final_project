/* getmem.c - getmem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getmem  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */
char  	*getmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
	//write_cr3((unsigned long)PT_START);
	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	prev = &memlist;
	curr = memlist.mnext;
	while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			memlist.mlength -= nbytes;
			//write_cr3((unsigned long)proctab[currpid].pdbr);
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			memlist.mlength -= nbytes;
			//write_cr3((unsigned long)proctab[currpid].pdbr);
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	//write_cr3((unsigned long)proctab[currpid].pdbr);
	restore(mask);
	return (char *)SYSERR;
}

char  	*getptmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
	//write_cr3((unsigned long)PT_START);
	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	prev = &pdtlist;
	curr = pdtlist.mnext;
	
	while (curr != NULL) {			/* Search free list	*/
		//kprintf(" %x %x %x -> %u %d\n", prev, curr, pdtlist.mnext, curr->mlength, nbytes);
		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			pdtlist.mlength -= nbytes;
			//write_cr3((unsigned long)proctab[currpid].pdbr);
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			//kprintf("should be here %x\n", (uint32)curr);
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			//kprintf("leftover = %x\n", (uint32)(leftover));
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			//kprintf("leftover = %x \n", (uint32)(leftover));
			pdtlist.mlength -= nbytes;
			//write_cr3((unsigned long)proctab[currpid].pdbr);
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	//write_cr3((unsigned long)proctab[currpid].pdbr);
	restore(mask);
	
	return (char *)SYSERR;
}

char  	*getffsmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
	write_cr3((unsigned long)PT_START);
	mask = disable();
	if (nbytes == 0) {
		write_cr3((unsigned long)proctab[currpid].pdbr);
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	prev = &ffslist;
	curr = ffslist.mnext;
	//kprintf("%x -> %u\n", ffslist.mnext, curr->mlength);
	while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			ffslist.mlength -= nbytes;
			//write_cr3((unsigned long)proctab[currpid].pdbr);
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			ffslist.mlength -= nbytes;
			//write_cr3((unsigned long)proctab[currpid].pdbr);
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	write_cr3((unsigned long)proctab[currpid].pdbr);
	restore(mask);
	return (char *)SYSERR;
}
