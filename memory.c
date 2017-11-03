#include <stdlib.h>
#include <math.h>
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
    if (______trace_switch) printf("YOU ARR IN MEMORY_INIT\n");
	initQueue(&queue);
}

void reference(int logic_addr, REFER_ACTION action) {
    //Part 1
    PCB *pcb = PTBR->pcb;
    if (______trace_switch) printf("YOU ARR IN REFERENCE\n");
    if (______trace_switch) printf("\tReference parameters: %d\n\n",logic_addr);
    if(action == store)
        if (______trace_switch) printf("\tACTION IS STORE");
    if(action == load)
        if (______trace_switch) printf("\tACTION IS LOAD\n");
    //Part 2
    int pageNumber = logic_addr/PAGE_SIZE;
    int pageOffset = logic_addr % PAGE_SIZE;
    if (______trace_switch) printf("\tpageNumber: %d\n\tpageOffset %d\n",pageNumber,pageOffset);
    int booliar = 2;
    if(pcb->page_tbl->page_entry[pageNumber].valid == true)
        booliar = 1;
    else
        booliar = 0;
    if (______trace_switch) printf("\tBOOL: %d\n",booliar);
    //Part 3/4
    if(pcb->page_tbl->page_entry[pageNumber].valid == false || pageNumber >= MAX_PAGE || pageNumber < 0) { //Not valid pointers
        if (______trace_switch) printf("MARKED AS NOT VALID ARRRR\n");
        Int_Vector.cause = pagefault;
        Int_Vector.pcb = pcb;
        Int_Vector.page_id = pageNumber;
        gen_int_handler();
    }
    //Part 5
    if(action == store) { //a
        Frame_Tbl[pcb->page_tbl->page_entry[pageNumber].frame_id].dirty = true;
    }
    enQueue(&queue,&Frame_Tbl[pageNumber]); //b
    int physicalAddress = (pageNumber * MAX_FRAME) + pageOffset; //c
    memoryAccess(action,pcb->page_tbl->page_entry[pageNumber].frame_id,pageOffset); // d

}

void prepage(PCB *pcb) {
    if (______trace_switch) printf("YOU ARR IN PREPAGE\n");
    return;
}

void get_page(PCB *pcb, int page_id) {
    if (______trace_switch) printf("YOU ARR IN GETPAGE\n");
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
            siodrum(write,pcb,page_id,frame->frame_id);
            frame->dirty = false;
        }
        PAGE_ENTRY *entry = &PTBR->page_entry[frame->page_id];
        entry->valid = false;
    }
    //Part 3:
    Frame_Tbl[frame->frame_id] = *frame; // a
    frame->lock_count = 1; //b
    frame->free = false;
    siodrum(read,pcb,page_id,frame->frame_id); //c
    frame->lock_count = 0; //d
    //update page table entries for process e ????????
    frame->pcb = pcb;
    frame->page_id = page_id;
    frame->pcb->page_tbl->page_entry[frame->page_id].frame_id = frame->frame_id;


    frame->dirty = false; //f
    frame->pcb->page_tbl->page_entry[page_id].valid = true;
    deQueue(&queue);
    enQueue(&queue,frame); //g

}
int start_cost(PCB *pcb) {
    if (______trace_switch) printf("YOU ARR IN START_COST\n");
    return 0;
}

void deallocate(PCB *pcb) {
    if (______trace_switch) printf("YOU ARR IN DEALLOCATE\n");
    int i = 0;
    for(i = 0; i < MAX_PAGE; i++) {
        PAGE_ENTRY *page = &(pcb->page_tbl->page_entry[i]);
        FRAME *frame = &Frame_Tbl[page->frame_id];
        if(page->valid == true){
            frame->pcb = NULL;
            frame->dirty = true;
            frame->page_id = 0;
            removeNode(&queue,findNode(&queue,frame,*compareTo));
            frame->free = true;
        }
    }
}

void lock_page(IORB *iorb) {
    if (______trace_switch) printf("YOU ARR IN LOCK_PAGE\n");
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
    if (______trace_switch) printf("YOU ARR IN UNLOCK_PAGE\n");
    PAGE_ENTRY *page = &(iorb->pcb->page_tbl->page_entry[iorb->page_id]);
    Frame_Tbl[page->frame_id].lock_count -= 1;
}
/* end of module */
