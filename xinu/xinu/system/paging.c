#include <xinu.h>


void initialize_page_tables()
{
	char *pd_start = (char *)(PT_START);
	int i, j;
	int npdes = TOTAL_PAGES/1024;
	pd_t *pd = (pd_t *)pd_start;
	
	for (i = 0; i < 1024; i++)
	{
		pd[i].pd_pres = 0;
		pd[i].pd_write = 1;
		//kprintf("PD[%d] = %x\n", i, &pd[i]);
	}
	
	pt_t *pt = (pt_t *)&pd[i];
	
	for (i = 0; i < npdes; i++)
	{
		pd[i].pd_pres = 1;
		pd[i].pd_base = (unsigned int)((int)&pt[i*1024] >> 12);
		//kprintf("pd %x, pt %x\n", pd[i].pd_base, &pt[i*1024]);
		for (j = i*1024; j < (i+1)*1024; j++)
		{
			pt[i].pt_pres = 1;
			pt[i].pt_write = 1;
			pt[i].pt_acc = 0;
			//pt[i].pt_valid = 1;
			pt[i].pt_base = ((j*4096) >> 12);
			if (i < 2){
			kprintf("pt_base %x\n", pt[i].pt_base);}
		}
	}
	write_cr3((unsigned long)PT_START);
	ptmeminit();
	kprintf("get %x -> %x\n", (uint32 *)getptmem(4096), (uint32 *)getptmem(4096));
	freeptmem((char *)(PT_START), 8192);
	kprintf("get %x -> %x\n", (uint32 *)getptmem(4096), (uint32 *)getptmem(4096));
}
			
	
	
			 
