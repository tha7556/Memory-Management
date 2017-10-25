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

}



void reference(int logic_addr, REFER_ACTION action)
{

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
