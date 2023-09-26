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

// cellule presente au debut d'un bloc libre
typedef struct fb_ {
	size_t size;
	struct fb_ *next; // chainaige vers le prochain bloc libre
} fb;

// liste chainee des zones libres
typedef struct { fb *first; } header;

// structure presente au debut de chaque bloc occupe
typedef struct { size_t size; } bb;

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
    bb* occ = adr + max_size - 32;
    occ->size = 32;

    occ = ((void*)occ) - 64;
    occ->size = 64;

    fb* second = ((void*)occ) - 128;
    second->size = 128;
    second->next = NULL;

    occ = ((void*)second) - 24;
    occ->size = 24;

    occ = ((void*)occ) - 512;
    occ->size = 512;

    fb* first = adr + sizeof(header) + 128 + 64;
    first->size = max_size - sizeof(header) - (128+64+512+24+128+64+32);
    first->next = second;

    occ = ((void*)first) - 64;
    occ->size = 64;

    occ = ((void*)occ) - 128;
    occ->size = 128;

    header* h = adr;
    h->first = first;

    mem_show(&pretty);

    return 0;
}
