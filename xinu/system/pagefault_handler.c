#include <xinu.h>
void pagefault_handler()
{
	uint32 cr2_val = read_cr2();
	
	kprintf("FAULLLLLT");
}

