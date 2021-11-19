#include<xinu.h>

syscall vfree(char*  ptr, uint32 nbytes){
    intmask mask;
    uint32 npages = (nbytes/4096);
    pd_t* pd_base = (pd_t)ptr;
	pt_t* pt_base;
    mask = disable();
    write_cr3((unsigned long)PT_START);

	if(nbytes == 0){
		restore(mask);
		return (char *)SYSERR;
	}

	//nbytes = (uint32) roundmb(nbytes);
	
	
	if (nbytes > (npages*4096))
	{
		npages += 1;
	}

	for ()   
}
