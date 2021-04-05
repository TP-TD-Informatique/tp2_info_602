#include <assert.h>
#include <stdlib.h>
#include "points.h"

void TabPoints_init(TabPoints *tab) {
    tab->taille = 100;
    tab->nb = 0;
    tab->points = (Point *) malloc(tab->taille * sizeof(Point));
}

void TabPoints_ajoute(TabPoints *tab, Point p) {
    if (tab->nb == tab->taille)
        TabPoints_agrandir(tab);

    tab->points[tab->nb++] = p;
}

void TabPoints_set(TabPoints *tab, int i, Point p) {
    assert (i < tab->nb);
    tab->points[i] = p;
}

Point TabPoints_get(TabPoints *tab, int i) {
    assert (i < tab->nb);
    return tab->points[i];
}

int TabPoints_nb(TabPoints *tab) {
    return tab->nb;
}

void TabPoints_termine(TabPoints *tab) {
    if (tab->points != NULL) free(tab->points);
    tab->taille = 0;
    tab->nb = 0;
    tab->points = NULL;
}

void TabPoints_agrandir(TabPoints *tab) {
    tab->taille += 100;
    tab->points = realloc(tab->points, sizeof(Point) * tab->taille);
}

int TabPoints_indexBasGauche(TabPoints *tab) {
    int res = tab->nb > 0 ? 0 : -1;

    for (int i = 1; i < tab->nb; ++i) {
        Point m = TabPoints_get(tab, res); // Point minimum
        Point a = TabPoints_get(tab, i); // Point actuel

        // Si a est sous m ou si a est au même niveau que m et plus à gauche
        if (a.y > m.y || (a.y == m.y && a.x < m.x)) {
            res = i;
        }
    }

    return res;
}

/**
 * Calcule l'angle polaire de b par rapport à a
 * @param a un Point
 * @param b un autre Point
 * @return l'angle entre les 2 points
 */
double anglePolaire(Point *a, Point *b) {
    double longueur = b->x - a->x;
    double hauteur = b->y - a->y;

    double adj = longueur;
    double hyp = sqrt((longueur * longueur) + (hauteur * hauteur));

    return acos(adj / hyp);
}

/**
 * Renvoie 1 si a a un plus grand angle que b par rapport à r
 * Renvoie 0 si ils ont le même angle
 * Renvoie -1 si a a un angle plus petit que b par rapport à r
 * @param a un Point
 * @param b un autre Point
 * @param r le point de référence
 * @return -1|0|1
 */
int comp(const void *a, const void *b, const void *r) {
    Point *pa = (Point *) a;
    Point *pb = (Point *) b;
    Point *pr = (Point *) r;

    double angleA = anglePolaire(pr, pa);
    double angleB = anglePolaire(pr, pb);

    if (angleA == angleB) {
        return 0;
    } else if (angleA < angleB) {
        return -1;
    } else {
        return 1;
    }
}

void TabPoints_triSelonT0(TabPoints *tab) {
    qsort_r(tab->points, tab->nb, sizeof(Point), comp, tab->points[0]);
}

void TabPoints_echange(TabPoints *tab, int i, int j) {
    Point t = tab->points[i];
    tab->points[i] = tab->points[j];
    tab->points[j] = t;
}

int estAGauche(Point a, Point b, Point r) {
    return comp(&a, &b, &r) > 0;
}
