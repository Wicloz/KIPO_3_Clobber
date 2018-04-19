#include <climits>
#include <cstdlib>
#include <vector>

using namespace std;

const int MINIMAX = 0;
const int ALPHABETA = 1;

// https://www.researchgate.net/publication/221932254_New_Trends_in_Clobber_Programming

inline bool inBordMatrix(const int& i, const int& j, const Clobber* spel) {
    return i >= 0 && j >= 0 && i < spel->hoogte && j < spel->breedte;
};

struct Coordinaat {
    Coordinaat(const int& i, const int& j) : i(i), j(j) {};
    int i;
    int j;
};

class Blok {
private:
    vector<Coordinaat> coordinaten;
    Clobber* spel = nullptr;
    bool allemaalDezelfde = true;
    int eersteKleur = LEEG_VAKJE;

    int scoreBlok(const int& speler) {
        int ander[4][2] = {{0,  -1},
                           {0,  1},
                           {-1, 0},
                           {1,  0}};
        int zelf[4][6] = {{-1, -1, 0,  -1, -1, 0},
                          {-1, 1,  -1, 0,  0,  1},
                          {1,  -1, 1,  0,  0,  -1},
                          {1,  1,  0,  1,  1,  0}};
        int score = 0;

        for (Coordinaat& coordinaat : coordinaten) {
            if (spel->bord[coordinaat.i][coordinaat.j] == speler) {
                // 1
                ++score;

                // O
                for (int i = 0; i < 4; ++i) {
                    int x = ander[i][0] + coordinaat.i;
                    int y = ander[i][1] + coordinaat.j;
                    if (inBordMatrix(x, y, spel) && spel->bord[x][y] != speler && spel->bord[x][y] != LEEG_VAKJE) {
                        ++score;
                    }
                }

                // D
                for (int i = 0; i < 4; ++i) {
                    int x = zelf[i][0] + coordinaat.i;
                    int y = zelf[i][1] + coordinaat.j;
                    int x1 = zelf[i][2] + coordinaat.i;
                    int y1 = zelf[i][3] + coordinaat.j;
                    int x2 = zelf[i][4] + coordinaat.i;
                    int y2 = zelf[i][5] + coordinaat.j;
                    if (inBordMatrix(x, y, spel) && spel->bord[x][y] == speler &&
                        ((spel->bord[x1][y1] != speler && spel->bord[x1][y1] != LEEG_VAKJE) ||
                         (spel->bord[x2][y2] != speler && spel->bord[x2][y2] != LEEG_VAKJE))) {
                        ++score;
                    }
                }
            }
        }

        return score;
    };

public:
    Blok(Clobber* spelPointer) : spel(spelPointer) {};

    void vindBlokVakjes(bool (& bezocht)[MAX_BORD][MAX_BORD], const int& i, const int& j) {
        for (int k = -1; k < 2; ++k) {
            for (int l = -1; l < 2; ++l) {
                int x = k + i;
                int y = l + j;
                if (inBordMatrix(x, y, spel) && (l == 0 || k == 0) && !bezocht[x][y] &&
                    spel->bord[x][y] != LEEG_VAKJE) {

                    if (eersteKleur == LEEG_VAKJE) {
                        eersteKleur = spel->bord[x][y];
                    }
                    if (eersteKleur != spel->bord[x][y]) {
                        allemaalDezelfde = false;
                    }

                    bezocht[x][y] = true;
                    coordinaten.emplace_back(Coordinaat(x, y));

                    vindBlokVakjes(bezocht, x, y);
                }
            }
        }
    };

    float blokEvaluatie(const int& speler) {
        float scoreSpeler = scoreBlok(speler);
        float scoreTegenstanders = 0;
        for (int i = 0; i < spel->aantalSpelers; ++i) {
            if (i != speler) {
                scoreTegenstanders += scoreBlok(i);
            }
        }

        if (allemaalDezelfde) {
            return scoreSpeler + scoreTegenstanders;
        } else if (scoreSpeler > scoreTegenstanders) {
            return scoreSpeler / scoreTegenstanders;
        } else {
            return (-1 * scoreTegenstanders) / scoreSpeler;
        }
    };
};

class DeBoerSpaink : public Basisspeler {
public:
    DeBoerSpaink(Clobber* spelPointer) : spel(spelPointer) {};

    DeBoerSpaink(Clobber* spelPointer, const int& spelerNummer, const int& speelStijl) : spel(spelPointer),
                                                                                         dezeSpeler(spelerNummer),
                                                                                         speelStijl(speelStijl) {};

    int volgendeZet() {
        int aantalZetten = spel->aantalZetten(spel->aanZet);
        diepKijken = aantalZetten < cutoffZetten;

        int zet = rand() % aantalZetten;
        switch (speelStijl) {
            case MINIMAX:
                miniMaxMax(spel, zet, 0);
                break;
            case ALPHABETA:
                alphaBetaMax(spel, INT_MIN, INT_MAX, zet, 0);
                break;
        }
        return zet;
    };

private:
    int dezeSpeler = 0;
    int speelStijl = ALPHABETA;
    Clobber* spel = nullptr;
    int cutoffDiepte = 0;
    int cutoffZetten = -1;
    bool diepKijken = true;

    float evaluatie(Clobber* spel) {
        int speler = dezeSpeler;

        if (!spel->isBezig()) {
            if (spel->winnaar() == speler) {
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
                    Blok newBlok = Blok(spel);
                    newBlok.vindBlokVakjes(bezocht, i, j);
                    blokken.emplace_back(newBlok);
                }
            }
        }

        float evaluatie = 0;
        for (Blok& blok : blokken) {
            evaluatie += blok.blokEvaluatie(speler);
        }

        return evaluatie;
    };

    float miniMaxMax(Clobber* spel, int& zet, const int& diepte) {
        if (!spel->isBezig() || (diepte > cutoffDiepte && !diepKijken)) {
            return evaluatie(spel);
        }

        float waarde = INT_MIN;

        for (int i = 0; i < spel->aantalZetten(spel->aanZet); ++i) {
            Clobber kopie = *spel;
            kopie.doeZet(i);

            float nieuweWaarde = max(waarde, miniMaxMin(&kopie, zet, diepte + 1));
            if (diepte == 0 && nieuweWaarde > waarde) {
                zet = i;
            }
            waarde = nieuweWaarde;
        }

        return waarde;
    };

    float miniMaxMin(Clobber* spel, int& zet, const int& diepte) {
        if (!spel->isBezig() || (diepte > cutoffDiepte && !diepKijken)) {
            return evaluatie(spel);
        }

        float waarde = INT_MAX;

        for (int i = 0; i < spel->aantalZetten(spel->aanZet); ++i) {
            Clobber kopie = *spel;
            kopie.doeZet(i);
            waarde = min(waarde, miniMaxMax(&kopie, zet, diepte + 1));
        }

        return waarde;
    };

    float alphaBetaMax(Clobber* spel, float alpha, const float& beta, int& zet, const int& diepte) {
        if (!spel->isBezig() || (diepte > cutoffDiepte && !diepKijken)) {
            return evaluatie(spel);
        }

        for (int i = 0; i < spel->aantalZetten(spel->aanZet); ++i) {
            Clobber kopie = *spel;
            kopie.doeZet(i);

            float nieuweAlpha = max(alpha, alphaBetaMin(&kopie, alpha, beta, zet, diepte + 1));
            if (diepte == 0 && nieuweAlpha > alpha) {
                zet = i;
            }
            alpha = nieuweAlpha;

            if (alpha >= beta) {
                return beta;
            }
        }

        return alpha;
    };

    float alphaBetaMin(Clobber* spel, const float& alpha, float beta, int& zet, const int& diepte) {
        if (!spel->isBezig() || (diepte > cutoffDiepte && !diepKijken)) {
            return evaluatie(spel);
        }

        for (int i = 0; i < spel->aantalZetten(spel->aanZet); ++i) {
            Clobber kopie = *spel;
            kopie.doeZet(i);

            beta = min(beta, alphaBetaMax(&kopie, alpha, beta, zet, diepte + 1));
            if (beta >= alpha) {
                return alpha;
            }
        }

        return beta;
    };
};
