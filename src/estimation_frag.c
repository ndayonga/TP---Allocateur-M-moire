#include "mem_space.h"
#include "mem.h"
#include "mem_os.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ALIGNMENT 16
#define aligned_size(s) (\
    (((s) % ALIGNMENT) == 0) ? (s) : \
    ((s) + ALIGNMENT - ((s) % ALIGNMENT)) \
)
#define aligned_sizeof(s) aligned_size(sizeof(s))

/**
 * On se donne une métrique (pas térrible...)
 * Compte le nombre de zones libres 
 * sur le nombre de zones (libres et occupées)
 */
double estimation_fragmentation() {
    void *zone_adr = mem_space_get_addr();
    size_t nb_petits_blocs = 0;
    size_t nb_blocs = 0;

    // 1ere cellule libre
    fb* cell_adr = ((header*)zone_adr)->first;
    zone_adr += aligned_sizeof(header);
    size_t zone_size;

    // on va parcourir jusqu'a l'@ de fin
    void *adr_fin = mem_space_get_addr()+mem_space_get_size()-1;
    while (zone_adr < adr_fin) {
        if (zone_adr == (void*)cell_adr) { // on a une zone libre
            zone_size = cell_adr->size;
            nb_petits_blocs++;
            cell_adr = cell_adr->next;
        }
        else zone_size = ((bb*)zone_adr)->size;
        nb_blocs++;
        zone_adr += zone_size;
    }
    
    return (double)nb_petits_blocs / nb_blocs * 100.;
}


/**
 * On se donne une métrique (un peu mieux...)
 * Compte le pourcentage de la taille des zones libres
 * relativement à la mémoire nécessaire à l'allocateur
 */
double estimation_fragmentation2() {
    void *zone_adr = mem_space_get_addr();
    size_t free_memory_size = 0;
    size_t used_memory_size = 0;

    // 1ere cellule libre
    fb *cell_adr = ((header*)zone_adr)->first;
    fb *prec_adr = NULL;
    zone_adr += aligned_sizeof(header);
    size_t zone_size;

    // on va parcourir jusqu'a l'@ de fin
    void *adr_fin = mem_space_get_addr()+mem_space_get_size();
    while (zone_adr < adr_fin) {
        if (zone_adr == (void*)cell_adr) { // on a une zone libre
            zone_size = cell_adr->size;
            if (zone_adr + zone_size < adr_fin)
                free_memory_size += zone_size;
            prec_adr = cell_adr;
            cell_adr = cell_adr->next;
        }
        else {
            zone_size = ((bb*)zone_adr)->size;
        }
        zone_adr += zone_size;
    }

    // memoire utilisé sauf la fin si elle est libre
    if (prec_adr && (void*)prec_adr + prec_adr->size >= adr_fin)
        used_memory_size = mem_space_get_size() - prec_adr->size;
    else
        used_memory_size = mem_space_get_size();
    
    return (double)free_memory_size / used_memory_size * 100.;
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
    printf("Estimation frag en nombre (first) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (first) : %f %%\n", estimation_fragmentation2());

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
    printf("Estimation frag en nombre (worst) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (worst) : %f %%\n", estimation_fragmentation2());

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
    printf("Estimation frag en nombre (best) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (best) : %f %%\n", estimation_fragmentation2());
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

    printf("Estimation frag en nombre (first) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (first) : %f %%\n", estimation_fragmentation2());


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

    printf("Estimation frag en nombre (worst) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (worst) : %f %%\n", estimation_fragmentation2());


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

    printf("Estimation frag en nombre (best) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (best) : %f %%\n", estimation_fragmentation2());
    printf("---------------------------------\n");
}


void frag_first() {
    void *tab_alloc[4];

    // first fit
    mem_init();
    mem_set_fit_handler(mem_first_fit);
    tab_alloc[0] = mem_alloc(6400); mem_alloc(1280);
    tab_alloc[1] = mem_alloc(1280); mem_alloc(1280);
    tab_alloc[2] = mem_alloc(4096); mem_alloc(1280);
    tab_alloc[3] = mem_alloc(1536); mem_alloc(1280);

    for (unsigned int i = 0; i < 4; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(3968);
    mem_alloc(6400-3968-aligned_sizeof(bb));
    mem_alloc(1280);

    printf("Estimation frag en nombre (first) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (first) : %f %%\n", estimation_fragmentation2());


    // worst fit
    mem_init();
    mem_set_fit_handler(mem_worst_fit);
    tab_alloc[0] = mem_alloc(6400); mem_alloc(1280);
    tab_alloc[1] = mem_alloc(1280); mem_alloc(1280);
    tab_alloc[2] = mem_alloc(4096); mem_alloc(1280);
    tab_alloc[3] = mem_alloc(1536); mem_alloc(1280);

    for (unsigned int i = 0; i < 4; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(3968);
    mem_alloc(6400-3968-aligned_sizeof(bb));
    mem_alloc(1280);

    printf("Estimation frag en nombre (worst) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (worst) : %f %%\n", estimation_fragmentation2());


    // best fit
    mem_init();
    mem_set_fit_handler(mem_best_fit);
    tab_alloc[0] = mem_alloc(6400); mem_alloc(1280);
    tab_alloc[1] = mem_alloc(1280); mem_alloc(1280);
    tab_alloc[2] = mem_alloc(4096); mem_alloc(1280);
    tab_alloc[3] = mem_alloc(1536); mem_alloc(1280);

    for (unsigned int i = 0; i < 4; i++)
        mem_free(tab_alloc[i]);

    mem_alloc(3968);
    mem_alloc(6400-3968-aligned_sizeof(bb));
    mem_alloc(1280);

    printf("Estimation frag en nombre (best) : %f %%\n", estimation_fragmentation());
    printf("Estimation frag en taille (best) : %f %%\n", estimation_fragmentation2());
}


int main(void) {
    frag_worst();
    frag_best();
    frag_first();
    return 0;
}