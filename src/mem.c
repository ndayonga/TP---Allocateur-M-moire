//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#include "mem.h"
#include "mem_space.h"
#include "mem_os.h"
#include <assert.h>
#include <stdint.h>

mem_fit_function_t *get_free_block = mem_first_fit;

//-------------------------------------------------------------
// mem_init
//-------------------------------------------------------------
/**
 * Initialize the memory allocator.
 * If already init it will re-init.
**/
void mem_init() {
    // on place la tete de la liste des zones libres en memoire
    mem_header_t* l = mem_space_get_addr();
    l->first = mem_space_get_addr() + sizeof(mem_header_t);

    // on replit le bloc en debut de 
    fb* first = l->first;
    first->size = mem_space_get_size() - sizeof(mem_header_t);
    first->next = NULL;
}


//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/
void *mem_alloc(size_t size) {
    // recupere l'entete
    mem_header_t* tete = mem_space_get_addr();

    /* la taille voulue est :
     * - la taille demandée + la taille d'une structure busy block
     * ET
     * - au moins la taille d'une cellule libre (structure fb) 
     *    garantissant la possibilité de libération du bloc
     */
    size_t wanted = (size + sizeof(bb) > sizeof(fb)) ? size + sizeof(bb) : sizeof(fb);
    size_t real_size;

    // get_free_block = mem_best_fit;
    // get_free_block = mem_worst_fit;

    // on recupere le bloc libre qui nous interesse
    mem_free_block_t *free_block = (*get_free_block)(tete->first, wanted);

    // si y'en a pas on renvoie NULL
    if (!free_block) return NULL;

    // Cas ou on peut mettre une structure fb sur la fin du bloc
    if (free_block->size > wanted + sizeof(fb)) {
        // on cree la nouvelle fb en fin de zone
        mem_free_block_t *new_fb = ((void*)free_block + wanted);
        new_fb->size = free_block->size - wanted;
        new_fb->next = free_block->next;

        if (tete->first == free_block) { // on est la tete
            tete->first = new_fb;
        }
        else { // sinon on va chercher le bloc qui precede
            mem_free_block_t *prec = tete->first;
            while(prec->next != free_block)
                prec = prec->next;

            prec->next = new_fb;
        }

        // taille allouee dans ce cas
        real_size = wanted;
    }

    // cas ou on occupe tout le bloc
    else {
        if (tete->first == free_block) {
            // si on est la tete, on passe au suivant
            tete->first = free_block->next;
        }
        else{
            mem_free_block_t *prec = tete->first;
            while (prec->next != free_block)
                prec = prec->next;

            prec->next = free_block->next;
        }

        // taille allouee dans cet autre cas
        real_size = free_block->size;
    }

    // construction du busy bloc
    mem_busy_block_t *bb = (mem_busy_block_t*) free_block;
    bb->size = real_size;
    return ((void*)free_block) + sizeof(bb);
}

//-------------------------------------------------------------
// mem_get_size
//-------------------------------------------------------------
size_t mem_get_size(void * zone)
{
    mem_busy_block_t *bb = zone - sizeof(mem_busy_block_t);
    return bb->size - sizeof(mem_busy_block_t);
}

//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
/**
 * Free an allocaetd bloc.
**/
void mem_free(void *zone) {
    // zone memoire
    if(zone == NULL)
        return;
    mem_header_t* tete = mem_space_get_addr();
    if ((void*)tete+sizeof(mem_header_t) > zone || zone > (void*)tete+mem_space_get_size()) {
        fprintf(stderr, "Pointeur hors zone mémoire !"); exit(1);
    }

    // bloc occupee
    mem_busy_block_t* busy = zone - sizeof(mem_busy_block_t);    // Début de la zone à libérer
    size_t blocksize = busy->size;
    if (blocksize < sizeof(mem_free_block_t)) {
        fprintf(stderr, "Pointeur invalide !"); exit(1);
    }

    // creation du free block
    mem_free_block_t* freeblock = (mem_free_block_t*)busy;
    freeblock->size = blocksize;

    if(tete->first == NULL || tete->first > freeblock) { // S'il n'y a pas de bloc libre ou s'il n'y a pas de zone libre avant la zone que l'on veut libérer
        freeblock->next = tete->first;
        tete->first = freeblock;

        // fusion à droite
        if ((void*)freeblock + blocksize == freeblock->next) {
            freeblock->size += freeblock->next->size;
            freeblock->next = freeblock->next->next;
        }
    }

    else if(tete->first < freeblock){ // Si la zone que l'on veut libérer vient après le premier bloc libre
        // recherche du precedent
        mem_free_block_t *prec = tete->first;
        while(prec->next != NULL && prec->next < freeblock) prec = prec->next;
        if (!prec) {
            fprintf(stderr, "Chainage invalide !"); exit(1);
        }

        freeblock->next = prec->next;
        prec->next = freeblock;

        // fusion à droite
        if ((void*)freeblock + blocksize == freeblock->next) {
            freeblock->size += freeblock->next->size;
            freeblock->next = freeblock->next->next;
        }

        // fusion à gauche
        if ((void*)prec + prec->size == (void*)freeblock) {
            prec->size += freeblock->size;
            prec->next = freeblock->next;
        }
    }
}

//-------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
//-------------------------------------------------------------
void mem_show(void (*print)(void *, size_t, int free)) {
    // adresse et taille de la zone à afficher
    void *adr_debut = mem_space_get_addr();
    void *zone_adr = adr_debut;
    size_t zone_size = sizeof(mem_header_t);

    // cellule de la premiere zone libre
    mem_free_block_t* cell_adr = ((mem_header_t*)zone_adr)->first;

    // header est une zone occupee
    print(0, zone_size, 0);
    // zone suivante
    zone_adr += sizeof(mem_header_t);
    
    // on va parcourir jusqu'a l'@ de fin
    void *adr_fin = mem_space_get_addr()+mem_space_get_size()-1;
    while (zone_adr < adr_fin) {
        if (zone_adr == (void*)cell_adr) { // on a une zone libre
            print((void*)((size_t)zone_adr-(size_t)adr_debut), cell_adr->size, 1);
            zone_size = cell_adr->size;
            cell_adr = cell_adr->next;
        }
        else { // on a une zone occupee
            zone_size = ((mem_busy_block_t*)zone_adr)->size; // recupere la taille
            print((void*)((size_t)zone_adr-(size_t)adr_debut), zone_size, 0);     // affiche
        }

        // zone suivante
        zone_adr += zone_size;
    }
}

//-------------------------------------------------------------
// mem_fit
//-------------------------------------------------------------
void mem_set_fit_handler(mem_fit_function_t *mff) {
    get_free_block = mff;
}

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    // schema de recherche classique
    mem_free_block_t *cellule = first_free_block;
    while (cellule && cellule->size < wanted_size)
        cellule = cellule->next;

    return cellule;
}

//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    mem_free_block_t *best = NULL;
    mem_free_block_t *cell = first_free_block;

    // parcours de toute les cellules de blocs libres
    while (cell) {
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
