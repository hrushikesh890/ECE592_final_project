#include <xinu.h>

/* Memory bounds */

void	*minheap;		/* Start of heap			*/
void	*maxheap;		/* Highest valid heap address		*/

/*------------------------------------------------------------------------
 * meminit - initialize memory bounds and the free memory list
 *------------------------------------------------------------------------
 */
void	meminit(void) {

       struct	memblk	*memptr;	/* Ptr to memory block		*/

       /* Initialize the free memory list */

       /* Note: we pre-allocate  the "hole" between 640K and 1024K */
	//maxheap already initialized in i386.c
//       maxheap = (void *)0x600000;	/* Assume 64 Mbytes for now */
       minheap = &end;

       memptr = memlist.mnext = (struct memblk *)roundmb(minheap);
       //pdtlist.mnext = (struct memblk *)PT_START;
       if ((char *)(maxheap+1) > HOLESTART) {
       	/* create two blocks that straddle the hole */
       	memptr->mnext = (struct memblk *)HOLEEND;
       	memptr->mlength = (int) truncmb((unsigned) HOLESTART -
            		 (unsigned)&end - 4);
       	memptr = (struct memblk *) HOLEEND;
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (int) truncmb( (uint32)maxheap - 
       			(uint32)HOLEEND - NULLSTK);
       } else {
       	/* initialize free memory list to one block */
       	memlist.mnext = memptr = (struct memblk *) roundmb(&end);
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (uint32) truncmb((uint32)maxheap -
       			(uint32)&end - NULLSTK);
       }

       return;
}

void	ptmeminit(void) {

       struct	memblk	*memptr;	/* Ptr to memory block		*/

       
       maxheap = (char *)(FFS_START);	/* Assume 64 Mbytes for now */
       minheap = (char *)(PT_START);

        memptr = pdtlist.mnext = (struct memblk *)roundmb(minheap);
       
       	/* initialize free memory list to one block */
       	pdtlist.mnext = memptr = (struct memblk *) roundmb(minheap);
       	//pdtlist.mlength = (uint32) truncmb((uint32)maxheap -     			(uint32)minheap);
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (uint32) truncmb((uint32)maxheap -
       			(uint32)minheap);

       return;
}

void	ffsmeminit(void) {

       struct	memblk	*memptr;	/* Ptr to memory block		*/

       /* Initialize the free memory list */

       /* Note: we pre-allocate  the "hole" between 640K and 1024K */
	//maxheap already initialized in i386.c
       maxheap = (char *)(SWP_START);	/* Assume 64 Mbytes for now */
       minheap = (char *)FFS_START;

       memptr = ffslist.mnext = (struct memblk *)roundmb(minheap);
       //pdtlist.mnext = (struct memblk *)PT_START;
       
       	/* initialize free memory list to one block */
       	ffslist.mnext = memptr = (struct memblk *) roundmb(FFS_START);
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (uint32) truncmb((uint32)maxheap -
       			(uint32)minheap);

       return;
}
