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

#define max(a, b)       ((a) < (b) ? (b) : (a))
#define MIN_SIZE_BLOCK  max(aligned_sizeof(bb), aligned_sizeof(fb))
#define ALIGNMENT 16
#define aligned_size(s) (\
    (((s) % ALIGNMENT) == 0) ? (s) : \
    ((s) + ALIGNMENT - ((s) % ALIGNMENT)) \
)
#define aligned_sizeof(s) aligned_size(sizeof(s))

void afficher_zone(void *adresse, size_t taille, int free) {
    printf("Zone %s, Adresse : %lu, Taille : %lu\n", free ? "libre" : "occupee",
           (unsigned long)adresse, (unsigned long)taille);
}


int main(int argc, char *argv[]) {
    size_t s1, s2, s3, s4, s5;
    s1 = aligned_size(32);
    s2 = aligned_size(64);
    s3 = aligned_size(128);
    s4 = aligned_size(24);
    s5 = aligned_size(512);

    mem_init();

    void* adr = mem_space_get_addr();
    size_t max_size = (size_t)mem_space_get_size();
    mem_header_t* h = adr;
    h->first = NULL;

    // profile de la memoire fait manuellement
    //    header      occupee occupee occupe libre occupee      libre
    // sizeof(header)   32      64      128   <->   512     espace restant

    mem_busy_block_t* occ0 = adr + aligned_sizeof(mem_header_t);
    occ0->size = aligned_sizeof(mem_busy_block_t) + s1;

    mem_busy_block_t* occ1 = (void*)occ0 + occ0->size;
    occ1->size = aligned_sizeof(mem_busy_block_t) + s2;

    mem_busy_block_t* occ2 = (void*)occ1 + occ1->size;
    occ2->size = aligned_sizeof(mem_busy_block_t) + s3;

    mem_busy_block_t* occ3 = (void*)occ2 + occ2->size;
    occ3->size = aligned_sizeof(mem_busy_block_t) + s4;

    mem_busy_block_t* occ4 = (void*)occ3 + occ3->size;
    occ4->size = aligned_sizeof(mem_busy_block_t) + s5;

    mem_free_block_t* second = (void*)occ4 + occ4->size;
    second->size = max_size - sizeof(mem_header_t);
    second->size -= (aligned_sizeof(mem_busy_block_t) + s1);
    second->size -= (aligned_sizeof(mem_busy_block_t) + s2);
    second->size -= (aligned_sizeof(mem_busy_block_t) + s3);
    second->size -= (aligned_sizeof(mem_busy_block_t) + s4);
    second->size -= (aligned_sizeof(mem_busy_block_t) + s5);
    second->next = NULL;

    h->first = second;

    mem_free_block_t* first = (void*)occ3;
    first->size = occ3->size;
    first->next = second;

    h->first = first;

    mem_show(afficher_zone);

    return 0;
}
