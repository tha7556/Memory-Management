#include <stdlib.h>
#include "memory.h"
#include "queue.h"

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
    if (______trace_switch) printf("Reference parameters: %d\n\n",logic_addr);

    int pageOffset = log(PAGE_SIZE)/log(2);
    int frameNumber = log(logic_addr)/log(2);
    frameNumber = frameNumber - pageOffset;
    if (______trace_switch) printf("pageNumber: %d\npageOffset %d\n",frameNumber,pageOffset);
    if(frameNumber >= MAX_FRAME || pageOffset >= MAX_PAGE || frameNumber < 0 || pageOffset < 0) { //Not valid pointers
        Int_Vector.cause = pagefault;
        Int_Vector.pcb = pcb;
        Int_Vector.page_id = frameNumber;
        gen_int_handler();
    }
    if(action == store) {
        if (______trace_switch) printf("Page is loaded, beginning storing\n");
        Frame_Tbl[frameNumber].dirty = true;
    }
    enqueue(queue,&Frame_Tbl[frameNumber]);
    int physicalAddress = (frameNumber * MAX_FRAME) + pageOffset;
    if (______trace_switch) printf("Physical Address: %d\n",physicalAddress);
    memoryAccess(action,frameNumber,pageOffset);
}

void prepage(PCB *pcb) {
    return;
}

void get_page(PCB *pcb, int page_id) {
    //Part 1
    FRAME *frame = NULL;
    int i = 0;
    while(i < MAX_FRAME) {
        if(Frame_Tbl[i].free && Frame_Tbl[i].lock_count == 0) {
            frame = &Frame_Tbl[i];
            break;
        }
        i++;
    }
    //Part 2
    if(frame == NULL) {
        QueueNode *node = frontNode(&queue);
        while(node != NULL) {
            frame = node->data;
            if(frame->lock_count == 0) {
                break;
            }
            node = node->next;
        }
        //victim is frame hopefully
        if(frame->dirty == true) {
            frame->lock_count = 1;
            siodrum(write,frame->pcb,frame->page_id,frame->frame_id);
        }
        PAGE_ENTRY *entry = &PTBR[frame->page_id];
        entry->valid = false;
    }
    //Part 3:
    Frame_Tbl[frame->frame_id] = *frame; // a
    frame->lock_count = 1; //b
    siodrum(read,frame->pcb,frame->page_id,frame->frame_id); //c
    frame->lock_count = 0; //d
    //update page table entries for process e
    frame->dirty = false; //f
    enQueue(&queue,frame); //g
}
int start_cost(PCB *pcb) {
    return 0;
}

void deallocate(PCB *pcb) {

}

void lock_page(IORB *iorb) {

}

void unlock_page(IORB  *iorb) {

}
/* end of module */
