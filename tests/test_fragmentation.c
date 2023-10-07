#include "mem_space.h"
#include "mem.h"
#include "mem_os.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * On se donne une métrique (pas térrible...)
 * Compte le pourcentage de zones libres de taille inférieure
 * à min_size sur le nombre de zones (libres et alloués)
 */
double estimation_fragmentation(size_t min_size) {
    void *zone_adr = mem_space_get_addr();
    size_t nb_petits_blocs = 0;
    size_t nb_blocs = 0;

    // 1ere cellule libre
    fb* cell_adr = ((header*)zone_adr)->first;
    zone_adr += sizeof(header);
    size_t zone_size;

    // on va parcourir jusqu'a l'@ de fin
    void *adr_fin = mem_space_get_addr()+mem_space_get_size()-1;
    while (zone_adr < adr_fin) {
        if (zone_adr == (void*)cell_adr) { // on a une zone libre
            zone_size = cell_adr->size;
            if (zone_size < min_size) nb_petits_blocs++;
            cell_adr = cell_adr->next;
        }
        else zone_size = ((bb*)zone_adr)->size;
        nb_blocs++;
        zone_adr += zone_size;
    }
    
    return (double)nb_petits_blocs / nb_blocs * 100.;
}


void frag_worst() {
    // first fit
    mem_init();
    mem_set_fit_handler(mem_first_fit);
    void *adr = mem_alloc(4000);
    mem_alloc(3000);
    while (adr != NULL) {
        void *adr2 = mem_alloc(4000);
        mem_free(adr);
        mem_alloc(3000);
        mem_alloc(3000);
        adr = adr2;
    }
    printf("Estimation frag (first) : %f %%\n", estimation_fragmentation(2500));

    // worst fit
    mem_init();
    mem_set_fit_handler(mem_worst_fit);
    adr = mem_alloc(4000);
    mem_alloc(3000);
    while (adr != NULL) {
        void *adr2 = mem_alloc(4000);
        mem_free(adr);
        mem_alloc(3000);
        mem_alloc(3000);
        adr = adr2;
    }
    printf("Estimation frag (worst) : %f %%\n", estimation_fragmentation(2500));

    // best fit
    mem_init();
    mem_set_fit_handler(mem_best_fit);
    adr = mem_alloc(4000);
    mem_alloc(3000);
    while (adr != NULL) {
        void *adr2 = mem_alloc(4000);
        mem_free(adr);
        mem_alloc(3000);
        mem_alloc(3000);
        adr = adr2;
    }
    printf("Estimation frag (best) : %f %%\n", estimation_fragmentation(2500));
    printf("---------------------------------\n");
}


void frag_best() {
    void *tab_alloc[7];

    // first_fit
    mem_init();
    mem_set_fit_handler(mem_first_fit);
    tab_alloc[0] = mem_alloc(10000); mem_alloc(3000);
    tab_alloc[1] = mem_alloc(8000); mem_alloc(3000);
    tab_alloc[2] = mem_alloc(6000); mem_alloc(3000);
    tab_alloc[3] = mem_alloc(4000); mem_alloc(3000);
    tab_alloc[4] = mem_alloc(6000); mem_alloc(3000);
    tab_alloc[5] = mem_alloc(8000); mem_alloc(3000);
    tab_alloc[6] = mem_alloc(10000); mem_alloc(3000);

    for (unsigned int i = 0; i < 7; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(8000);
    mem_alloc(6000);
    mem_alloc(6000);
    mem_alloc(8000);
    mem_alloc(4000);
    mem_alloc(10000);
    mem_alloc(10000);

    printf("Estimation frag (first) : %f %%\n", estimation_fragmentation(11000));


    // worst_fit
    mem_init();
    mem_set_fit_handler(mem_worst_fit);
    tab_alloc[0] = mem_alloc(10000); mem_alloc(3000);
    tab_alloc[1] = mem_alloc(8000); mem_alloc(3000);
    tab_alloc[2] = mem_alloc(6000); mem_alloc(3000);
    tab_alloc[3] = mem_alloc(4000); mem_alloc(3000);
    tab_alloc[4] = mem_alloc(6000); mem_alloc(3000);
    tab_alloc[5] = mem_alloc(8000); mem_alloc(3000);
    tab_alloc[6] = mem_alloc(10000); mem_alloc(3000);

    for (unsigned int i = 0; i < 7; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(8000);
    mem_alloc(6000);
    mem_alloc(6000);
    mem_alloc(8000);
    mem_alloc(4000);
    mem_alloc(10000);
    mem_alloc(10000);

    printf("Estimation frag (worst) : %f %%\n", estimation_fragmentation(11000));


    // best fit
    mem_init();
    mem_set_fit_handler(mem_best_fit);
    tab_alloc[0] = mem_alloc(10000); mem_alloc(3000);
    tab_alloc[1] = mem_alloc(8000); mem_alloc(3000);
    tab_alloc[2] = mem_alloc(6000); mem_alloc(3000);
    tab_alloc[3] = mem_alloc(4000); mem_alloc(3000);
    tab_alloc[4] = mem_alloc(6000); mem_alloc(3000);
    tab_alloc[5] = mem_alloc(8000); mem_alloc(3000);
    tab_alloc[6] = mem_alloc(10000); mem_alloc(3000);

    for (unsigned int i = 0; i < 7; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(8000);
    mem_alloc(6000);
    mem_alloc(6000);
    mem_alloc(8000);
    mem_alloc(4000);
    mem_alloc(10000);
    mem_alloc(10000);

    printf("Estimation frag (best) : %f %%\n", estimation_fragmentation(11000));
    printf("---------------------------------\n");
}


void frag_first() {
    void *tab_alloc[4];

    // first fit
    mem_init();
    mem_set_fit_handler(mem_first_fit);
    tab_alloc[0] = mem_alloc(5000); mem_alloc(3000);
    tab_alloc[1] = mem_alloc(3000); mem_alloc(3000);
    tab_alloc[2] = mem_alloc(2000); mem_alloc(3000);
    tab_alloc[3] = mem_alloc(4500); mem_alloc(3000);

    for (unsigned int i = 0; i < 4; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(4000);
    mem_alloc(1000-sizeof(bb));

    printf("Estimation frag (first) : %f %%\n", estimation_fragmentation(6000));

    // worst fit
    mem_init();
    mem_set_fit_handler(mem_worst_fit);
    tab_alloc[0] = mem_alloc(5000); mem_alloc(3000);
    tab_alloc[1] = mem_alloc(3000); mem_alloc(3000);
    tab_alloc[2] = mem_alloc(2000); mem_alloc(3000);
    tab_alloc[3] = mem_alloc(4500); mem_alloc(3000);

    for (unsigned int i = 0; i < 4; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(4000);
    mem_alloc(1000-sizeof(bb));

    printf("Estimation frag (worst) : %f %%\n", estimation_fragmentation(6000));


    // best fit
    mem_init();
    mem_set_fit_handler(mem_best_fit);
    tab_alloc[0] = mem_alloc(5000); mem_alloc(3000);
    tab_alloc[1] = mem_alloc(3000); mem_alloc(3000);
    tab_alloc[2] = mem_alloc(2000); mem_alloc(3000);
    tab_alloc[3] = mem_alloc(4500); mem_alloc(3000);

    for (unsigned int i = 0; i < 4; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(4000);
    mem_alloc(1000-sizeof(bb));

    printf("Estimation frag (best) : %f %%\n", estimation_fragmentation(6000));
}


int main(void) {
    frag_worst();
    frag_best();
    frag_first();
    return 0;
}