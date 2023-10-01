#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "mem_os.h"

int main(int argc, char const *argv[])
{
    mem_header_t h;
    h.first = NULL;

    //----------------------------------------------------------------
    printf("Test de mem_first_fit pour une taille de 100 octests\n");
    printf("plus de place dans la memoire : @=%p\n", mem_first_fit(h.first, 100));

    mem_free_block_t fb1_1 = {10, NULL};
    h.first = &fb1_1;
    printf("Ajout d'un bloc de taille 10 à l'adresse %p\n", &fb1_1);
    printf("pas de bloc assez grand : @=%p\n", mem_first_fit(h.first, 100));

    mem_free_block_t fb1_2 = {100, NULL};
    fb1_1.next = &fb1_2;
    printf("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb1_2);
    printf("2e bloc assez grand : @=%p\n", mem_first_fit(h.first, 100));

    mem_free_block_t fb1_3 = {100, NULL};
    fb1_2.next = &fb1_3;
    printf("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb1_3);
    printf("2e bloc assez grand : @=%p\n", mem_first_fit(h.first, 100));

    //----------------------------------------------------------------
    printf("--------------------------------------------------------\n");
    printf("Test de mem_best_fit pour une taille de 100 octests\n");
    h.first = NULL;

    printf("plus de place dans la memoire : @=%p\n", mem_first_fit(h.first, 100));

    mem_free_block_t fb2_1 = {10, NULL};
    h.first = &fb2_1;
    printf("Ajout d'un bloc de taille 10 à l'adresse %p\n", &fb2_1);
    printf("pas de bloc assez grand : @=%p\n", mem_best_fit(h.first, 100));

    mem_free_block_t fb2_2 = {200, NULL};
    fb2_2.next = &fb2_1;
    printf("Ajout d'un bloc de taille 200 à l'adresse %p\n", &fb2_2);
    printf("2e bloc assez grand : @=%p\n", mem_best_fit(h.first, 100));

    mem_free_block_t fb2_3 = {150, NULL};
    fb2_2.next = &fb2_3;
    printf("Ajout d'un bloc de taille 150 à l'adresse %p\n", &fb2_3);
    printf("3e bloc meilleure taille : @=%p\n", mem_best_fit(h.first, 100));

    mem_free_block_t fb2_4 = {100, NULL};
    fb2_3.next = &fb2_4;
    printf("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb2_4);
    printf("4e bloc meilleure taille : @=%p\n", mem_best_fit(h.first, 100));

    mem_free_block_t fb2_5 = {150, NULL};
    fb2_4.next = &fb2_5;
    printf("Ajout d'un bloc de taille 150 à l'adresse %p\n", &fb2_5);
    printf("toujours 4e bloc meilleure taille : @=%p\n", mem_best_fit(h.first, 100));

    //----------------------------------------------------------------
    printf("--------------------------------------------------------\n");
    printf("Test de mem_worst_fit pour une taille de 100 octests\n");
    h.first = NULL;
    printf("plus de place dans la memoire header.first = NULL : @=%p\n", mem_worst_fit(h.first, 100));

    mem_free_block_t fb3_1 = {10, NULL};
    h.first = &fb3_1;
    printf("Ajout d'un bloc de taille 10 à l'adresse %p\n", &fb3_1);
    printf("pas de bloc assez grand : @=%p\n", mem_worst_fit(h.first, 100));

    mem_free_block_t fb3_2 = {200, NULL};
    fb3_1.next = &fb3_2;
    printf("Ajout d'un bloc de taille 200 à l'adresse %p\n", &fb3_2);
    printf("2e bloc le plus grand : @=%p\n", mem_worst_fit(h.first, 100));

    mem_free_block_t fb3_3 = {150, NULL};
    fb3_2.next = &fb3_3;
    printf("Ajout d'un bloc de taille 150 à l'adresse %p\n", &fb3_3);
    printf("2e bloc le plus grand : @=%p\n", mem_worst_fit(h.first, 100));

    mem_free_block_t fb3_4 = {300, NULL};
    fb3_3.next = &fb3_4;
    printf("Ajout d'un bloc de taille 300 à l'adresse %p\n", &fb3_4);
    printf("4e bloc le plus grand : @=%p\n", mem_worst_fit(h.first, 100));

    mem_free_block_t fb3_5 = {200, NULL};
    fb3_4.next = &fb3_5;
    printf("Ajout d'un bloc de taille 200 à l'adresse %p\n", &fb3_5);
    printf("toujours 4e bloc le plus grand : @=%p\n", mem_worst_fit(h.first, 100));

    mem_free_block_t fb3_6 = {100, NULL};
    fb3_5.next = &fb3_6;
    printf("Ajout d'un bloc de taille 100 à l'adresse %p\n", &fb3_6);
    printf("toujours 4e bloc le plus grand : @=%p\n", mem_worst_fit(h.first, 100));

    return 0;
}