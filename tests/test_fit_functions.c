#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "mem_os.h"

int main(int argc, char const *argv[])
{
    mem_header_t h;
    h.first = NULL;

    //----------------------------------------------------------------
    printf("Test de mem_first_fit\n");
    printf("plus de place dans la memoire : @=%p\n", mem_first_fit(h.first, 100));

    mem_free_block_t fb = {10, NULL};
    h.first = &fb;
    printf("Ajout d'un bloc de taille 10 à l'adresse %p\n", &fb);
    printf("pas de bloc assez grand : @=%p\n", mem_first_fit(h.first, 100));

    mem_free_block_t fb2 = {100, NULL};
    fb.next = &fb2;
    printf("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb2);
    printf("2e bloc assez grand : @=%p\n", mem_first_fit(h.first, 100));

    //----------------------------------------------------------------
    printf("--------------------------------------------------------\n");
    printf("Test de mem_best_fit\n");
    h.first = NULL;

    printf("plus de place dans la memoire : @=%p\n", mem_first_fit(h.first, 100));

    fb.next = NULL;
    h.first = &fb;
    printf("Ajout d'un bloc de taille 10 à l'adresse %p\n", &fb);
    printf("pas de bloc assez grand : @=%p\n", mem_first_fit(h.first, 100));

    mem_free_block_t fb3 = {200, NULL};
    fb.next = &fb3;
    printf("Ajout d'un bloc de taille 200 à l'adresse %p\n", &fb3);
    printf("2e bloc assez grand : @=%p\n", mem_first_fit(h.first, 100));

    mem_free_block_t fb4 = {150, NULL};
    fb3.next = &fb4;
    printf("Ajout d'un bloc de taille 150 à l'adresse %p\n", &fb4);
    printf("3e bloc meilleure taille : @=%p\n", mem_first_fit(h.first, 100));

    return 0;
}