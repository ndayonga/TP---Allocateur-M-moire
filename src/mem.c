//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#include "mem.h"
#include "mem_space.h"
#include "mem_os.h"
#include <assert.h>

//--------------------------------------
// Structures utilisée
//--------------------------------------


// cellule presente au debut d'un bloc libre
typedef struct fb_ {
    size_t size;
    struct fb_ *next; // chainaige vers le prochain bloc libre
} fb;

// liste chainee des zones libres
typedef struct { fb *first; } header;

// structure presente au debut de chaque bloc occupe
typedef struct { size_t size; } bb;


//-------------------------------------------------------------
// mem_init
//-------------------------------------------------------------
/**
 * Initialize the memory allocator.
 * If already init it will re-init.
**/
void mem_init() {
    // on place la tete de la liste des zones libres en memoire
    header* l = mem_space_get_addr();
    l->first = mem_space_get_addr() + sizeof(header);

    // on replit le bloc en debut de 
    fb* first = l->first;
    first->size = mem_space_get_size() - sizeof(header);
    first->next = NULL;
}


//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/
void *mem_alloc(size_t size) {
    //TODO: implement
    assert(! "NOT IMPLEMENTED !");
    return NULL;
}

//-------------------------------------------------------------
// mem_get_size
//-------------------------------------------------------------
size_t mem_get_size(void * zone)
{
    //TODO: implement
    assert(! "NOT IMPLEMENTED !");
    return 0;
}

//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
/**
 * Free an allocaetd bloc.
**/
void mem_free(void *zone) {
    //TODO: implement
    assert(! "NOT IMPLEMENTED !");
}

//-------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
//-------------------------------------------------------------
void mem_show(void (*print)(void *, size_t, int free)) {
    // adresse et taille de la zone à afficher
    void *zone_adr = mem_space_get_addr();
    size_t zone_size = sizeof(header);

    // cellule de la premiere zone libre
    fb* cell_adr = ((header*)zone_adr)->first;

    // header est une zone occupee
    print(zone_adr, zone_size, 0);
    // zone suivante
    zone_adr += sizeof(header);
    
    // on va parcourir jusqu'a l'@ de fin
    void *adr_fin = mem_space_get_addr()+mem_space_get_size()-1;
    while (zone_adr < adr_fin) {
        if (zone_adr == (void*)cell_adr) { // on a une zone libre
            print(cell_adr, cell_adr->size, 1);
            zone_size = cell_adr->size;
            cell_adr = cell_adr->next;
        }
        else { // on a une zone occupee
            zone_size = ((bb*)zone_adr)->size; // recupere la taille
            print(zone_adr, zone_size, 0);     // affiche
        }

        // zone suivante
        zone_adr += zone_size;
    }
}

//-------------------------------------------------------------
// mem_fit
//-------------------------------------------------------------
void mem_set_fit_handler(mem_fit_function_t *mff) {
    //TODO: implement
    assert(! "NOT IMPLEMENTED !");
}

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
    assert(! "NOT IMPLEMENTED !");
    return NULL;
}
//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
    assert(! "NOT IMPLEMENTED !");
    return NULL;
}

//-------------------------------------------------------------
mem_free_block_t *mem_worst_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
    assert(! "NOT IMPLEMENTED !");
    return NULL;
}
