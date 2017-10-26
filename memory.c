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


Queue queue;
char *toString(FRAME *f)
{
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


void memory_init()
{
	initQueue(&queue);
}



void reference(int logic_addr, REFER_ACTION action)
{
    PCB *pcb = PTBR->pcb;
    if (______trace_switch) printf("Reference parameters: %d\n\n",logic_addr);

    int pageOffset = log(PAGE_SIZE)/log(2);
    int pageNumber = log(logic_addr)/log(2);
    pageNumber = pageNumber - pageOffset;
    if (______trace_switch) printf("pageNumber: %d\npageOffset %d\n",pageNumber,pageOffset);
    if(pageNumber >= MAX_PAGE || pageOffset >= MAX_FRAME) { //Not valid pointers
        Int_Vector.cause = pagefault;
        Int_Vector.pcb = pcb;
        Int_Vector.page_id = pageNumber;
        gen_int_handler();

    }


}



void prepage(PCB *pcb)
{

}


int start_cost(PCB *pcb)
{

}



void deallocate(PCB *pcb)
{

}



void get_page(PCB *pcb, int page_id)
{

}



void lock_page(IORB *iorb)
{

}



void unlock_page(IORB  *iorb)
{

}



/* end of module */
