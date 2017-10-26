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
        //set dirty bit of frame to true TODO: how to get the frame?
    }
    //update the queue
    //turn page/offset into physical address
    memoryAccess(action,frameNumber,pageOffset);
}

void prepage(PCB *pcb) {
    return;
}

void get_page(PCB *pcb, int page_id) {

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
