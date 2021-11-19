#include <xinu.h>


char* vmalloc(uint32 nbytes){
	intmask mask;
	pd_t* pd_base = proctab[currpid].pdbr;
	pt_t* pt_base;
	char* retVal;
	mask = disable();

	write_cr3((unsigned long)PT_START);

	if(nbytes == 0){
		restore(mask);
		return (char *)SYSERR;
	}

	//nbytes = (uint32) roundmb(nbytes);
	uint32 npages = (nbytes/4096);
	
	if (nbytes > (npages*4096))
	{
		npages += 1;
	}
	
	proctab[currpid].pages_alloc += npages;
	int i;
	for (i=9;i<1024;i++)
	{
		
		if (pd_base[i].pd_pres == 0)
		{
			pd_base[i].pd_pres = 1;
			pd_base[i].pd_write = 1;
			pt_base = (pt_t*)(getptmem(4096));
			pd_base[i].pd_base = (uint32) (((uint32) pt_base >> 12) & (0x0FFFFF));
		}
		else
		{
			pt_base = (pt_t*)pd_base[i].pd_base;
		}

		for(i=0; i<1024 ;i++)
		{
			if ((pt_base[i].pt_valid == 0) && (pt_base[i].pt_pres == 0) && (npages > 0))
			{
				pt_base[i].pt_pres = 0;
				pt_base[i].pt_write = 1;
				pt_base[i].pt_valid = 1;
				npages -= 1;
			}
		}
		if (npages == 0)
		{
			break;
		}
		
	}

	write_cr3((unsigned long) proctab[currpid].pdbr);
	return 
}
