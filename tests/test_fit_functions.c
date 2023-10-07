#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(DEBUG)
#define debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug(...)
#endif

//--------------------------------------
// Structures utilisées
//--------------------------------------
// cellule presente au debut d'un bloc libre
typedef struct fb_ {
    size_t size;
    struct fb_ *next; // chainaige vers le prochain bloc libre
} fb;
typedef fb mem_free_block_t;

// liste chainee des zones libres
typedef struct { fb *first; } header;
typedef header mem_header_t;

// structure presente au debut de chaque bloc occupe
typedef struct { size_t size; } bb;
typedef bb mem_busy_block_t;

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    // schema de recherche classique
    mem_free_block_t *cell = first_free_block;
    while (cell && cell->size < wanted_size)
        cell = cell->next;

    return cell;
}

//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    mem_free_block_t *best = NULL;
    mem_free_block_t *cell = first_free_block;

    // parcours de toute les cellules de blocs libres
    while (cell) {
        // calcul du meilleur bloc
        if (best) { // si un meilleur bloc -> comparaison
            if (cell->size >= wanted_size && best->size > cell->size) // et si mieux ajuste
                best = cell; // alors c'est le meilleur bloc jusqu'a maintenant
        }
        else { // si pas de meilleur bloc
            if (cell->size >= wanted_size) // et taille suffisante
                best = cell; // alors c'est un meilleur bloc
        }

        cell = cell->next; // au suivant !
    }
    
    return best;
}

//-------------------------------------------------------------
mem_free_block_t *mem_worst_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    mem_free_block_t *block = first_free_block;
    mem_free_block_t *worst = NULL;
    size_t size_max = 0; // taille max initiale = 0

    while (block) {
        // si c'est mieux qu'auparavant
        if (block->size > size_max && block->size >= wanted_size) {
            worst = block;
            size_max = block->size;
        }

        block = block->next; // au suivant
    }

    return worst;
}

int main(int argc, char const *argv[])
{
    mem_header_t h;
    mem_free_block_t *result;
    h.first = NULL;

    //----------------------------------------------------------------
    debug("Test de mem_first_fit pour une taille de 100 octests\n");
    result = mem_first_fit(h.first, 100);
    debug("plus de place dans la memoire : @=%p\n", result);
    assert(result == NULL);

    mem_free_block_t fb1_1 = {10, NULL};
    h.first = &fb1_1;
    debug("Ajout d'un bloc de taille 10 à l'adresse %p\n", &fb1_1);
    result = mem_first_fit(h.first, 100);
    debug("pas de bloc assez grand : @=%p\n", result);
    assert(result == NULL);

    mem_free_block_t fb1_2 = {100, NULL};
    fb1_1.next = &fb1_2;
    debug("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb1_2);
    result = mem_first_fit(h.first, 100);
    debug("2e bloc assez grand : @=%p\n", result);
    assert(result == &fb1_2);

    mem_free_block_t fb1_3 = {100, NULL};
    fb1_2.next = &fb1_3;
    debug("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb1_3);
    result = mem_first_fit(h.first, 100);
    debug("2e bloc assez grand : @=%p\n", result);
    assert(result == &fb1_2);

    printf("TEST FIRST FIT : OK !\n");


    //----------------------------------------------------------------
    debug("--------------------------------------------------------\n");
    debug("Test de mem_best_fit pour une taille de 100 octests\n");
    h.first = NULL;

    result = mem_best_fit(h.first, 100);
    debug("plus de place dans la memoire : @=%p\n", result);
    assert(result == NULL);

    mem_free_block_t fb2_1 = {10, NULL};
    h.first = &fb2_1;
    debug("Ajout d'un bloc de taille 10 à l'adresse %p\n", &fb2_1);
    result = mem_best_fit(h.first, 100);
    debug("pas de bloc assez grand : @=%p\n", result);
    assert(result == NULL);

    mem_free_block_t fb2_2 = {200, NULL};
    fb2_1.next = &fb2_2;
    debug("Ajout d'un bloc de taille 200 à l'adresse %p\n", &fb2_2);
    result = mem_best_fit(h.first, 100);
    debug("2e bloc assez grand : @=%p\n", result);
    assert(result == &fb2_2);

    mem_free_block_t fb2_3 = {150, NULL};
    fb2_2.next = &fb2_3;
    debug("Ajout d'un bloc de taille 150 à l'adresse %p\n", &fb2_3);
    result = mem_best_fit(h.first, 100);
    debug("3e bloc meilleure taille : @=%p\n", result);
    assert(result == &fb2_3);

    mem_free_block_t fb2_4 = {100, NULL};
    fb2_3.next = &fb2_4;
    debug("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb2_4);
    result = mem_best_fit(h.first, 100);
    debug("4e bloc meilleure taille : @=%p\n", result);
    assert(result == &fb2_4);

    mem_free_block_t fb2_5 = {150, NULL};
    fb2_4.next = &fb2_5;
    debug("Ajout d'un bloc de taille 150 à l'adresse %p\n", &fb2_5);
    result = mem_best_fit(h.first, 100);
    debug("toujours 4e bloc meilleure taille : @=%p\n", result);
    assert(result == &fb2_4);

    printf("TEST BEST FIT : OK !\n");


    //----------------------------------------------------------------
    debug("--------------------------------------------------------\n");
    debug("Test de mem_worst_fit pour une taille de 100 octests\n");
    h.first = NULL;
    result = mem_worst_fit(h.first, 100);
    debug("plus de place dans la memoire header.first = NULL : @=%p\n", result);
    assert(result == NULL);

    mem_free_block_t fb3_1 = {10, NULL};
    h.first = &fb3_1;
    debug("Ajout d'un bloc de taille 10 à l'adresse %p\n", &fb3_1);
    result = mem_worst_fit(h.first, 100);
    debug("pas de bloc assez grand : @=%p\n", result);
    assert(result == NULL);

    mem_free_block_t fb3_2 = {200, NULL};
    fb3_1.next = &fb3_2;
    debug("Ajout d'un bloc de taille 200 à l'adresse %p\n", &fb3_2);
    result = mem_worst_fit(h.first, 100);
    debug("2e bloc le plus grand : @=%p\n", result);
    assert(result == &fb3_2);

    mem_free_block_t fb3_3 = {150, NULL};
    fb3_2.next = &fb3_3;
    debug("Ajout d'un bloc de taille 150 à l'adresse %p\n", &fb3_3);
    result = mem_worst_fit(h.first, 100);
    debug("2e bloc le plus grand : @=%p\n", result);
    assert(result == &fb3_2);

    mem_free_block_t fb3_4 = {300, NULL};
    fb3_3.next = &fb3_4;
    debug("Ajout d'un bloc de taille 300 à l'adresse %p\n", &fb3_4);
    result = mem_worst_fit(h.first, 100);
    debug("4e bloc le plus grand : @=%p\n", result);
    assert(result == &fb3_4);

    mem_free_block_t fb3_5 = {200, NULL};
    fb3_4.next = &fb3_5;
    debug("Ajout d'un bloc de taille 200 à l'adresse %p\n", &fb3_5);
    result = mem_worst_fit(h.first, 100);
    debug("toujours 4e bloc le plus grand : @=%p\n", result);
    assert(result == &fb3_4);

    mem_free_block_t fb3_6 = {100, NULL};
    fb3_5.next = &fb3_6;
    debug("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb3_6);
    result = mem_worst_fit(h.first, 100);
    debug("toujours 4e bloc le plus grand : @=%p\n", result);
    assert(result == &fb3_4);

    printf("TEST WORST FIT : OK !\n");

    return 0;
}