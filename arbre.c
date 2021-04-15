#include <stdlib.h>
#include "arbre.h"


/**
 * Copie une donnée dans une autre. Cette fonction est utile lorsque
 * le type ValType est complexe et que l'opérateur d'affectation du C
 * ne fonctionne pas bien.
 *
 * @param d1 un pointeur vers la première donnée à recopier.  
 *
 * @param d2 un pointeur vers la deuxième donnée, qui sera écrasée et
 * prendra les valeurs de d1.
 */
void CopierDonnees(Donnee *d1, Donnee *d2) {
    *d2 = *d1;
}


/**
 * @return l'arbre vide.
 */
Arbre *ArbreVide() {
    return NULL;
}

/**
 * Détruit un arbre et désalloue tous ses éléments alloués.
 *
 * @param A un pointeur vers un arbre valide.
 */
void Detruire(Arbre *A) {
    if (A != ArbreVide()) {
        Detruire(A->gauche);
        Detruire(A->droit);
        free(A);
    }
}

/**
 * Crée et retourne un arbre avec un seul noeud qui recopie la donnée
 * pointée par ptr_d.
 *
 * @param ptr_d un pointeur vers une donnée.
 *
 * @return un pointeur vers l'arbre créé (ie. un pointeur vers sa
 * racine).
 */
extern Arbre *Creer0(Donnee *ptr_d) {
    return Creer2(ptr_d, ArbreVide(), ArbreVide());
}


/**
 * Crée et retourne un arbre qui l'union de deux sous-arbres plus un
 * noeud qui recopie la donnée pointée par ptr_d.
 *
 * @param ptr_d un pointeur vers une donnée.
 *
 * @param G un pointeur vers le futur sous-arbre gauche. Attention il
 * ne faut plus s'en servir après, car il est intégré à l'arbre A.
 *
 * @param D un pointeur vers le futur sous-arbre droit. Attention il
 * ne faut plus s'en servir après, car il est intégré à l'arbre A.
 *
 * @return un pointeur vers l'arbre créé (ie. un pointeur vers sa
 * racine).
 */
Arbre *Creer2(Donnee *ptr_d, Arbre *G, Arbre *D) {
    Noeud *racine = (Noeud *) malloc(sizeof(Noeud));
    CopierDonnees(ptr_d, &racine->data);
    racine->gauche = G;
    racine->droit = D;
    return racine;
}


/**
 * Retourne le noeud racine de A (éventuellement NULL si arbre vide).
 *
 * NB: Vu les définitions, retourne A directement, car l'arbre est
 * confondu avec son noeud racine.
 *
 * @param A un pointeur vers un arbre valide.
 */
extern Noeud *Racine(Arbre *A) {
    return A;
}


/**
 * @return le noeud fils gauche de N (éventuellement NULL si N n'avait
 * pas de fils gauche).
 *
 * @param N un pointeur vers un noeud valide.
 */
Noeud *Gauche(Noeud *N) {
    return N->gauche;
}

/**
 * Modifie le noeud N de façon à ce que SG devienne sous nouveau
 * sous-arbre gauche. L'ancien sous-arbre gauche de N est désalloué.
 *
 * @param N un pointeur vers un noeud valide.
 * @param SG le nouveau sous-arbre, éventuellement vide ou réduit à un noeud.
 */
void ModifieGauche(Noeud *N, Arbre *SG) {
    Detruire(N->gauche);
    N->gauche = SG;
}

/**
 * @return le noeud fils droit de N (éventuellement NULL si N n'avait
 * pas de fils droit).
 *
 * @param N un pointeur vers un noeud valide.
 */
Noeud *Droit(Noeud *N) {
    return N->droit;
}

/**
 * Modifie le noeud N de façon à ce que SD devienne sous nouveau
 * sous-arbre droit. L'ancien sous-arbre droit de N est désalloué.
 *
 * @param N un pointeur vers un noeud valide.
 * @param SD le nouveau sous-arbre, éventuellement vide ou réduit à un noeud.
 */
void ModifieDroit(Noeud *N, Arbre *SD) {
    Detruire(N->droit);
    N->droit = SD;
}

/**
 * Permet la lecture et l'écriture dans la donnée associée au noeud N.
 *
 * @return un pointeur vers la donnee du noeud N.
 *
 * @param N un pointeur vers un noeud valide.
 */
Donnee *Valeur(Noeud *N) {
    return &N->data;
}

int comp(void *d1, void *d2, void *a) {
    int axe = *((int *) a);
    Donnee *D1 = (Donnee *) d1;
    Donnee *D2 = (Donnee *) d2;

    if (D1->x[axe] < D2->x[axe]) {
        printf("-1\n");
        return -1;
    } else if (D1->x[axe] == D2->x[axe]) {
        printf("0\n");
        return 0;
    } else { // D1->x[axe] > D2->x[axe]
        printf("1\n");
        return 1;
    }
}

Arbre *KDT_Creer(Donnee *T, int i, int j, int a) {
    if (i > j)
        return NULL;

    if (i == j)
        return Creer0(&T[i]);

    qsort_r(T + i, j - i + 1, sizeof(Donnee), comp, &a);

    int m = (i + j) / 2;
    Arbre *A = Creer0(&T[m]);
    ModifieGauche(Racine(A), KDT_Creer(T, i, m - 1, (a + 1) % DIM));
    ModifieDroit(Racine(A), KDT_Creer(T, m + 1, j, (a + 1) % DIM));

    return A;
}
