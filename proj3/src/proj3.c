/**
 * Kostra programu pro 3. projekt IZP 2018/19
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

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
    if (c != NULL && c->obj != NULL) {
        free(c->obj);
        c->obj = NULL;
    }
    c->capacity = c->size = 0;
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

    if (c->capacity >= new_cap) {
        return c;
    }
    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL) {
        return NULL;
    }
    c->obj = (struct obj_t *) arr;
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
        resize_cluster(c, c->capacity + CLUSTER_CHUNK);
    } else if (c->size < c->capacity) {
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
    int origSize = c1->size;
    int newSize = c1->size + c2->size;
    if (c1->capacity < newSize) {
        *c1 = *resize_cluster(c1, newSize);
    }
    for (int i = origSize; i < newSize; i++) {
        c1->obj[i] = c2->obj[newSize - i];
    }
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx) {
    assert(idx < narr);
    assert(narr > 0);

    // TODO
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

    // TODO
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2) {
    assert(narr > 0);

    // TODO
}

/**
 * Pomocná funkce pro řazení shluku
 * @param a
 * @param b
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
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
 * Vytiskne skluk
 * @param c Shluk
 */
void print_cluster(struct cluster_t *c) {
    for (int i = 0; i < c->size; i++) {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr) {
    assert(arr != NULL);

    // TODO
}

/**
 * Tiskne pole shluků
 * @param carr Ukazatel na první shluk
 * @param narr POčet vytištěných shluků
 */
void print_clusters(struct cluster_t *carr, int narr) {
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++) {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/**
 * Vypíše použití programu
 */
void print_usage() {
    puts("Usage: ./proj3 filename [N]");
    puts("Options:");
    puts("\tN\t\tFinal count of clusters.");
}

int main(int argc, char *argv[]) {
    struct cluster_t *clusters;
    if (argc != 2 && argc != 3) {
        print_usage();
        return 0;
    }
    return 0;
}
