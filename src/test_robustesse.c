#include "mem.h"
#include "mem_os.h"
#include "mem_space.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Cette fonction teste si mem_free gère les différents cas où 
// l'on appelle mem_free de manière inappropriée

// Pour faire les tests, il faut enlever en commentaire la partie 
// que l'on veut tester et cela dans la fonction test_robustesse_mem_free
void test_robustesse_mem_free(){
    void *adr1, *adr2, *adr3;
    size_t taille_zone_1, taille_zone_2, taille_zone_3;
    taille_zone_1 = 2300;
    taille_zone_2 = 5300;
    taille_zone_3 = 7500;

    printf("1 - Cas où l'on libère deux fois la mème adresse \n");
    mem_init();
    adr1 = mem_alloc(taille_zone_1);
    adr2 = mem_alloc(taille_zone_2);
    adr3 = mem_alloc(taille_zone_3);

    mem_free(adr1);
    mem_free(adr2);
    mem_free(adr1);
    mem_free(adr3);

    // printf("2 - Cas où l'on appelle mem_free sur le milieu d'une zone \n");
    // mem_init();
    // adr1 = mem_alloc(taille_zone_1);
    // adr2 = mem_alloc(taille_zone_2);
    // adr3 = mem_alloc(taille_zone_3);

    // mem_free(adr1);
    // mem_free(adr3);
    // mem_free(adr2 + 100);

    // printf("3 - Cas où l'on appelle mem_free sur une zone hors mémoire\n");
    // mem_init();
    // adr1 = mem_alloc(taille_zone_1);
    // adr2 = mem_alloc(taille_zone_2);
    // adr3 = mem_alloc(taille_zone_3);

    // mem_free(adr1);
    // mem_free(adr2);
    // adr1 = mem_alloc(taille_zone_1);
    // mem_free(adr3);
    // mem_free(mem_space_get_addr() + mem_space_get_size() + 10000);
    // mem_free(mem_space_get_addr() - 100);
}


void test_acces_memoire_illegale(){
    mem_init();

    __uint16_t* p0 = mem_alloc(200);
    for(int i = 0; i < 200; i++){
        p0[i] = i * 2;
    }

    __uint64_t* p1 = (__uint64_t*)mem_alloc(2000);
    for(int i=0; i<2500; i++){
        p1[i] = i+200;
    }

    printf("Accès mémoire illégale\n");
    __uint64_t* p2 = (__uint64_t*)mem_alloc(3000);
    for(int i=0; i<2500; i++){
        p2[i] = i+200;
    }
}

int main(int argc, char** argv){
    // test_robustesse_mem_free();
    test_acces_memoire_illegale();
}