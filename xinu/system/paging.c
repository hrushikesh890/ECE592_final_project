#include <xinu.h>


void initialize_page_tables()
{
	char *pd_start = (char *)(getptmem(4096));
	int i, j;
	int npdes = (TOTAL_PAGES)/1024;
	pd_t *pd = (pd_t *)pd_start;
	for (i = 0; i < 1024; i++)
	{
		pd[i].pd_pres = 0;
		pd[i].pd_write = 1;
	}
	
	for (i = 0; i < npdes; i++)
	{
		pd[i].pd_pres = 1;
		pt_t *pt = (pt_t *)(getptmem(4096));
		
		pd[i].pd_base = (unsigned int)((unsigned int)&pt[0] >> 12);
		for (j = 0; j < 1024; j++)
		{
			pt[j].pt_pres = 1;
			pt[j].pt_write = 1;
			pt[j].pt_acc = 0;
			pt[j].pt_valid = 1;
			pt[j].pt_base = (((i*1024 + j)*4096) >> 12);
			
		}
	}
	write_cr3((unsigned long)&pd[0]);	
	set_evec(14, (uint32)pagefault_handler_disp);
}
			
uint32 allocate_pdpt()
{
	char *pd_start = (char *)(getptmem(4096));
	int i, j;
	int npdes = (XINU_PAGES)/1024;
	pd_t *pd = (pd_t *)pd_start;
	
	for (i = 0; i < 1024; i++)
	{
		pd[i].pd_pres = 0;
		pd[i].pd_write = 1;
	}
	
	for (i = 0; i < npdes; i++)
	{
		pt_t *pt = (pt_t *)getptmem(4096);
		pd[i].pd_pres = 1;
		pd[i].pd_base = (unsigned int)((int)&pt[0] >> 12);

		for (j = 0; j < 1024; j++)
		{
			pt[j].pt_pres = 1;
			pt[j].pt_write = 1;
			pt[j].pt_acc = 0;
			pt[i].pt_valid = 1;
			pt[j].pt_base = (((i*1024 + j)*4096) >> 12);
		}
	}
	return &pd[0];
}
	
			 
