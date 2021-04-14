#ifndef _FORCES_H_
#define _FORCES_H_

#include "particules.h"

#define NB_FORCES 1
/// Un seul type de force pour le moment.
typedef enum {
    GRAVITE
} ForceType;

/// Une force est un type et des paramètres qui la définissent.
struct SForce {
    ForceType type;
    double params[2];
};
typedef struct SForce Force;

/// Définit la force de gravité dans la direction donnée.
Force gravite(double gx, double gy);

/// Ajoute à la particule \a p la force donnée \a f
void appliqueForce(Particule *p, Force *f);

#endif
