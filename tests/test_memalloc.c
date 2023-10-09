#include "mem.h"
#include "mem_os.h"
#include "mem_space.h"

#include <assert.h>
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

void *adrfb1, *adrfb2, *adrfb3;

// Memoire apres init on inclut la taille des busy-block dans les zones occupees
// _____________________________________________________________________________
// | 16 | 80 |  288  |   96   |  64  |  16  |  48  |  128   |   128   | 127136
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//header occ    occ     libre    occ   libre    occ   occ      libre   occupe
void init(void) {
    mem_init();
    size_t adr_base = (size_t)mem_space_get_addr();
    
    debug("Quelques mem_alloc\n");
    void *adr1 = mem_alloc(64);
    void *adr2 = mem_alloc(272);
    adrfb1 = mem_alloc(80);
    mem_alloc(40);
    adrfb2 = mem_alloc(-1);
    mem_alloc(32);
    mem_alloc(112);
    adrfb3 = mem_alloc(112);

    // on rend le dernier bloc occupee
    header *h = mem_space_get_addr();
    h->first = NULL;

    assert((size_t)adr1 == adr_base + aligned_sizeof(header) + aligned_sizeof(bb));
    assert(adr2 == adr1 + 64 + aligned_sizeof(bb));
    assert(adrfb1 == adr2 + 272 + aligned_sizeof(bb));
    assert(mem_get_size(adrfb1) == 80);

    // on libere adrfb1 adrfb2 et adrfb3 à la main
    fb *libre;
    
    libre = adrfb1 - aligned_sizeof(bb);
    libre->size = aligned_size(80) + aligned_sizeof(bb);
    libre->next = adrfb2 - aligned_sizeof(bb);

    libre = adrfb2 - aligned_sizeof(bb);
    libre->size = MIN_SIZE_BLOCK;
    libre->next = adrfb3 - aligned_sizeof(bb);

    libre = adrfb3 - aligned_sizeof(bb);
    libre->size = aligned_size(112) + aligned_sizeof(bb);
    libre->next = NULL;

    h->first = adrfb1 - aligned_sizeof(bb);
}

void test_first_fit(void) {
    printf("-----------------------------------\n");
    printf("Test mem_alloc en stratégie first fit\n");
    init();

    #ifdef DEBUG
    mem_show(afficher_zone);    
    printf("=======================\n");
    #endif

    mem_alloc(80);
    mem_alloc(40);
    mem_show(afficher_zone);
}

void test_best_fit(void) {
    printf("-----------------------------------\n");
    printf("Test mem_alloc en stratégie best fit\n");
    init();
    mem_set_fit_handler(mem_best_fit);

    mem_alloc(80);
    mem_alloc(40);
    mem_show(afficher_zone);
}

void test_worst_fit(void) {
    printf("-----------------------------------\n");
    printf("Test mem_alloc en stratégie worst fit\n");
    init();
    mem_set_fit_handler(mem_worst_fit);

    mem_alloc(80);
    mem_alloc(40);
    mem_show(afficher_zone);
}

int main(void) {
    test_first_fit();
    test_best_fit();
    test_worst_fit();
    
    // TEST OK
    return 0;
}