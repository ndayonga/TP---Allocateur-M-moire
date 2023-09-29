//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#include "mem.h"
#include "mem_space.h"
#include "mem_os.h"
#include <assert.h>

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
    size_t zone_size = sizeof(mem_header_t);

    // cellule de la premiere zone libre
    mem_free_block_t* cell_adr = ((mem_header_t*)zone_adr)->first;

    // header est une zone occupee
    print(zone_adr, zone_size, 0);
    // zone suivante
    zone_adr += sizeof(mem_header_t);
    
    // on va parcourir jusqu'a l'@ de fin
    void *adr_fin = mem_space_get_addr()+mem_space_get_size()-1;
    while (zone_adr < adr_fin) {
        if (zone_adr == (void*)cell_adr) { // on a une zone libre
            print(cell_adr, cell_adr->size, 1);
            zone_size = cell_adr->size;
            cell_adr = cell_adr->next;
        }
        else { // on a une zone occupee
            zone_size = ((mem_busy_block_t*)zone_adr)->size; // recupere la taille
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
    mem_free_block_t *cellule = first_free_block;
    while (cellule && cellule->size < wanted_size)
        cellule = cellule->next;

    return cellule;
}
//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    if (!first_free_block) return NULL;

    mem_free_block_t *best = NULL;
    mem_free_block_t *cell = first_free_block;

    // parcours de toute les cellules de blocs libres
    while (cell) {
        if (best) {
            if (cell->size >= wanted_size && best->size > cell->size)
                best = cell;
        }
        else {
            if (cell->size >= wanted_size)
                best = cell;
        }

        cell = cell->next;
    }
    
    return best;
}

//-------------------------------------------------------------
mem_free_block_t *mem_worst_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    if (first_free_block == NULL){
        return NULL;
    }
    int size_diff;
	fb* block;
	// fb* ff_block = mem_space_get_addr() + sizeof(header);		// first_free_block
	block = first_free_block;		// Initialement, le bloc à retourner est le premier block libre
	int size_diff_max = first_free_block->size - (wanted_size);
	if (size_diff_max < 0)
		size_diff_max = 0;
	while(first_free_block->next != NULL){
		first_free_block = first_free_block->next;
		size_diff = first_free_block->size - (wanted_size);
		if(size_diff > size_diff_max){
			block = first_free_block;
			size_diff_max = size_diff;
		}
	}
	if (size_diff_max == 0){
		return NULL;
	}
	return block;
}
