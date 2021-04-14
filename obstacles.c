#include "obstacles.h"


void initObstacle(Obstacle *o, ObstacleType type, double x, double y, double rayon, double att,
                  double cr, double cg, double cb) {
    o->type = type;
    o->x[0] = x;
    o->x[1] = y;
    o->r = rayon;
    o->att = att;
    o->cr = cr;
    o->cg = cg;
    o->cb = cb;
}

void TabObstacles_init(TabObstacles *tab) {
    tab->taille = 10;
    tab->nb = 0;
    tab->obstacles = (Obstacle *) malloc(tab->taille * sizeof(Obstacle));
}

void TabObstacles_ajoute(TabObstacles *tab, Obstacle p) {
    if (tab->nb == tab->taille)
        TabObstacles_agrandir(tab);
    tab->obstacles[tab->nb++] = p;
}

void TabObstacles_set(TabObstacles *tab, int i, Obstacle p) {
    assert (i < tab->nb);
    tab->obstacles[i] = p;
}

Obstacle TabObstacles_get(TabObstacles *tab, int i) {
    assert (i < tab->nb);
    return tab->obstacles[i];
}

int TabObstacles_nb(TabObstacles *tab) {
    return tab->nb;

}

void TabObstacles_termine(TabObstacles *tab) {
    if (tab->obstacles != NULL) free(tab->obstacles);
    tab->taille = 0;
    tab->nb = 0;
    tab->obstacles = NULL;
}

Obstacle *TabObstacles_ref(TabObstacles *tab, int i) {
    assert (i < tab->nb);
    return tab->obstacles + i;
}

void TabObstacles_agrandir(TabObstacles *tab) {
    int new_taille = 2 * tab->taille;
    Obstacle *new_pts = (Obstacle *) malloc(new_taille * sizeof(Obstacle));
    for (int i = 0; i < tab->nb; ++i)
        new_pts[i] = tab->obstacles[i];
    free(tab->obstacles);
    tab->obstacles = new_pts;
    tab->taille = new_taille;
}