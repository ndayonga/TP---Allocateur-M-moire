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

#define max(a, b)       ((a) < (b) ? (b) : (a))
#define MIN_SIZE_BLOCK  max(aligned_sizeof(bb), aligned_sizeof(fb))

//-----------------------
// ALIGNEMENT
//-----------------------
#define ALIGNMENT 16
#define aligned_size(s) (\
    (((s) % ALIGNMENT) == 0) ? (s) : \
    ((s) + ALIGNMENT - ((s) % ALIGNMENT)) \
)
#define aligned_sizeof(s) aligned_size(sizeof(s))

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
    l->first = mem_space_get_addr() + aligned_sizeof(mem_header_t);
    l->fit_function = mem_first_fit;

    // on replit le bloc en debut de 
    fb* first = l->first;
    size_t size = mem_space_get_size() - aligned_sizeof(mem_header_t);
    size_t mod = size % ALIGNMENT;
    if (mod == 0) first->size = size;
    else first->size = size - mod;
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
    size_t wanted = max(aligned_size(size) + aligned_sizeof(bb), MIN_SIZE_BLOCK);
    size_t real_size;

    // on recupere le bloc libre qui nous interesse
    mem_free_block_t *free_block = (*tete->fit_function)(tete->first, wanted);

    // s'il n'y en a pas on renvoie NULL
    if (!free_block) return NULL;

    // Cas ou on peut mettre un bloc libre d'au moins MIN_SIZE_BLOCK octets 
    // après le bloc à allouer
    if (free_block->size > wanted + MIN_SIZE_BLOCK) {
        // on cree le nouveau bloc libre en fin de zone qui sera alloue
        mem_free_block_t *new_fb = (void*)free_block + wanted;
        new_fb->size = free_block->size - wanted;
        new_fb->next = free_block->next;

        if (tete->first == free_block) { // Si le bloc à allouer est le premier bloc libre de la memoire
            tete->first = new_fb;
        }
        else { // sinon on va chercher le bloc qui precede le bloc qui sera alloue
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
            // si bloc a allouer est le premier bloc libre, on passe au suivant
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

    // construction du busy block
    mem_busy_block_t *bb = (mem_busy_block_t*) free_block;
    bb->size = real_size;
    return (void*)free_block + aligned_sizeof(bb);
}

//-------------------------------------------------------------
// mem_get_size
//-------------------------------------------------------------
size_t mem_get_size(void * zone)
{
    mem_busy_block_t *bb = zone - aligned_sizeof(mem_busy_block_t);
    return bb->size - aligned_sizeof(mem_busy_block_t);
}

//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
/**
 * Free an allocaetd bloc.
**/
void mem_free(void *zone) {
    if(!zone) return;

    // zone memoire
    mem_header_t* tete = mem_space_get_addr();
    if ((void*)tete+sizeof(mem_header_t) > zone || zone > (void*)tete+mem_space_get_size()) {
        fprintf(stderr, "mem_free : pointeur hors zone mémoire !\n"); exit(1);
    }

    // bloc occupe    // Début de la zone à libérer
    mem_busy_block_t* busy = zone - aligned_sizeof(mem_busy_block_t);
    size_t blocksize = busy->size;
    if (blocksize < MIN_SIZE_BLOCK) {
        fprintf(stderr, "mem_free : pointeur invalide !\n"); exit(1);
    }

    // creation du free block
    mem_free_block_t* freeblock = (mem_free_block_t*)busy;
    freeblock->size = blocksize;

    // s'il n'y a pas de zone libre avant la zone que l'on veut libérer
    if(tete->first == NULL || tete->first > freeblock) {
        freeblock->next = tete->first;
        tete->first = freeblock;

        // fusion à droite
        if ((void*)freeblock + blocksize == freeblock->next) {
            freeblock->size += freeblock->next->size;
            freeblock->next = freeblock->next->next;
        }
    }

    // Si la zone que l'on veut libérer vient après le premier bloc libre
    else if(tete->first <= freeblock) { 
        // recherche du precedent
        mem_free_block_t *prec = tete->first;

        // dans une zone libre !!!
        if(prec == freeblock || (prec < freeblock && (void*)freeblock < (void*)prec + prec->size)){
            fprintf(stderr, "mem_free: zone déjà libre !\n"); exit(1);
        }

        while(prec->next && prec->next <= freeblock){
            if (prec->size < MIN_SIZE_BLOCK) {
                fprintf(stderr, "mem_free: chainage corrompu !\n"); exit(1);
            }
            prec = prec->next;
        }

        if (!prec) {
            fprintf(stderr, "mem_free : chainage invalide !\n"); exit(1);
        }
        if (prec->size < MIN_SIZE_BLOCK) {
            fprintf(stderr, "mem_free: chainage corrompu !\n"); exit(1);
        }
        if (prec->next == freeblock) {
            fprintf(stderr, "mem_free: zone déjà libre !\n"); exit(1);
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
    size_t zone_size = aligned_sizeof(mem_header_t);

    // cellule de la premiere zone libre
    mem_free_block_t* cell_adr = ((mem_header_t*)zone_adr)->first;

    // header est une zone occupee
    print(0, zone_size, 0);
    // zone suivante
    zone_adr += zone_size;
    
    size_t mem_size = mem_space_get_size() - (mem_space_get_size() % ALIGNMENT);
    void *adr_fin = mem_space_get_addr()+mem_size-1;
    
    // on va parcourir jusqu'a l'@ de fin
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
    mem_header_t *header = mem_space_get_addr();
    header->fit_function = mff;
}

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    void *memory = mem_space_get_addr();
    size_t mem_size = mem_space_get_size();

    // schema de recherche classique
    mem_free_block_t *cell = first_free_block;
    while (cell && cell->size < wanted_size) {
        // cas d'erreur
        if (cell->size < MIN_SIZE_BLOCK) {
            // La taille minimale d'un bloc est de sizeof(fb) octets
            fprintf(stderr, "mem_alloc : chainage corrompu !\n");
            exit(1);
        }

        cell = cell->next;
        if (cell && ((void*)cell < memory + sizeof(mem_header_t) ||
            (void*)cell >= memory + mem_size))
        {
            // bloc hors zone mémoire
            fprintf(stderr, "mem_alloc : chainage invalide !\n");
            exit(1);
        }
    }

    return cell;
}

//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    void *memory = mem_space_get_addr();
    size_t mem_size = mem_space_get_size();

    mem_free_block_t *best = NULL;
    mem_free_block_t *cell = first_free_block;

    // parcours de toute les cellules de blocs libres
    while (cell) {
        // erreur
        if (cell->size < MIN_SIZE_BLOCK) {
            // La taille minimale d'un bloc est de sizeof(fb) octets
            fprintf(stderr, "mem_alloc : chainage corrompu !\n");
            exit(1);
        }

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
        if (cell && ((void*)cell < memory + sizeof(mem_header_t) ||
            (void*)cell >= memory + mem_size))
        {
            // bloc hors zone mémoire
            fprintf(stderr, "mem_alloc : chainage invalide !\n");
            exit(1);
        }
    }
    
    return best;
}

//-------------------------------------------------------------
mem_free_block_t *mem_worst_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    void *memory = mem_space_get_addr();
    size_t mem_size = mem_space_get_size();


    mem_free_block_t *block = first_free_block;
    mem_free_block_t *worst = NULL;
    size_t size_max = 0; // taille max initiale = 0

    while (block) {
        // erreur
        if (block->size < MIN_SIZE_BLOCK) {
            // La taille minimale d'un bloc est de sizeof(fb) octets
            fprintf(stderr, "mem_alloc : chainage corrompu !\n");
            exit(1);
        }

        // si c'est mieux qu'auparavant
        if (block->size > size_max && block->size >= wanted_size) {
            worst = block;
            size_max = block->size;
        }

        block = block->next; // au suivant
        
        if (block && ((void*)block < memory + sizeof(mem_header_t) ||
            (void*)block >= memory + mem_size))
        {
            // bloc hors zone mémoire
            fprintf(stderr, "mem_alloc : chainage invalide !\n");
            exit(1);
        }
    }

    return worst;
}
