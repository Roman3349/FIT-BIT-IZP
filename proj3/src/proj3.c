/*
 * Copyright (C) 2018  Roman Ondráček <xondra58@stud.fit.vutbr.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

/// Vypíše ladící řetězec
#define debug(s) printf("- %s\n", s)

/// Vypíše formátovaný ladící výstup - použití podobné jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

/// Vypíše ladící informaci o proměnné - použití dint(identifikator_promenné)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

/// Vypíše ladící informaci o proměnné typu float - použití - dfloat(identifikátor_promenné)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/**
 * Chybové stavy
 */
enum exitStatuses {
    /// Nenastala žádná chyba
    NO_ERROR,
    /// Nastala chyba při převodu řetězce na číslo
    CONVERSION_ERROR,
    /// Nastala chyba při čtení souboru
    FILE_READ_ERROR,
    /// Nastala chyba při alokaci paměti
    ALLOCATION_ERROR,
};

/**
 * Struktura objektu
 */
struct obj_t {
    /// Identifikátor objektu
    int id;
    /// Souřadnice - x
    float x;
    /// Souřadnice - y
    float y;
};

/**
 * Struktura shluku
 */
struct cluster_t {
    /// Počet objektů ve shluku
    int size;
    /// Kapacita shluku
    int capacity;
    /// Ukazatel na pole shluků
    struct obj_t *obj;
};

/**
 * Inicializace shluku
 * Alokuje paměť pro počet objektů.
 * @param c Shluk
 * @param cap Kapacita pole
 */
void init_cluster(struct cluster_t *c, int cap) {
    assert(c != NULL);
    assert(cap >= 0);
    size_t clusterSize = sizeof(struct obj_t) * cap;
    c->obj = malloc(clusterSize);
    if (c->obj == NULL) {
        return;
    }
    c->size = 0;
    c->capacity = cap;

}

/**
 * Odstranění všech objektů shluku a inicializace na prázdný shluk
 * @param c Shluk
 */
void clear_cluster(struct cluster_t *c) {
    if (c->obj != NULL) {
        free(c->obj);
        c->obj = NULL;
    }
    c->capacity = 0;
    c->size = 0;

}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/**
 * Změna kapacity shluku
 * @param c Shluk
 * @param new_cap Nová kapacita shluku
 * @return Shluk
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap) {
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);
    size_t size = sizeof(struct obj_t) * new_cap;
    struct obj_t *arr = realloc(c->obj, size);
    if (arr == NULL) {
        return NULL;
    }
    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/**
 * Přidá objekt na konec shluku
 * Rozšíří shluk, pokud se do něj objekt nevejde.
 * @param c Shluk
 * @param obj Objekt, který chceme do shluku přidat
 */
void append_cluster(struct cluster_t *c, struct obj_t obj) {
    if (c->size == c->capacity) {
        if (resize_cluster(c, c->capacity + CLUSTER_CHUNK) == NULL) {
            return;
        }
    }
    if (c->obj == NULL) {
        return;
    }
    if (c->size < c->capacity) {
        c->obj[c->size] = obj;
        c->size++;
    }
}

/**
 * Seřadí vzestupně objekty ve shluku podle jejich ID
 * @param c Shluk
 */
void sort_cluster(struct cluster_t *c);

/**
 * Do prvního shluku se přidají objekty z druhého shluku. A seřadí první shluk.
 * @param c1 První shluk
 * @param c2 Druhý shluk
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2) {
    assert(c1 != NULL);
    assert(c2 != NULL);
    dint(c2->size);
    if (resize_cluster(c1, c1->size + c2->size) == NULL) {
        return;
    }
    for (int i = 0; i < c2->size; i++) {
        append_cluster(c1, c2->obj[i]);
    }
    sort_cluster(c1);
}

/**
 * Odstraní shluk z pole
 * @param carr Pole shluků
 * @param narr Počet shluků v poli
 * @param idx Index shluku, který chceme odstranit
 * @return Nový počet položek v poli
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx) {
    assert(idx < narr);
    assert(narr > 0);
    clear_cluster(&carr[idx]);
    for (int i = idx; i < (narr - 1); i++) {
        carr[i] = carr[i + 1];
    }
    return --narr;
}

/**
 * Počítá Euklidovskou vzdálenost mezi dvěma objekty
 * @param o1 První objekt
 * @param o2 Druhý objekt
 * @return Euklidovská vzdálenost mezi dvěma objekty
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2) {
    assert(o1 != NULL);
    assert(o2 != NULL);
    float newX = o1->x - o2->x;
    float newY = o1->y - o2->y;
    return sqrtf((newX * newX) + (newY * newY));
}

/**
 * Počítá vzdálenost dvou shluků
 * @param c1 První shluk
 * @param c2 Druhý shluk
 * @return Vzálenost dvou shluků
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2) {
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);
    float distance, finalDistance = 0;
    for (int x = 0; x < c1->size; x++) {
        for (int y = 0; y < c2->size; y++) {
            distance = obj_distance(&c1->obj[x], &c2->obj[y]);
            // Je první iterace cyklu nebo aktuální vzálenost je menší než předchozí
            if ((x == 0 && y == 0) || distance < finalDistance) {
                finalDistance = distance;
            }
        }
    }
    return finalDistance;
}

/**
 * Funkce najde dva nejbližší shluky.
 * Funkce nalezene shluky uklada do pameti na adresu 'c1' resp. 'c2'.
 * @param carr Pole shluků
 * @param narr Velikost pole shluků
 * @param c1 První shluk
 * @param c2 Druhý shluk
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2) {
    assert(narr > 0);
    float finalDistance = INFINITY;
    float distance = 0;
    for (int x = 0; x < narr; x++) {
        for (int y = 0; y < narr; y++) {
            if (x == y) {
                continue;
            }
            distance = cluster_distance(&carr[x], &carr[y]);
            if (distance < finalDistance) {
                finalDistance = distance;
                *c1 = x;
                *c2 = y;
            }
        }
    }
}

/**
 * Pomocná funkce pro řazení shluku
 * @param a První objekt
 * @param b Druhý objekt
 * @return
 */
static int obj_sort_compar(const void *a, const void *b) {
    const struct obj_t *o1 = (const struct obj_t *) a;
    const struct obj_t *o2 = (const struct obj_t *) b;
    if (o1->id < o2->id) {
        return -1;
    }
    if (o1->id > o2->id) {
        return 1;
    }
    return 0;
}

/**
 * Seřadí objekty ve shluku podle jejich ID
 * @param c Shluk
 */
void sort_cluster(struct cluster_t *c) {
    qsort(c->obj, (size_t) c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
 * Vytiskne skluk
 * @param c Shluk
 */
void print_cluster(struct cluster_t *c) {
    for (int i = 0; i < c->size; i++) {
        if (i) {
            putchar(' ');
        }
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/**
 * Tiskne pole shluků
 * @param carr Ukazatel na první shluk
 * @param narr Počet vytištěných shluků
 */
void print_clusters(struct cluster_t *carr, int narr) {
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++) {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/**
 * Načte shluk a přidá ho do pole
 * @param file Soubor
 * @param arr Pole shluků
 * @param clusters Počet shluků
 * @return Počet načtených shluků
 */
int parse_clusters(FILE* file, struct cluster_t **arr, int clusters) {
    struct obj_t object = {0, 0.0, 0.0};
    int i = 0;
    while ((i < clusters) && (fscanf(file, "%d %f %f\n", &object.id, &object.x, &object.y) == 3)) {
        init_cluster(&arr[0][i], CLUSTER_CHUNK);
        append_cluster(&arr[0][i], object);
        i++;
    }
    return i;
}

/**
 * Načte objekty ze souboru. Pro každý objekt vytvoří shluk a uloží jej do pole shluků.
 * @param filename Vstupní soubor
 * @param arr Pole shluků
 * @return Počet načtenách shluků nebo chyba
 */
int load_clusters(char *filename, struct cluster_t **arr) {
    assert(arr != NULL);
    FILE *file;
    if ((file = fopen(filename, "r")) == NULL) {
        perror("proj3");
        *arr = NULL;
        return -FILE_READ_ERROR;
    }
    int clusterCount = 0;
    if (fscanf(file, "count=%d\n", &clusterCount) != 1) {
        fclose(file);
        return -CONVERSION_ERROR;
    }
    dint(clusterCount);
    *arr = malloc(clusterCount * sizeof(struct cluster_t));
    if (*arr == NULL) {
        fclose(file);
        return -ALLOCATION_ERROR;
    }
    int count = parse_clusters(file, arr, clusterCount);
    fclose(file);
    return count;
}

/**
 * Uvolní paměť pole shluků
 * @param arr Pole shluků
 * @param size Počet shluků v poli
 */
void clear_clusters(struct cluster_t *arr, int size) {
    // Uvolní paměť použitou pro shluky v poli
    for (int i = 0; i < size; i++) {
        clear_cluster(&arr[i]);
    }
    // Uvolní paměť použitou pro pole
    free(arr);
}

/**
 * Spojí dva shluky, které mají k sobě nejblíže.
 * @param clusters Pole shluků
 * @param clusterCount Počet načtených shluků
 * @param finalCount Finální počet shluků
 */
void combine_clusters(struct cluster_t *clusters, int clusterCount, int finalCount) {
    while (clusterCount > finalCount) {
        int cluster1 = 0;
        int cluster2 = 0;
        // Najde nejbližšího souseda
        find_neighbours(clusters, clusterCount, &cluster1, &cluster2);
        dint(clusters[cluster1].obj->id);
        dint(clusters[cluster2].obj->id);
        // Spojí dva nejbližší shluky
        merge_clusters(&clusters[cluster1], &clusters[cluster2]);
        // Odstraní spojený shluk
        clusterCount = remove_cluster(clusters, clusterCount, cluster2);
    }
}

/**
 * Převede řetězec na číslo
 * @param string Řetězec, který chceme převést
 * @param number Převedené číslo
 * @return Stav
 */
int strToInt(char *string, int *number) {
    char *endptr;
    *number = (int) strtol(string, &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Error in string conversion to unsigned int.\n");
        return CONVERSION_ERROR;
    }
    return NO_ERROR;
}

/**
 * Vypíše použití programu
 */
int print_usage() {
    puts("Usage: ./proj3 filename [N]");
    puts("Options:");
    puts("\tN\t\tFinal count of clusters.");
    return NO_ERROR;
}

/**
 * Hlavní funkce programu
 * @param argc Počet argumentů programu
 * @param argv Pole argumentů programu
 * @return Stav
 */
int main(int argc, char *argv[]) {
    struct cluster_t *clusters;
    int finalCount = 1;
    if (argc != 2 && argc != 3) {
        return print_usage();
    }
    if ((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {
        return print_usage();
    }
    if (argc == 3) {
        int status = strToInt(argv[2], &finalCount);
        if (status != NO_ERROR) {
            return status;
        }
    }
    int clusterCount = load_clusters(argv[1], &clusters);
    if (clusterCount < 0) {
        free(clusters);
        return -clusterCount;
    }
    combine_clusters(clusters, clusterCount, finalCount);
    print_clusters(clusters, finalCount);
    clear_clusters(clusters, finalCount);
    return NO_ERROR;
}
