//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#include "mem_space.h"
#include "mem.h"
#include "mem_os.h"
#include <stdio.h>
#include <stdlib.h>

void pretty(void * zone, size_t s, int free) {
    printf("Zone %p Taille %ld - libre=%d\n", zone, s, free);
}


int main(int argc, char *argv[]) {
    void *adr = mem_space_get_addr();
    size_t max_size = mem_space_get_size();

    mem_init();

    // profile de la memoire fait manuellement
    // header occupee occupee libre occupee occupee libre occupee occupee
    // sizeof  128      64     <->    512     24     128    64      32     
    
    // on commence par la fin et on va par @ decroissante
    mem_busy_block_t* occ = adr + max_size - 32;
    occ->size = 32;

    occ = ((void*)occ) - 64;
    occ->size = 64;

    mem_free_block_t* second = ((void*)occ) - 128;
    second->size = 128;
    second->next = NULL;

    occ = ((void*)second) - 24;
    occ->size = 24;

    occ = ((void*)occ) - 512;
    occ->size = 512;

    mem_free_block_t* first = adr + sizeof(mem_header_t) + 128 + 64;
    first->size = max_size - sizeof(mem_header_t) - (128+64+512+24+128+64+32);
    first->next = second;

    occ = ((void*)first) - 64;
    occ->size = 64;

    occ = ((void*)occ) - 128;
    occ->size = 128;

    mem_header_t* h = adr;
    h->first = first;

    mem_show(&pretty);

    return 0;
}
