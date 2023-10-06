#include "mem_space.h"
#include "mem.h"
#include "mem_os.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_SIZE 1000

/**
 * Compte le nombre de zone libre de taille inférieure
 * à MIN_SIZE
 */
size_t estimation_fragmentation() {
    mem_header_t *h = mem_space_get_addr();
    mem_free_block_t *fb = h->first;

    size_t compteur = 0;
    while (fb) {
        if (fb->size < MIN_SIZE) compteur++;
        fb = fb->next;
    }
    
    return compteur;
}

void frag_first_fit() {
    mem_init();
    void *adr1 = mem_alloc(3000);
    mem_alloc(3000);
    void *adr2 = mem_alloc(3000);
    mem_alloc(3000);
    void *adr3 = mem_alloc(3000);
    mem_alloc(3000);

    mem_free(adr1);
    mem_free(adr2);
    mem_free(adr3);

    mem_alloc(2000);
    mem_alloc(2000);
    mem_alloc(2000);
    mem_alloc(100000);

    printf("Estimation frag (worst) : %ld\n", estimation_fragmentation());
}

void frag_worst_fit() {
    mem_init();
    mem_set_fit_handler(mem_worst_fit);
    void *adr1 = mem_alloc(5000);
    mem_alloc(3000);
    void *adr2 = mem_alloc(5000);
    mem_alloc(3000);
    void *adr3 = mem_alloc(5000);
    mem_alloc(3000);

    mem_free(adr1);
    mem_free(adr2);
    mem_free(adr3);

    mem_alloc(2000);
    mem_alloc(2000);
    mem_alloc(2000);
    mem_alloc(100000);

    printf("Estimation frag (worst) : %ld\n", estimation_fragmentation());
}

void frag_best_fit() {
    mem_init();
    void *adr1 = mem_alloc(3000);
    mem_alloc(3000);
    void *adr2 = mem_alloc(3000);
    mem_alloc(3000);
    void *adr3 = mem_alloc(3000);
    mem_alloc(3000);

    mem_free(adr1);
    mem_free(adr2);
    mem_free(adr3);

    mem_alloc(2000);
    mem_alloc(2000);
    mem_alloc(2000);
    mem_alloc(100000);

    printf("Estimation frag (worst) : %ld\n", estimation_fragmentation());
}

int main(void)
{
    frag_first_fit();
    /* code */
    return 0;
}
