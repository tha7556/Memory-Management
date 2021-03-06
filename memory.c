#include <stdlib.h>
#include "memory.h"
#include "queue.h"

//Tyler Atkinson & Dylan Menchetti

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                              Module MEMORY                               */
/*                            Internal Routines                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

int compareTo(FRAME *f1, FRAME *f2);
Queue queue;
char *toString(FRAME *f) {
    static char result[BUFSIZ];

    // For each from that is printed, print the frame number, pcb id of the
    // process that owns it, the page number of the page it contains,
    // D or C if it is dirty or clean, and the lock count.
    if (f == NULL) sprintf (result, "(null) ");
    else sprintf (result, "Frame %d(pcb-%d,page-%d,%c,lock-%d) ",
               f->frame_id,
               f->pcb->pcb_id,
               f->page_id,
               (f->dirty ? 'D' : 'C'),
               f->lock_count);

    return result;
}
int compareTo(FRAME *f1, FRAME *f2) {
    return f1->frame_id - f2->frame_id;
}

void memory_init() {
	initQueue(&queue);
}

void reference(int logic_addr, REFER_ACTION action) {
    PCB *pcb = PTBR->pcb;
    int pageNumber = logic_addr/PAGE_SIZE;
    int pageOffset = logic_addr % PAGE_SIZE;

    if(pcb->page_tbl->page_entry[pageNumber].valid == false) {
        Int_Vector.cause = pagefault;
        Int_Vector.pcb = pcb;
        Int_Vector.page_id = pageNumber;
        gen_int_handler();
    }
    if(action == store) {
        Frame_Tbl[pcb->page_tbl->page_entry[pageNumber].frame_id].dirty = true;
    }
    removeNode(&queue,findNode(&queue,&Frame_Tbl[pcb->page_tbl->page_entry[pageNumber].frame_id],compareTo));
    enQueue(&queue,&Frame_Tbl[pcb->page_tbl->page_entry[pageNumber].frame_id]);
    int physicalAddress = (pageNumber * PAGE_SIZE) + pageOffset;
    memoryAccess(action,pcb->page_tbl->page_entry[pageNumber].frame_id,pageOffset);

}

void prepage(PCB *pcb) {
    return;
}

void get_page(PCB *pcb, int page_id) {
    FRAME *frame = NULL;
    int i = 0;
    while(i < MAX_FRAME) {
        if(Frame_Tbl[i].free && Frame_Tbl[i].lock_count == 0) {
            frame = &Frame_Tbl[i];
            break;
        }
        i++;
    }
    if(frame == NULL) {
        QueueNode *node = frontNode(&queue);
        while(node != NULL) {
            frame = node->data;
            if(frame->lock_count == 0) {
                break;
            }
            node = node->next;
        }
        if(frame->dirty == true) {
            frame->lock_count = 1;
            siodrum(write,frame->pcb,frame->page_id,frame->frame_id);
            frame->dirty = false;
        }
        PAGE_ENTRY *entry = &frame->pcb->page_tbl->page_entry[frame->page_id];
        entry->valid = false;
    }
    frame->lock_count = 1;
    frame->free = false;
    siodrum(read,pcb,page_id,frame->frame_id);
    frame->lock_count = 0;
    frame->pcb = pcb;
    frame->page_id = page_id;
    pcb->page_tbl->page_entry[page_id].frame_id = frame->frame_id;


    frame->dirty = false;
    pcb->page_tbl->page_entry[page_id].valid = true;
    removeNode(&queue,findNode(&queue,&Frame_Tbl[frame->frame_id],compareTo));
    enQueueSorted(&queue,&Frame_Tbl[frame->frame_id],compareTo);

}
int start_cost(PCB *pcb) {
    return 0;
}

void deallocate(PCB *pcb) {
    int i = 0;
    for(i = 0; i < MAX_PAGE; i++) {
        PAGE_ENTRY *page = &(pcb->page_tbl->page_entry[i]);
        if(page->valid == true){
            FRAME *frame = &Frame_Tbl[page->frame_id];
            frame->pcb = NULL;
            frame->dirty = true;
            frame->page_id = 0;
            removeNode(&queue,findNode(&queue,frame,compareTo));
            frame->free = true;
        }
    }
}

void lock_page(IORB *iorb) {
    PAGE_ENTRY *page = &(iorb->pcb->page_tbl->page_entry[iorb->page_id]);
    if(!page->valid == true) {
        Int_Vector.cause = pagefault;
        Int_Vector.pcb = iorb->pcb;
        Int_Vector.page_id = iorb->page_id;
        gen_int_handler();
    }
    if(iorb->action == read) {
        Frame_Tbl[page->frame_id].dirty = true;
    }
    Frame_Tbl[page->frame_id].lock_count += 1;
}

void unlock_page(IORB  *iorb) {
    PAGE_ENTRY *page = &(iorb->pcb->page_tbl->page_entry[iorb->page_id]);
    Frame_Tbl[page->frame_id].lock_count -= 1;
}
/* end of module */
