#ifndef _ARBRE_H_
#define _ARBRE_H_

#include "obstacles.h"
#include "stdio.h"


/* A vous de changer cela. */
typedef Obstacle Donnee;

/**
 * Copie une donnée dans une autre. Cette fonction est utile lorsque
 * le type Donnee est complexe et que l'opérateur d'affectation du C
 * ne fonctionne pas bien.
 *
 * @param d1 un pointeur vers la première donnée à recopier.  
 *
 * @param d2 un pointeur vers la deuxième donnée, qui sera écrasée et
 * prendra les valeurs de d1.
 */
extern void CopierDonnees(Donnee *d1, Donnee *d2);


/*****************************************************************************/
/* Les arbres */
/*****************************************************************************/

/**
 * Un noeud dans un arbre binaire contient une donnée, et
 * éventuellement un pointeur vers un fils gauche et/ou vers un fils
 * droit.  On pourrait aussi stocker le noeud père ici, mais on ne
 * s'en servira pas dans les algorithmes développés.
 */
typedef struct SNoeud {
    Donnee data;
    struct SNoeud *gauche;
    struct SNoeud *droit;
} Noeud;


/**
 * Un arbre binaire est un noeud appelé racine.  On utilisera le type
 * Arbre lorsqu'on désignera le noeud racine et le type Noeud
 * lorsqu'on désigne un noeud quelconque.
 */
typedef Noeud Arbre;


/**
 * @return l'arbre vide.
 */
extern Arbre *ArbreVide();

/**
 * Détruit un arbre et désalloue tous ses éléments alloués.
 *
 * @param A un pointeur vers un arbre valide.
 */
extern void Detruire(Arbre *A);

/**
 * Crée et retourne un arbre avec un seul noeud qui recopie la donnée
 * pointée par ptr_d.
 *
 * @param ptr_d un pointeur vers une donnée.
 *
 * @return un pointeur vers l'arbre créé (ie. un pointeur vers sa
 * racine).
 */
extern Arbre *Creer0(Donnee *ptr_d);

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
extern Arbre *Creer2(Donnee *ptr_d, Arbre *G, Arbre *D);

/**
 * Retourne le noeud racine de A (éventuellement NULL si arbre vide).
 *
 * @param A un pointeur vers un arbre valide.
 */
extern Noeud *Racine(Arbre *A);

/**
 * @return le noeud fils gauche de N (éventuellement NULL si N n'avait
 * pas de fils gauche).
 *
 * @param N un pointeur vers un noeud valide.
 */
extern Noeud *Gauche(Noeud *N);

/**
 * Modifie le noeud N de façon à ce que SG devienne sous nouveau
 * sous-arbre gauche. L'ancien sous-arbre gauche de N est désalloué.
 *
 * @param N un pointeur vers un noeud valide.
 * @param SG le nouveau sous-arbre, éventuellement vide ou réduit à un noeud.
 */
extern void ModifieGauche(Noeud *N, Arbre *SG);

/**
 * @return le noeud fils droit de N (éventuellement NULL si N n'avait
 * pas de fils droit).
 *
 * @param N un pointeur vers un noeud valide.
 */
extern Noeud *Droit(Noeud *N);

/**
 * Modifie le noeud N de façon à ce que SD devienne sous nouveau
 * sous-arbre droit. L'ancien sous-arbre droit de N est désalloué.
 *
 * @param N un pointeur vers un noeud valide.
 * @param SD le nouveau sous-arbre, éventuellement vide ou réduit à un noeud.
 */
extern void ModifieDroit(Noeud *N, Arbre *SD);


/**
 * Permet la lecture et l'écriture dans la donnée associée au noeud N.
 *
 * @return un pointeur vers la donnee du noeud N.
 *
 * @param N un pointeur vers un noeud valide.
 */
extern Donnee *Valeur(Noeud *N);


// Si T est un Obstacle* pointant vers la première case d'un tableau
// d'Obstacle, i < j désignent les indices de début et de fin dans le
// tableau T, a est l'axe (0 ou 1) utilisé pour découper le plan.
// Alors cette fonction crée et retourne l'arbre binaire (arbre k-D)
// stockant tous les obstacles spécifiés.
Arbre *KDT_Creer(Donnee *T, int i, int j, int a);

// Ajoute dans le tableau d'obstacles F les obstacles de l'arbre
// désigné par le noeud racine N qui sont à une distance inférieure à
// r du point p. Le paramètre a désigne l'axe courant (0 ou 1) et
// change à chaque niveau de récursion.
void KDT_PointsDansBoule(TabObstacles *F, Noeud *N, Point *p, double r, int a);


#endif
