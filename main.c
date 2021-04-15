#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "points.h"
#include "particules.h"
#include "forces.h"
#include "obstacles.h"
#include "arbre.h"

//-----------------------------------------------------------------------------
// Déclaration des types
//-----------------------------------------------------------------------------
/**
   Le contexte contient les informations utiles de l'interface pour
   les algorithmes de géométrie algorithmique.
*/
typedef struct SContexte {
    int width;
    int height;
    GtkWidget *drawing_area;
    TabParticules TabP;
    TabObstacles TabO;
    Arbre *kdtree;
    Force forces[NB_FORCES];
    GtkWidget *label_nb;
    GtkWidget *label_distance;
} Contexte;

// Pas de temps en s
#define DT 0.01
// Pas de temps en s pour le réaffichage
#define DT_AFF 0.02


//-----------------------------------------------------------------------------
// Déclaration des fonctions
//-----------------------------------------------------------------------------
/**
   Crée l'interface graphique en fonction du contexte \a pCtxt.
*/
GtkWidget *creerIHM(Contexte *pCtxt);

/**
    Cette réaction est appelée à la création de la zone de dessin.
*/
gboolean realize_evt_reaction(GtkWidget *widget, gpointer data);

/**
   C'est la réaction principale qui est appelée pour redessiner la zone de dessin.
*/
gboolean expose_evt_reaction(GtkWidget *widget, GdkEventExpose *event, gpointer data);

/**
   Fait la conversion coordonnées réelles de \a p vers coordonnées de la zone de dessin.
   @param pCtxt le contexte de l'IHM
   @param p le point en entrée
   @return ses coordonnées dans la zone de dessin.
*/
Point point2DrawingAreaPoint(Contexte *pCtxt, Point p);

/**
   Fait la conversion longueur réelle \a l vers longueur en pixel dans la zone de dessin.
   @param pCtxt le contexte de l'IHM
   @param l la longueur réelle
   @return la longueur correspondante en pixels.
*/
double length2DrawingAreaLength(Contexte *pCtxt, double l);

/**
   Fait la conversion coordonnées pixel de \a p vers coordonnées réelles.
   @param pCtxt le contexte de l'IHM
   @param p les coordonnées pixel en entrée
   @return ses coordonnées réelles.
*/
Point drawingAreaPoint2Point(Contexte *pCtxt, Point p);

/**
   Affiche un point \a p dans une zone de dessin cairo \a cr comme un disque.
   La masse influe sur la taille d'affichage de la particule. 
   
   @param cr le contexte CAIRO pour dessiner dans une zone de dessin.
   @param p un point dans la zone de dessin.
 */
void drawParticule(Contexte *pCtxt, cairo_t *cr, Particule p);

/**
   Fonction de base qui affiche un disque de centre (x,y) et de rayon r via cairo.
*/
void drawPoint(cairo_t *cr, double x, double y, double r);

void viewerKDTree(Contexte *pCtxt, cairo_t *cr, Noeud *N, Point bg, Point hd, int a);

/**
   Fonction appelée régulièrement (tous les DT secondes) et qui s'occupe de (presque tout):
   - générer de nouvelles particules: \ref fontaine
   - calculer les forces sur chaque particule: \ref calculDynamique
   - déplacer les particules et gérer les collisions: \ref deplaceTout

   @param data correspond en fait au pointeur vers le Contexte.
*/
gint tic(gpointer data);

/**
   Fonction appelée régulièrement (tous les DT_AFF secondes) et qui
   s'occupe de demander le réaffichage dela zone de dessin.

   @param data correspond en fait au pointeur vers le Contexte.
*/
gint ticAffichage(gpointer data);

/**
   Fonction appelée régulièrement (tous les secondes) et qui
   affiche le nombre d'appels à la fonction \c distance par seconde.

   @param data correspond en fait au pointeur vers le Contexte.
*/
gint ticDistance(gpointer data);

/**
   Calcul la dynamique de tous les points en appliquant les forces et
   met à jour la vitesse.
*/
void calculDynamique(Contexte *pCtxt);

/**
   Déplace toutes les particules en fonction de leur vitesse.
*/
void deplaceTout(Contexte *pCtxt);

/**
   Déplace une particule en fonction de sa vitesse. Devra s'occuper
   des collisions plus tard.
*/
void deplaceParticule(Contexte *pCtxt, Particule *p);

/**
  Fontaine pour créer une particule à la position (\a x, \a y), avec
  la vitesse (\a vx, \a vy) et la masse \a m.

  @param p probabilité (entre 0 et 1) qu'une particule soit effectivement créée.
  @param x la coordonnée x de la position où la particule est créée.
  @param y la coordonnée y de la position où la particule est créée.
  @param vx la composante x de la vitesse de la particule.
  @param vy la composante y de la vitesse de la particule.
  @param m la masse de la particule créée.
 */
void fontaine(Contexte *pCtxt, double p,
              double x, double y, double vx, double vy, double m);

void fontaineVariable(Contexte *pCtxt,
                      double p, double var,
                      double x, double y, double vx, double vy, double m);

gboolean mouse_clic_reaction(GtkWidget *widget, GdkEventButton *event, gpointer data);


//-----------------------------------------------------------------------------
// Programme principal
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[]) {
    Contexte context;
    TabParticules_init(&context.TabP);
    TabObstacles_init(&context.TabO);
    context.kdtree = ArbreVide();

    /* Passe les arguments à GTK, pour qu'il extrait ceux qui le concernent. */
    gtk_init(&argc, &argv);

    /* Crée une fenêtre. */
    creerIHM(&context);

    /* Rentre dans la boucle d'événements. */
    gtk_main();
    return 0;
}

gboolean realize_evt_reaction(GtkWidget *widget, gpointer data) { // force un événement "expose" juste derrière.
    gtk_widget_queue_draw(widget);
    return TRUE;
}

gboolean expose_evt_reaction(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    // c'est la réaction principale qui va redessiner tout.
    Contexte *pCtxt = (Contexte *) data;
    TabParticules *ptrP = &(pCtxt->TabP);
    TabObstacles *ptrO = &(pCtxt->TabO);
    // c'est la structure qui permet d'afficher dans une zone de dessin
    // via Cairo
    cairo_t *cr = gdk_cairo_create(widget->window);
    cairo_set_source_rgb(cr, 1, 1, 1); // choisit le blanc.
    cairo_paint(cr); // remplit tout dans la couleur choisie.

    // Affiche tous les points en bleu.
    cairo_set_source_rgb(cr, 0, 0, 1);
    for (int i = 0; i < TabParticules_nb(ptrP); ++i) {
        drawParticule(pCtxt, cr, TabParticules_get(ptrP, i));
    }

    // Affiche tous les obstacle
    for (int i = 0; i < TabObstacles_nb(ptrO); ++i) {
        Obstacle o = TabObstacles_get(ptrO, i);
        cairo_set_source_rgb(cr, o.cr, o.cg, o.cb);
        Point p;
        p.x[0] = o.x[0];
        p.x[1] = o.x[1];
        p = point2DrawingAreaPoint(pCtxt, p);
        drawPoint(cr, p.x[0], p.x[1], 10);
    }

    Point bg = {{-10.0, -10.0}};
    Point hd = {{10.0, 10.0}};
    viewerKDTree(pCtxt, cr, Racine(pCtxt->kdtree), bg, hd, 0);

    // On a fini, on peut détruire la structure.
    cairo_destroy(cr);
    return TRUE;
}

Point point2DrawingAreaPoint(Contexte *pCtxt, Point p) {
    Point q;
    q.x[0] = (p.x[0] + 1.0) / 2.0 * pCtxt->width;
    q.x[1] = (1.0 - p.x[1]) / 2.0 * pCtxt->height;
    return q;
}

double length2DrawingAreaLength(Contexte *pCtxt, double l) {
    return pCtxt->width * l / 2.0;
}

Point drawingAreaPoint2Point(Contexte *pCtxt, Point p) {
    Point q;
    q.x[0] = 2.0 * ((double) p.x[0] / (double) pCtxt->width) - 1.0;
    q.x[1] = -2.0 * ((double) p.x[1] / (double) pCtxt->height) + 1.0;
    return q;
}

void drawParticule(Contexte *pCtxt, cairo_t *cr, Particule p) {
    Point pp;
    pp.x[0] = p.x[0];
    pp.x[1] = p.x[1];
    // On convertit les coordonnées réelles des particules (dans [-1:1]x[-1:1]) en coordonnées
    // de la zone de dessin (dans [0:499]x[0:499]).
    Point q = point2DrawingAreaPoint(pCtxt, pp);
    drawPoint(cr, q.x[0], q.x[1], 1.5 * sqrt(p.m));
}

void drawPoint(cairo_t *cr, double x, double y, double r) {
    cairo_arc(cr, x, y, r, 0.0, 2.0 * 3.14159626);
    cairo_fill(cr);
}

void viewerKDTree(Contexte *pCtxt, cairo_t *cr, Noeud *N, Point bg, Point hd, int a) {
    if (N != ArbreVide()) {
        int b = (a + 1) % DIM;
        Obstacle *q = Valeur(N);
        Point p, p1, p2;
        p1.x[a] = q->x[a];
        p1.x[b] = bg.x[b];
        p2.x[a] = q->x[a];
        p2.x[b] = hd.x[b];
        cairo_set_source_rgb(cr, 0.0, 1.0, 1.0);
        Point draw_p1 = point2DrawingAreaPoint(pCtxt, p1);
        Point draw_p2 = point2DrawingAreaPoint(pCtxt, p2);
        cairo_move_to(cr, draw_p1.x[0], draw_p1.x[1]);
        cairo_line_to(cr, draw_p2.x[0], draw_p2.x[1]);
        cairo_stroke(cr);
        cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
        p.x[0] = q->x[0];
        p.x[1] = q->x[1];
        Point dp = point2DrawingAreaPoint(pCtxt, p);
        drawPoint(cr, dp.x[0], dp.x[1], 3);
        Point hd2 = hd;
        hd2.x[a] = q->x[a];
        Point bg2 = bg;
        bg2.x[a] = q->x[a];
        viewerKDTree(pCtxt, cr, Gauche(N), bg, hd2, b);
        viewerKDTree(pCtxt, cr, Droit(N), bg2, hd, b);
    }
}


void drawLine(cairo_t *cr, Point p, Point q) {
    cairo_move_to(cr, p.x[0], p.x[1]);
    cairo_line_to(cr, q.x[0], q.x[1]);
    cairo_stroke(cr);
}

/// Charge l'image donnée et crée l'interface.
GtkWidget *creerIHM(Contexte *pCtxt) {
    GtkWidget *window;
    GtkWidget *vbox1;
    GtkWidget *vbox2;
    GtkWidget *hbox1;
    GtkWidget *button_quit;

    /* Crée une fenêtre. */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // Crée un conteneur horitzontal box.
    hbox1 = gtk_hbox_new(FALSE, 10);
    // Crée deux conteneurs vertical box.
    vbox1 = gtk_vbox_new(FALSE, 10);
    vbox2 = gtk_vbox_new(FALSE, 10);
    // Crée une zone de dessin
    pCtxt->drawing_area = gtk_drawing_area_new();
    pCtxt->width = 500;
    pCtxt->height = 500;
    gtk_widget_set_size_request(pCtxt->drawing_area, pCtxt->width, pCtxt->height);
    // Crée le pixbuf source et le pixbuf destination
    gtk_container_add(GTK_CONTAINER(hbox1), pCtxt->drawing_area);
    // ... votre zone de dessin s'appelle ici "drawing_area"
    g_signal_connect(G_OBJECT(pCtxt->drawing_area), "realize",
                     G_CALLBACK(realize_evt_reaction), pCtxt);
    g_signal_connect(G_OBJECT(pCtxt->drawing_area), "expose_event",
                     G_CALLBACK(expose_evt_reaction), pCtxt);
    // Rajoute le 2eme vbox dans le conteneur hbox (pour mettre les boutons sélecteur de points
    gtk_container_add(GTK_CONTAINER(hbox1), vbox2);
    // Crée les labels pour afficher le nombre de points et le nombre
    // d'appel à la fonction distance.
    pCtxt->label_nb = gtk_label_new("0 points");
    gtk_container_add(GTK_CONTAINER(vbox2), pCtxt->label_nb);
    pCtxt->label_distance = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(vbox2), pCtxt->label_distance);

    // Crée le bouton quitter.
    button_quit = gtk_button_new_with_label("Quitter");
    // Connecte la réaction gtk_main_quit à l'événement "clic" sur ce bouton.
    g_signal_connect(button_quit, "clicked",
                     G_CALLBACK(gtk_main_quit),
                     NULL);
    // Rajoute tout dans le conteneur vbox.
    gtk_container_add(GTK_CONTAINER(vbox1), hbox1);
    gtk_container_add(GTK_CONTAINER(vbox1), button_quit);
    // Rajoute la vbox  dans le conteneur window.
    gtk_container_add(GTK_CONTAINER(window), vbox1);

    // Connecte la réaction pour le clic gauche
    g_signal_connect(G_OBJECT(pCtxt->drawing_area), "button_press_event",
                     G_CALLBACK(mouse_clic_reaction), pCtxt);
    gtk_widget_set_events(pCtxt->drawing_area, GDK_EXPOSURE_MASK
                                               | GDK_LEAVE_NOTIFY_MASK
                                               | GDK_BUTTON_PRESS_MASK
                                               | GDK_POINTER_MOTION_MASK
                                               | GDK_POINTER_MOTION_HINT_MASK);

    // Rend tout visible
    gtk_widget_show_all(window);
    g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Crée les forces
    Force g = gravite(0.0, -0.2);
    pCtxt->forces[0] = g;

    // enclenche le timer pour se déclencher dans 5ms.
    g_timeout_add(1000 * DT, tic, (gpointer) pCtxt);
    // enclenche le timer pour se déclencher dans 20ms.
    g_timeout_add(1000 * DT_AFF, ticAffichage, (gpointer) pCtxt);
    // enclenche le timer pour se déclencher dans 1000ms.
    g_timeout_add(1000, ticDistance, (gpointer) pCtxt);

    return window;
}

void fontaine(Contexte *pCtxt,
              double p, double x, double y, double vx, double vy, double m) {
    TabParticules *P = &pCtxt->TabP;
    if ((rand() / (double) RAND_MAX) < p) {
        Particule q;
        initParticule(&q, x, y, vx, vy, m);
        TabParticules_ajoute(P, q);
    }
}

void fontaineVariable(Contexte *pCtxt,
                      double p, double var,
                      double x, double y, double vx, double vy, double m) {
    TabParticules *P = &pCtxt->TabP;
    if ((rand() / (double) RAND_MAX) < p) {
        Particule q;
        double v1 = (rand() / (double) RAND_MAX) * var;
        double v2 = (rand() / (double) RAND_MAX) * var;
        initParticule(&q, x, y, vx - v1, vy + v2, m);
        TabParticules_ajoute(P, q);
    }
}

gint tic(gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
//    fontaine(pCtxt, 0.25, -0.5, 0.5, 0.3, 0.3, 2.5);
    fontaineVariable(pCtxt, 0.2, 0.1, -0.5, 0.5, 0.3, 0.3, 1.0);
    fontaineVariable(pCtxt, 0.25, 0.2, -0.5, 0.5, 0.3, 0.3, 0.5);
    fontaineVariable(pCtxt, 0.6, 0.18, -0.5, 0.5, 0.3, 0.3, 2.5);
    calculDynamique(pCtxt);
    deplaceTout(pCtxt);
    g_timeout_add(1000 * DT, tic, (gpointer) pCtxt); // réenclenche le timer.
    return 0;
}

gint ticAffichage(gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
    char buffer[128];
    sprintf(buffer, "%d points", TabParticules_nb(&pCtxt->TabP));
    gtk_label_set_text(GTK_LABEL(pCtxt->label_nb), buffer);
    gtk_widget_queue_draw(pCtxt->drawing_area);
    g_timeout_add(1000 * DT_AFF, ticAffichage, (gpointer) pCtxt); // réenclenche le timer.
    return 0;
}

gint ticDistance(gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
    char buffer[128];
    sprintf(buffer, "%7d nb appels à distance()", getCompteurDistance()),
            gtk_label_set_text(GTK_LABEL(pCtxt->label_distance), buffer);
    resetCompteurDistance();
    g_timeout_add(1000, ticDistance, (gpointer) pCtxt); // réenclenche le timer.
    return 0;
}

void calculDynamique(Contexte *pCtxt) {
    TabParticules *P = &pCtxt->TabP;
    int n = TabParticules_nb(P);
    Force *F = pCtxt->forces;
    // On met à zéro les forces de chaque point.
    for (int i = 0; i < n; ++i) {
        Particule *p = TabParticules_ref(P, i);
        p->f[0] = 0.0;
        p->f[1] = 0.0;
    }
    // On applique les forces à tous les points
    for (int i = 0; i < n; ++i) {
        Particule *p = TabParticules_ref(P, i);
        for (int j = 0; j < NB_FORCES; ++j)
            appliqueForce(p, &F[j]);
    }
    // On applique la loi de Newton: masse*acceleration = somme des forces
    // ie m dv/dt = sum f
    // ie v[t+dt] = v[t] + (dt/m) * sum f
    for (int i = 0; i < n; ++i) {
        Particule *p = TabParticules_ref(P, i);
        p->v[0] += (DT / p->m) * p->f[0];
        p->v[1] += (DT / p->m) * p->f[1];
    }
}

// Cette fonction n'est appelé que si la particule p déplacée (p.x +
// DT*p.v) est à l'intérieur du disque B_r(center).  Elle retourne
// alors le rebond de la particule calculé pour cet obstacle circulaire
// (nouveau x, nouveau v) en fonction de l'atténuation choisie.  En
// sortie, la particule est en dehors de l'obstacle.
Particule calculRebond(Particule p, Point center, double r, double att) {
    Point xd, v;
    // Calcule la nouvelle position xd (sans collision) et le vecteur vitesse.
    xd.x[0] = p.x[0] + DT * p.v[0];
    xd.x[1] = p.x[1] + DT * p.v[1];
    v.x[0] = p.v[0];
    v.x[1] = p.v[1];
    Point u = Point_normalize(Point_sub(xd, center));
    double l = Point_norm(Point_sub(xd, center));
    Point xm = Point_add(center, Point_mul(r + att * (r - l), u));
    double proj_v = Point_dot(v, u);
    // réalise le rebond si la particule est bien en train de rentrer dans l'obstacle.
    if (proj_v < 0.0)
        v = Point_sub(v, Point_mul(2.0 * proj_v, u));
    Particule p_out = p;
    p_out.x[0] = xm.x[0];
    p_out.x[1] = xm.x[1];
    p_out.v[0] = att * v.x[0];
    p_out.v[1] = att * v.x[1];
    return p_out;
}


void deplaceParticule(Contexte *pCtxt, Particule *p) {
    // Déplace p en supposant qu'il n'y a pas de collision.
    Point pp;
    pp.x[0] = p->x[0] + DT * p->v[0];
    pp.x[1] = p->x[1] + DT * p->v[1];

    TabObstacles F; // obstacles potentiels;
    TabObstacles_init(&F);
    KDT_PointsDansBoule(&F, Racine(pCtxt->kdtree), &pp, 0.05, 0);

    bool collision = false;
    int i = 0;
    while (!collision && i < TabObstacles_nb(&F)) {
        Obstacle obs = TabObstacles_get(&F, i);
        if (distance(p->x[0], p->x[1], obs.x[0], obs.x[1]) <= obs.r) {
            collision = true;
            Point point;
            point.x[0] = obs.x[0];
            point.x[1] = obs.x[1];
            *p = calculRebond(*p, point, obs.r, obs.att);
        }

        i++;
    }

    if (!collision) {
        p->x[0] += DT * p->v[0];
        p->x[1] += DT * p->v[1];
    }

    TabObstacles_termine(&F); // pour éviter les fuites mémoire.
}

void deplaceTout(Contexte *pCtxt) {
    TabParticules *P = &pCtxt->TabP;
    int n = TabParticules_nb(P);
    // Applique le vecteur vitesse sur toutes les particules.
    for (int i = 0; i < n; ++i) {
        Particule *p = TabParticules_ref(P, i);
        deplaceParticule(pCtxt, p);
    }
    // Détruit les particules trop loin de la zone
    for (int i = 0; i < TabParticules_nb(P);) {
        Particule *p = TabParticules_ref(P, i);
        if ((p->x[0] < -1.5) || (p->x[0] > 1.5)
            || (p->x[1] < -1.5) || (p->x[1] > 1.5))
            TabParticules_supprime(P, i);
        else ++i;
    }
}

gboolean mouse_clic_reaction(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
    int button = event->button; // 1 is left button
    if (button != 1) return TRUE;
    int x = event->x;
    int y = event->y;

    Obstacle o;
    Point p;
    p.x[0] = x;
    p.x[1] = y;
    p = drawingAreaPoint2Point(pCtxt, p);
    initObstacle(&o, DISQUE, p.x[0], p.x[1], 0.05, 0.6, 0, 0, 0);
    TabObstacles_ajoute(&pCtxt->TabO, o);
    Detruire(pCtxt->kdtree);
    pCtxt->kdtree = KDT_Creer(pCtxt->TabO.obstacles, 0, TabObstacles_nb(&pCtxt->TabO) - 1, 0);

    return TRUE;
}
