#include "mem.h"
#include "mem_os.h"
#include "mem_space.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Les fonctions suivantes testent si mem_free gère les différents
// cas où l'on appelle mem_free de manière inappropriée
void test_deux_liberations() {
    printf("==== 1 - On libère deux fois la même zone ====\n");

    mem_init();
    void *adr1 = mem_alloc(2300);
    void *adr2 = mem_alloc(5300);
    void *adr3 = mem_alloc(7500);

    mem_free(adr1);
    mem_free(adr2);
    mem_free(adr3);
    mem_free(adr2);
}

void test_free_milieu_zone() {
    printf("==== 2 - On libère au milieu d'une zone ====\n");

    mem_init();
    void *adr1 = mem_alloc(2300);
    char *adr2 = mem_alloc(5300);
    void *adr3 = mem_alloc(7500);

    mem_free(adr3);
    mem_free(adr1);
    mem_free(&adr2[10]);
}

void test_hors_memoire_apres() {
    printf("==== 3 - Appel mem_free après memoire ====\n");

    mem_init();
    void *adr1 = mem_alloc(2300);
    void *adr2 = mem_alloc(5300);
    void *adr3 = mem_alloc(7500);

    mem_free(adr1);
    mem_free(adr2);
    adr1 = mem_alloc(2300);
    mem_free(adr3);
    mem_free(mem_space_get_addr() + mem_space_get_size() + 10000);
}

void test_hors_memoire_avant() {
    printf("==== 4 - Appel mem_free avant memoire ====\n");

    mem_init();
    void *adr1 = mem_alloc(2300);
    void *adr2 = mem_alloc(5300);
    void *adr3 = mem_alloc(7500);

    mem_free(adr1);
    mem_free(adr2);
    mem_free(adr3);
    mem_free(mem_space_get_addr() - 8);
}


// Les fonctions suivantes testent ce qui se passe si on ecrase
// des structures de gestion (header, fb, bb).
void test_ecrase_header() {
    printf("==== 5 - On écrase le header ====\n");

    mem_init();
    uint64_t *adr = mem_space_get_addr();
    adr[0] = 20;
    adr[1] = 30;
    adr[2] = 40;

    adr = mem_alloc(50);
    mem_free(adr);
}

void test_ecrase_fb() {
    printf("==== 6 - Ecrase le seul fb ====\n");
    mem_init();

    uint16_t* p0 = (uint16_t *)mem_alloc(200 * sizeof(uint16_t));
    for(int i = 0; i < 200; i++) p0[i] = i * 2;

    uint64_t* p1 = (uint64_t*)mem_alloc(2000 * sizeof(uint64_t));
    for(int i = 0; i < 2500; i++) p1[i] = i + 200;

    // ici c'est la loterie
    uint64_t* p2 =(uint64_t*) mem_alloc(3000 * sizeof(uint64_t));
    for (int i=0; i<2500; i++) p2[i] = i + 200;
}


void test_ecrase_fb_2() {
    printf("==== 7 - Ecrase le seul fb sans erreur ====\n");
    mem_init();

    uint16_t* p0 = (uint16_t *)mem_alloc(3000 * sizeof(uint16_t));
    uint16_t* p1 = (uint16_t *)mem_alloc(2000 * sizeof(uint16_t));
    
    printf("t = allouer 3000; t2 = allouer 2000;\n t[i] = 2*i pour i=0..2999\n");
    for(int i = 0; i < 3000; i++) p0[i] = i * 2;

    printf("t2[i] = i+200 pour i=0..4999 !!\n");
    for(int i = 0; i < 5000; i++) p1[i] = i + 200;

    printf("On est passé !\n t = allouer 3000; t[i] = i + 2000 pour i=0..2999\n");
    printf("Ici on sait pas trop ou on écrit....\n");
    uint16_t* p2 = (uint16_t *)mem_alloc(3000 * sizeof(uint16_t));
    for(int i = 0; i < 3000; i++) p2[i] = i + 200;

    printf("On libère les trois blocs !\n");
    mem_free(p0);
    mem_free(p1);
    mem_free(p2);    
}

void test_ecrase_bb() {
    printf("==== 8 - On ecrabouille deuxième bb avec erreur  ====\n");

    mem_init();
    char *adr = mem_alloc(16 * sizeof(char));
    char *adr2 = mem_alloc(64 * sizeof(char));

    for (int i = 0; i < 80; i++) adr[i] = 0;
    
    mem_free(adr2);
    mem_free(adr);
}

void test_ecrase_bb_2() {
    printf("==== 9 - On ecrabouille deuxième bb sans erreur  ====\n");

    mem_init();
    char *adr = mem_alloc(16 * sizeof(char));
    char *adr2 = mem_alloc(64 * sizeof(char));

    for (int i = 0; i < 80; i++) adr[i] = i;
    
    mem_free(adr);
    mem_free(adr2);

    adr = mem_alloc(64);
    mem_free(adr);
}

// fonction principale : selection de la fonction à tester n°1 à 9
int main(int argc, char** argv){
    if (argc < 2) {
        printf("Usage: %s <numero_test>\n", argv[0]);
        printf("\t1 - libere deux fois la même zone\n");
        printf("\t2 - libere au milieu d'une zone\n");
        printf("\t3 - appel mem_free apres zone memoire\n");
        printf("\t4 - appel mem_free avant zone memoire\n");
        printf("\t5 - ecrabouille le header\n");
        printf("\t6 - ecrase un fb avec erreur\n");
        printf("\t7 - ecrase un fb sans erreur\n");
        printf("\t8 - ecrase un bb avec erreur\n");
        printf("\t9 - ecrase un bb sans erreur\n");
        exit(0);
    }
    
    unsigned int numero = atoi(argv[1]);
    switch (numero) {
        case 1: test_deux_liberations(); break;
        case 2: test_free_milieu_zone(); break;
        case 3: test_hors_memoire_apres(); break;
        case 4: test_hors_memoire_avant(); break;
        case 5: test_ecrase_header(); break;
        case 6: test_ecrase_fb(); break;
        case 7: test_ecrase_fb_2(); break;
        case 8: test_ecrase_bb(); break;
        case 9: test_ecrase_bb_2(); break;
        default: break;
    }
}
