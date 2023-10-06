#include "mem_space.h"
#include "mem.h"
#include "mem_os.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void afficher_zone(void *adresse, size_t taille, int free) {
    printf("Zone %s, Adresse : %lu, Taille : %lu\n", free ? "libre" : "occupee",
           (unsigned long)adresse, (unsigned long)taille);
}

int main(void)
{
    mem_init();
    void *adr = mem_alloc(128000-sizeof(header)-sizeof(bb));
    mem_show(&afficher_zone);
    mem_free(adr);
    printf("---------------------------\n");
    mem_show(&afficher_zone);
    printf("---------------------------\n");

    mem_init();
    adr = mem_alloc(2048);
    printf("---------------------------\n");
    mem_show(&afficher_zone);
    mem_free(adr);
    printf("---------------------------\n");
    mem_show(&afficher_zone);
    printf("---------------------------\n");

    mem_init();
    adr = mem_alloc(1024);
    void *adr2 = mem_alloc(1024);
    printf("---------------------------\n");
    mem_show(&afficher_zone);
    mem_free(adr);
    printf("---------------------------\n");
    mem_show(&afficher_zone);
    mem_free(adr2);
    printf("---------------------------\n");
    mem_show(&afficher_zone);

    return 0;
}
