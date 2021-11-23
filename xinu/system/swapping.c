#include <xinu.h>

void set_invpt_entry(uint32 addr, pid32 pid, uint32 pd_offset, uint32 pt_offset, uint32 swp_addr)
{
	uint32 swp_index = (addr & 0x3fff);
	inv_pt* ipt = (inv_pt *)(INV_TABLE_START);

	inv_pt[swp_index].pt_offset = pt_offset;
	inv_pt[swp_index].pd_offset = pd_offset;
	inv_pt[swp_index].swp_addr = swp_addr;
	inv_pt[swp_index].proc_pid = pid;
}

void init_inv_pt()
{
	inv_pt* ipt = (inv_pt *)(INV_TABLE_START);
	int i;
	for (i = 0; i < 16*1024; i++)
	{
		inv_pt[i].proc_pid = 0; // setting it to zero to show that current inv pt entry is invalid
	}
}

int search_LRU_FFS()
{
	inv_pt* ipt = (inv_pt *)(INV_TABLE_START);
	int i = 0;

	for (i = 0; i < 16*1024; i++)
	{
		if (ipt[i].proc_pid != 0)
		{
			pd_t* pd = (pd_t*)proctab[ipt[i].proc_pid].pdbr;
			pt_t* pt = (pt_t*)pd[ipt[i].pd_offset].pt_base;
			if (pt[ipt[i].pt_offset].acc == 0)
			{
				return i;
			}
		}
	}
	return -1;
}

void swapping(uint32 pt_offset, uint32 pd_offset)
{
	pd_t* pd = (pd_t*) proctab[currpid].pdbr;
	pt_t* pt = (pt_t*) pd[pd_offset].pd_base;
	inv_pt* ipt = (inv_pt *)(INV_TABLE_START);
	int id;
	pid32 ppid;
	uint32 addr;

	if (pt[pt_offset].pt_swap == 1) // already present in swap space
	{
		addr = pt[pt_offset].pt_base;
		id = search_LRU_FFS(); // find frame to evict 

		if (id != -1)
		{
			kprintf("FRAME FOUND\n");
			ppid = ipt[id].proc_pid;
			pd_t* pd1 = (pd_t*) proctab[ppid].pdbr;
			pt_t* pt1 = (pt_t*) pd[ipt[id].pd_offset].pd_base;
			if (pt1[ipt[id].pt_offset].pt_swap == 1)
			{
				pt[pt_offset].pt_offset = pt1[ipt[id].pt_offset].pt_offset; // save the new pt to the ffs
				pt1[ipt[id].pt_offset].pt_offset = inv_pt[id].swp_addr;
				pt[pt_offset].pt_pres = 1;
				pt1[ipt[id].pt_offset].pt_pres = 0;
				set_invpt_entry(pt[pt_offset].pt_base, currpid, pd_offset, pt_offset, addr);
			}
			else
			{
				pt[pt_offset].pt_offset = pt1[ipt[id].pt_offset].pt_offset; // save the new pt to the ffs
				pt1[ipt[id].pt_offset].pt_offset = (uint32) getswpmem(4096);
				pt[pt_offset].pt_pres = 1;
				pt1[ipt[id].pt_offset].pt_pres = 0;
				pt1[ipt[id].pt_offset].pt_swap = 1;
				set_invpt_entry(pt[pt_offset].pt_base, currpid, pd_offset, pt_offset, addr);
			}
		}
	}

	else
	{
		id = search_LRU_FFS(); // find frame to evict 

		if (id != -1)
		{
			kprintf("*FRAME FOUND\n");
			ppid = ipt[id].proc_pid;
			pd_t* pd1 = (pd_t*) proctab[ppid].pdbr;
			pt_t* pt1 = (pt_t*) pd[ipt[id].pd_offset].pd_base;
			if (pt1[ipt[id].pt_offset].pt_swap == 1)
			{
				pt[pt_offset].pt_offset = pt1[ipt[id].pt_offset].pt_offset; // save the new pt to the ffs
				pt1[ipt[id].pt_offset].pt_offset = inv_pt[id].swp_addr;
				pt[pt_offset].pt_pres = 1;
				pt1[ipt[id].pt_offset].pt_pres = 0;
				set_invpt_entry(pt[pt_offset].pt_base, currpid, pd_offset, pt_offset, 0);
			}
			else
			{
				pt[pt_offset].pt_offset = pt1[ipt[id].pt_offset].pt_offset; // save the new pt to the ffs
				pt1[ipt[id].pt_offset].pt_offset = (uint32) getswpmem(4096);
				pt[pt_offset].pt_pres = 1;
				pt1[ipt[id].pt_offset].pt_pres = 0;
				pt1[ipt[id].pt_offset].pt_swap = 1;
				set_invpt_entry(pt[pt_offset].pt_base, currpid, pd_offset, pt_offset, 0);
			}
		}
	}
}

char  	*getswpmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
	write_cr3((unsigned long)PT_START);
	mask = disable();
	if (nbytes == 0) {
		//write_cr3((unsigned long)proctab[currpid].pdbr);
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	prev = &swplist;
	curr = swplist.mnext;
	//kprintf("%x -> %u\n", ffslist.mnext, curr->mlength);
	while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			swplist.mlength -= nbytes;
			//write_cr3((unsigned long)proctab[currpid].pdbr);
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			swplist.mlength -= nbytes;
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

void	swpmeminit(void) {

       struct	memblk	*memptr;	/* Ptr to memory block		*/

       /* Initialize the free memory list */

       /* Note: we pre-allocate  the "hole" between 640K and 1024K */
	//maxheap already initialized in i386.c
       maxheap = (char *)(INV_TABLE_START);	/* Assume 64 Mbytes for now */
       minheap = (char *)SWP_START;

       memptr = swplist.mnext = (struct memblk *)roundmb(minheap);
       //pdtlist.mnext = (struct memblk *)PT_START;
       
       	/* initialize free memory list to one block */
       	swplist.mnext = memptr = (struct memblk *) roundmb(FFS_START);
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (uint32) truncmb((uint32)maxheap -
       			(uint32)minheap);

       return;
}

