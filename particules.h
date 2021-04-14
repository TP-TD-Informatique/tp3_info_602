#ifndef _PARTICULES_H_
#define _PARTICULES_H_

#include "points.h"

/// Représente un point / une particule en mouvement, avec position,
/// mais aussi vitesse, force, et masse.
typedef struct SParticule {
    double x[DIM];  //< position x et y
    double v[DIM];  //< vitesse x et y
    double f[DIM];  //< somme des forces x et y
    double m;       //< masse
} Particule;

/**
   Initialise le point \a p avec la position (x,y), la vitesse
   (vx,vy), une masse \a m et des forces nulles.x  
*/
void initParticule(Particule *p, double x, double y, double vx, double vy, double m);


/// Représente un tableau dynamique de particules.
typedef struct STabParticule {
    int taille;
    int nb;
    Particule *particules;
} TabParticules;

/**
   Initialise le tableau de particules \a tab. Il contient 0 particules initialement, mais
   peut accueillir jusqu'à 10 particules sans être agrandi.

   @param tab un pointeur vers une structure TabParticule.
*/
void TabParticules_init(TabParticules *tab);

/**
   Ajoute si possible le particule \a p à la fin du tableau de particules \a tab.
   
   @param tab  un pointeur vers une structure TabParticule valide.
   @param p une particule.
*/
void TabParticules_ajoute(TabParticules *tab, Particule p);

/**
   Modifie le \a i-ème point du tableau de points \a tab. Il devient
   le point \a p.
   
   @param tab  un pointeur vers une structure TabParticule valide.
   @param i un index valide (entre 0 et TabParticules_nb( tab ) )
   @param p une particule.
*/
void TabParticules_set(TabParticules *tab, int i, Particule p);

/**
   @param tab  un pointeur vers une structure TabParticule valide.
   @param i un index valide (entre 0 et TabParticules_nb( tab ) )
   @return le \a i-ème point du tableau de points \a tab.   
*/
Particule TabParticules_get(TabParticules *tab, int i);

/**
   @param tab  un pointeur vers une structure TabParticule valide.
   @param i un index valide (entre 0 et TabParticules_nb( tab ) )
   @return un pointeur vers le \a i-ème point du tableau de points \a tab.   
*/
Particule *TabParticules_ref(TabParticules *tab, int i);

/**
   @param tab  un pointeur vers une structure TabParticule valide.
   @return le nombre de points utiles stockés dans le tableau de points \a tab.   
*/
int TabParticules_nb(TabParticules *tab);

/**
   Indique que le tableau de points \a tab n'est plus utilisé et
   libère la mémoire associée. Il passe à une taille 0.
  
   @param tab  un pointeur vers une structure TabParticule valide.
 */
void TabParticules_termine(TabParticules *tab);

/**
   Utilisé en interne. Agrandit automatiquement le tableau si nécessaire.
*/
void TabParticules_agrandir(TabParticules *tab);

/**
   Supprime le dernier élément du tableau.
*/
void TabParticules_supprime_dernier(TabParticules *tab);

/**
   Supprime un élément en position \a i du tableau. Met le dernier
   élément du tableau à la place.
  */
void TabParticules_supprime(TabParticules *tab, int i);

#endif
