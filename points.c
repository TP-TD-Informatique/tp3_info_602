#include <math.h>
#include "points.h"


Point Point_sub(Point p, Point q) {
    p.x[0] -= q.x[0];
    p.x[1] -= q.x[1];
    return p;
}

Point Point_add(Point p, Point q) {
    p.x[0] += q.x[0];
    p.x[1] += q.x[1];
    return p;
}

Point Point_mul(double c, Point p) {
    p.x[0] *= c;
    p.x[1] *= c;
    return p;
}

double Point_dot(Point p, Point q) {
    return p.x[0] * q.x[0] + p.x[1] * q.x[1];
}

double Point_norm2(Point p) {
    return Point_dot(p, p);
}

double Point_norm(Point p) {
    return sqrt(Point_norm2(p));
}

double Point_distance(Point p, Point q) {
    return distance(p.x[0], p.x[1], q.x[0], q.x[1]);
}

Point Point_normalize(Point p) {
    return Point_mul(1.0 / Point_norm(p), p);
}

static int compteur_distance = 0;

double distance(double x1, double y1, double x2, double y2) {
    ++compteur_distance;
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void resetCompteurDistance() {
    compteur_distance = 0;
}

int getCompteurDistance() {
    return compteur_distance;
}
