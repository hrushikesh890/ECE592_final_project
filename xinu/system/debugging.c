#include<xinu.h>

uint32 free_ffs_pages(){
    struct memblk *curr;
    uint32 free_ffs = 0;
    curr = &ffslist;

    while(curr != NULL){
        free_ffs += curr->mlength;
    }

    return free_ffs;
}

uint32 free_swap_pages(){
    struct memblk *curr;
    uint32 free_swp = 0;
    curr = &swplist;

    while(curr != NULL){
        free_swp += curr->mlength;
    }

    return free_swp;
}

uint32 allocated_virtual_pages(pid32 pid){
    return proctab[pid].pages_alloc;
}

uint32 used_ffs_frames(pid32 pid){
    return proctab[pid].pages_used;
}