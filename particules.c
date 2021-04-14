#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "particules.h"

void initParticule(Particule *p, double x, double y, double vx, double vy,
                   double m) {
    p->x[0] = x;
    p->x[1] = y;
    p->v[0] = vx;
    p->v[1] = vy;
    p->f[0] = 0.0;
    p->f[1] = 0.0;
    p->m = m;
}

void TabParticules_init(TabParticules *tab) {
    tab->taille = 10;
    tab->nb = 0;
    tab->particules = (Particule *) malloc(tab->taille * sizeof(Particule));
}

void TabParticules_ajoute(TabParticules *tab, Particule p) {
    if (tab->nb == tab->taille)
        TabParticules_agrandir(tab);
    tab->particules[tab->nb++] = p;
}

void TabParticules_set(TabParticules *tab, int i, Particule p) {
    assert (i < tab->nb);
    tab->particules[i] = p;
}

Particule TabParticules_get(TabParticules *tab, int i) {
    assert (i < tab->nb);
    return tab->particules[i];
}

Particule *TabParticules_ref(TabParticules *tab, int i) {
    assert (i < tab->nb);
    return tab->particules + i;
}

int TabParticules_nb(TabParticules *tab) {
    return tab->nb;
}

void TabParticules_termine(TabParticules *tab) {
    if (tab->particules != NULL) free(tab->particules);
    tab->taille = 0;
    tab->nb = 0;
    tab->particules = NULL;
}

void TabParticules_agrandir(TabParticules *tab) {
    int new_taille = 2 * tab->taille;
    Particule *new_pts = (Particule *) malloc(new_taille * sizeof(Particule));
    for (int i = 0; i < tab->nb; ++i)
        new_pts[i] = tab->particules[i];
    free(tab->particules);
    tab->particules = new_pts;
    tab->taille = new_taille;
}

void TabParticules_supprime_dernier(TabParticules *tab) {
    assert(tab->nb > 0);
    --tab->nb;
}

void TabParticules_supprime(TabParticules *tab, int i) {
    assert (i >= 0);
    assert (i < tab->nb);
    tab->particules[i] = tab->particules[--tab->nb];
}


