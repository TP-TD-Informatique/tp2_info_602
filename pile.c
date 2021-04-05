#include "pile.h"

void PilePoints_init(PilePoints *pile) {
    pile->taille = 100;
    pile->nb = 0;
    pile->points = (Point *) malloc(pile->taille * sizeof(Point));
}

int PilePoints_estVide(PilePoints *pile) {
    return pile->nb == 0;
}

void PilePoints_empile(PilePoints *pile, Point p) {
    if (pile->nb == pile->taille)
        PilePoints_agrandir(pile);

    pile->points[pile->nb++] = p;
}

void PilePoints_depile(PilePoints *pile) {
    if (pile->nb > 0)
        pile->nb--;
}

Point PilePoints_sommet(PilePoints *pile) {
    assert(pile->nb > 0);
    return pile->points[pile->nb];
}

Point PilePoints_deuxiemeSommet(PilePoints *pile) {
    assert(pile->nb > 1);
    return pile->points[pile->nb - 1];
}

void PilePoints_agrandir(PilePoints *pile) {
    pile->taille += 100;
    pile->points = realloc(pile->points, sizeof(Point) * pile->taille);
}

void PilePoints_termine(PilePoints *pile) {
    if (pile->points != NULL)
        free(pile->points);
    pile->taille = 0;
    pile->nb = 0;
    pile->points = NULL;
}

int PilePoints_nb(PilePoints *pile) {
    return pile->nb;
}
