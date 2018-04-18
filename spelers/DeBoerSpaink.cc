#include <climits>
#include <cstdlib>
#include <vector>

using namespace std;

// https://www.researchgate.net/publication/221932254_New_Trends_in_Clobber_Programming

bool inBordMatrix(int i, int j, Clobber* spel) {
    return i > 0 && j > 0 && i < spel->hoogte && j < spel->breedte;
}

struct Coordinaat {
    Coordinaat(int i, int j) : i(i), j(j) {};
    int i;
    int j;
};

class Blok {
private:
    vector<Coordinaat> coordinaten;
    Clobber* spel;
    bool allemaalDezelfde = true;

public:
    Blok(Clobber* spel) : spel(spel) {};

    void vindRest(bool bezocht[MAX_BORD][MAX_BORD], int i, int j) {
        int vorigeKleur = spel->bord[i - 1][j - 1];
        for (int k = -1; k < 2; ++k)
            for (int l = -1; l < 2; ++l) {
                int x = k + i;
                int y = l + j;
                if (inBordMatrix(x, y, spel) && (i == 0 || j == 0) && !bezocht[x][y] &&
                    spel->bord[x][y] != LEEG_VAKJE) {
                    if (vorigeKleur != spel->bord[x][y])
                        allemaalDezelfde = false;
                    bezocht[x][y] = true;
                    coordinaten.emplace_back(Coordinaat(x, y));
                    vindRest(bezocht, x, y);
                }
            }
    };

    int scoreBlok(int speler) {
        if (allemaalDezelfde)
            return 0;

        int ander[4][2] = {{0,  -1},
                           {0,  1},
                           {-1, 0},
                           {1,  0}};
        int zelf[4][2] = {{-1, -1},
                          {-1, 1},
                          {1,  -1},
                          {1,  1}};
        int score = 1;
        for (Coordinaat& coordinaat : coordinaten) {
            for (int i = 0; i < 4; ++i) {
                if (spel->bord[ander[0][i]][ander[1][i]] != speler)
                    ++score;
                if (spel->bord[zelf[0][i]][zelf[1][i]] == speler)
                    ++score;
            }
        }
        return score;
    };

    int blokEvaluatie(int evalSpeler) {
        if (allemaalDezelfde)
            return 0;

        int scoreSpeler = scoreBlok(evalSpeler);
        int scoreTegenstander = 0;
        for (int i = 0; i < spel->aantalSpelers; ++i) {
            if (i != evalSpeler)
                scoreTegenstander += scoreBlok(i);
        }

        if (scoreSpeler > scoreTegenstander)
            return scoreSpeler / scoreTegenstander;
        else
            return -1 * scoreTegenstander / scoreSpeler;
    };
};

class DeBoerSpaink : public Basisspeler {
public:
    DeBoerSpaink(Clobber* spelPointer);

    int volgendeZet();

private:
    const int evalSpeler = 0;
    int cutoff = 6;
    int knopenBezocht = 0;
    bool diepKijken = true;

    int evaluatie(Clobber* spel);

    int minimax(Clobber* spel, int& zet, int diepte);

    int alphaBetaMax(Clobber* spel, int alpha, int beta, int& zet, int diepte);

    int alphaBetaMin(Clobber* spel, int alpha, int beta, int& zet, int diepte);
};

DeBoerSpaink::DeBoerSpaink(Clobber* spelPointer) {
    spel = spelPointer;
}

int DeBoerSpaink::volgendeZet() {
    int aantalZetten = spel->aantalZetten(spel->aanZet);
    diepKijken = aantalZetten < 15;
    knopenBezocht = 0;

    int zet = rand() % aantalZetten;

    alphaBetaMax(spel, INT_MIN, INT_MAX, zet, 0);
    //minimax(spel, zet, 0);
    return zet;
}

int DeBoerSpaink::minimax(Clobber* spel, int& zet, int diepte) {
    if (!spel->isBezig() || diepte > cutoff) {
        return evaluatie(spel);
    }

    int waarde = spel->aanZet ? INT_MIN : INT_MAX;

    for (int i = 0; i < spel->aantalZetten(spel->aanZet); ++i) {
        Clobber kopie = *spel;
        kopie.doeZet(i);

        int nieuweWaarde = max(waarde, -minimax(&kopie, zet, diepte + 1));
        if (!diepte && nieuweWaarde > waarde)
            zet = i;
        waarde = nieuweWaarde;
    }

    return waarde;
}

int DeBoerSpaink::alphaBetaMax(Clobber* spel, int alpha, int beta, int& zet, int diepte) {
    knopenBezocht++;
    if (!spel->isBezig() || (knopenBezocht > 2000000 && !diepKijken)) {
        return evaluatie(spel);
    }

    for (int i = 0; i < spel->aantalZetten(spel->aanZet); ++i) {
        Clobber kopie = *spel;
        kopie.doeZet(i);

        int nieuweAlpha = max(alpha, alphaBetaMin(&kopie, alpha, beta, zet, diepte + 1));
        if (!diepte && nieuweAlpha > alpha)
            zet = i;
        alpha = nieuweAlpha;
        if (alpha >= beta)
            return beta;
    }

    return alpha;
}

int DeBoerSpaink::alphaBetaMin(Clobber* spel, int alpha, int beta, int& zet, int diepte) {
    knopenBezocht++;
    if (!spel->isBezig() || (knopenBezocht > 2000000 && !diepKijken)) {
        return evaluatie(spel);
    }

    for (int i = 0; i < spel->aantalZetten(spel->aanZet); ++i) {
        Clobber kopie = *spel;
        kopie.doeZet(i);

        beta = min(beta, alphaBetaMax(&kopie, alpha, beta, zet, diepte + 1));
        if (beta >= alpha)
            return alpha;
    }

    return beta;
}

int DeBoerSpaink::evaluatie(Clobber* spel) {
    if (!spel->isBezig()) {
        if (spel->winnaar() == evalSpeler) {
            return INT_MAX;
        } else {
            return INT_MIN;
        }
    }

    bool bezocht[MAX_BORD][MAX_BORD];
    for (int i = 0; i < spel->hoogte; ++i) {
        for (int j = 0; j < spel->breedte; ++j) {
            bezocht[i][j] = false;
        }
    }

    vector<Blok> blokken;
    for (int i = 0; i < spel->hoogte; ++i) {
        for (int j = 0; j < spel->breedte; ++j) {
            if (spel->bord[i][j] != LEEG_VAKJE && !bezocht[i][j]) {
                blokken.emplace_back(Blok(spel));
                blokken.back().vindRest(bezocht, i, j);
            }
        }
    }

    int evaluatie = 0;
    for (Blok& blok : blokken) {
        evaluatie += blok.blokEvaluatie(evalSpeler);
    }

    return evaluatie;
}
