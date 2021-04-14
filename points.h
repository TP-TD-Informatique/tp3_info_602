#ifndef _POINTS_H_
#define _POINTS_H_

// On est dans le plan, il faut deux coordonnées. La dimension est donc 2.
#define DIM 2

/// Un point est une structure contenant un tableau de DIM double, ses
/// coordonnées.
typedef struct SPoint {
    double x[DIM];
} Point;

/// @return le vecteur p-q
Point Point_sub(Point p, Point q);

/// @return le vecteur p+q
Point Point_add(Point p, Point q);

/// @return le vecteur c.p (multiplication scalaire d'un vecteur)
Point Point_mul(double c, Point p);

/// @return le produit scalaire p * q
double Point_dot(Point p, Point q);

/// @return la norme au carré du vecteur p.
double Point_norm2(Point p);

/// @return la norme du vecteur p.
double Point_norm(Point p);

/// @return la distance entre les deux points p et q, qui est aussi la norme de p-q.
double Point_distance(Point p, Point q);

/// @return le vecteur de norme 1 aligné avec le vecteur p.
Point Point_normalize(Point p);

/// @return la distance entre (x1,y1) et (x2,y2).
double distance(double x1, double y1, double x2, double y2);

/// Remet à zéro le compteur du nombre d'appel à distance.
void resetCompteurDistance();

/// @return la valeur du compteur du nombre d'appel à distance.
int getCompteurDistance();

#endif
