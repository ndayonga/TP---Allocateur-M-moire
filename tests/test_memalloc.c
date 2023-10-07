#include "mem.h"
#include "mem_os.h"
#include "mem_space.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ALIGNMENT 16
#define aligned_size(s) (\
    ((s) % ALIGNMENT) == 0 ? (s) : \
    ((s) + ALIGNMENT - (((size_t)(s)) % ALIGNMENT)) \
)


void afficher_zone(void *adresse, size_t taille, int free) {
    printf("Zone %s, Adresse : %lu, Taille : %lu\n", free ? "libre" : "occupee",
           (unsigned long)adresse, (unsigned long)taille);
}

void *adrfb1, *adrfb2, *adrfb3;

// Memoire apres init
// _____________________________________________________________________________
// | 8 |  72  |   280   |  40  |  48  |  48  | 16 |   116   |   116   | 127256
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
//  occ   occ    occ      occ   libre    occ  libr   occ        libre   occupe
void init(void) {
    mem_init();
    size_t adr_base = (size_t)mem_space_get_addr();
    
    debug("Quelques mem_alloc\n");
    void *adr1 = mem_alloc(64);
    void *adr2 = mem_alloc(272);
    adrfb1 = mem_alloc(80);
    assert(mem_get_size(adrfb1) == 80);
    mem_alloc(40);
    adrfb2 = mem_alloc(-1);
    mem_alloc(108);
    adrfb3 = mem_alloc(108);

    // on rend le dernier bloc occupee
    header *h = mem_space_get_addr();
    h->first = NULL;

    assert((size_t)adr1 == adr_base + aligned_size(sizeof(header)) + aligned_size(sizeof(bb)));
    assert(adr2 == adr1 + 64 + aligned_size(sizeof(bb)));
    assert(adrfb1 == adr2 + 272 + aligned_size(sizeof(bb)));

    // on libere adrfb1 adrfb2 et adrfb3 à la main
    fb *libre;
    
    libre = adrfb1 - aligned_size(sizeof(bb));
    libre->size = aligned_size(80) + aligned_size(sizeof(bb));
    libre->next = adrfb2 - aligned_size(sizeof(bb));

    libre = adrfb2 - aligned_size(sizeof(bb));
    libre->size = aligned_size(sizeof(fb));
    libre->next = adrfb3 - aligned_size(sizeof(bb));

    libre = adrfb3 - aligned_size(sizeof(bb));
    libre->size = 108 + aligned_size(sizeof(bb));
    libre->next = NULL;

    h->first = adrfb1 - aligned_size(sizeof(bb));
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

    mem_alloc(32);
    mem_alloc(4);
    mem_alloc(50);
    mem_alloc(50);
    mem_show(afficher_zone);
}

void test_worst_fit(void) {
    printf("-----------------------------------\n");
    printf("Test mem_alloc en stratégie worst fit\n");
    init();
    mem_set_fit_handler(mem_worst_fit);

    mem_alloc(50);
    mem_alloc(50);
    mem_alloc(4);
    mem_show(afficher_zone);
}

int main(void) {
    test_first_fit();
    test_best_fit();
    test_worst_fit();
    
    // TEST OK
    return 0;
}