#include <xinu.h>


void initialize_page_tables()
{
	char *pd_start = (char *)(getptmem(4096));
	int i, j;
	int npdes = TOTAL_PAGES/1024;
	pd_t *pd = (pd_t *)pd_start;
	
	for (i = 0; i < 1024; i++)
	{
		pd[i].pd_pres = 0;
		pd[i].pd_write = 1;
		//kprintf("PD[%d] = %x\n", i, &pd[i]);
	}
	
	
	
	for (i = 0; i < npdes; i++)
	{
		pd[i].pd_pres = 1;
		pt_t *pt = (pt_t *)(getptmem(4096));
		pd[i].pd_base = (unsigned int)((unsigned int)&pt[0] >> 12);
		//kprintf("pd %x, pt %x %x\n", pd[i].pd_base, &pt[0], &pd[0]);
		for (j = 0; j < 1024; j++)
		{
			pt[j].pt_pres = 1;
			pt[j].pt_write = 1;
			pt[j].pt_acc = 0;
			//pt[i].pt_valid = 1;
			pt[j].pt_base = (((i*1024 + j)*4096) >> 12);
			if (i < 20){
			//kprintf("pd addr %x pd_base = %x pt_addr %x pt_base %x\n", pt[j].pt_base);
			}
		}
	}
	write_cr3((unsigned long)&pd[0]);
	
	/*kprintf("get %x -> %x\n", (uint32 *)getptmem(4096), (uint32 *)getptmem(4096));
	freeptmem((char *)(PT_START), 8192);
	kprintf("get %x -> %x\n", (uint32 *)getptmem(4096), (uint32 *)getptmem(4096));*/
	set_evec(14, (uint32)pagefault_handler_disp);
}
			
uint32 allocate_pdpt()
{
	char *pd_start = (char *)(getptmem(4096));
	int i, j;
	int npdes = (XINU_PAGES + 1024)/1024;
	pd_t *pd = (pd_t *)pd_start;
	
	for (i = 0; i < 1024; i++)
	{
		pd[i].pd_pres = 0;
		pd[i].pd_write = 1;
		//kprintf("PD[%d] = %x\n", i, &pd[i]);
	}
	
	for (i = 0; i < npdes; i++)
	{
		pt_t *pt = (pt_t *)getptmem(4096);
		pd[i].pd_pres = 1;
		pd[i].pd_base = (unsigned int)((int)&pt[0] >> 12);
		kprintf("pd %x, pt %x\n", pd[i].pd_base, &pt[i*1024]);
		for (j = 0; j < 1024; j++)
		{
			pt[j].pt_pres = 1;
			pt[j].pt_write = 1;
			pt[j].pt_acc = 0;
			//pt[i].pt_valid = 1;
			pt[j].pt_base = (((i*1024 + j)*4096) >> 12);
			if (i < 2){
			kprintf("pt_base %x\n", pt[j].pt_base);}
		}
	}
	return &pd[0];
}
	
			 
