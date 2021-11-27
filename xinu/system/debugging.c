#include<xinu.h>

uint32 free_ffs_pages(){
    struct memblk *curr;
    uint32 free_ffs = 0;
    write_cr3((unsigned long) PT_START);
    curr = ffslist.mnext;

    while(curr != NULL){
        free_ffs += curr->mlength;
        curr = curr->mnext;
    }
    write_cr3((unsigned long) proctab[currpid].pdbr);
    return (free_ffs/4096);
}

uint32 free_swap_pages(){
    struct memblk *curr;
    uint32 free_swp = 0;
    write_cr3((unsigned long) PT_START);
    curr = swplist.mnext;

    while(curr != NULL){
        //kprintf("free swp:%d, mlength:%d\n",free_swp,curr->mlength);
        free_swp += curr->mlength;
        curr = curr->mnext;
       // kprintf("*free swp:%d, mlength:%d\n",free_swp,curr->mlength);
    }
     write_cr3((unsigned long) proctab[currpid].pdbr);
     //kprintf("free swap = %d/4096 %d\n",free_swp, free_swp/4096);
    return (free_swp/4096);
}

    
uint32 allocated_virtual_pages(pid32 pid){
    return proctab[pid].pages_alloc;
}

uint32 used_ffs_frames(pid32 pid){
    return proctab[pid].pages_used;
}
