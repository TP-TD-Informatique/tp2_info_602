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
