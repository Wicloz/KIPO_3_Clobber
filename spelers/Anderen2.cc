#include <climits>
#include <algorithm>
#include <iostream>

const unsigned int VRIEND_IN_BUURT = 1;
const unsigned int VIJAND_IN_BUURT = 1 << 1;

//Markeert de buren van een bepaalde positie mits de buurtpositie een steen bevat
void
markeerBuren(const Clobber& spel, int inDeBuurt[MAX_BORD][MAX_BORD], const int markering, const int x, const int y) {
    //Verwacht dat x en y binnen het bord vallen om een paar checks te besparen
    if (x - 1 >= 0 && spel.bord[y][x - 1] != -1) {
        inDeBuurt[y][x - 1] |= markering;
    }

    if (x + 1 < spel.breedte && spel.bord[y][x + 1] != -1) {
        inDeBuurt[y][x + 1] |= markering;
    }

    if (y + 1 < spel.hoogte && spel.bord[y + 1][x] != -1) {
        inDeBuurt[y + 1][x] |= markering;
    }

    if (y - 1 >= 0 && spel.bord[y - 1][x] != -1) {
        inDeBuurt[y - 1][x] |= markering;
    }
}

//Een nuttige steen zou nog gebruikt kunnen worden om de steen van een tegenstander te clobberen
void berekenNuttigeStenen(const Clobber& spel, const int speler, int radius, int& nuttigeVriendStenen,
                          int& nuttigeVijandStenen) {
    nuttigeVriendStenen = 0;
    nuttigeVijandStenen = 0;
    if (radius <= 0) {
        return;
    }

    //Markeer directe buren van elke steen
    int inDeBuurt[MAX_BORD][MAX_BORD] = {{0}};
    for (int y = 0; y < spel.hoogte; ++y) {
        for (int x = 0; x < spel.breedte; ++x) {
            if (spel.bord[y][x] == speler) {
                markeerBuren(spel, inDeBuurt, VRIEND_IN_BUURT, x,
                             y); //TODO: Also mark position itself in addition to the neighbours? Don't think necessary but think
            } else if (spel.bord[y][x] != -1) {
                markeerBuren(spel, inDeBuurt, VIJAND_IN_BUURT, x, y);
            }
        }
    }
    //Markeer de buren van al reeds gemarkeerde vakjes met dezelfde markering
    --radius;
    unsigned int markering;
    while (radius) {
        for (int y = 0; y < spel.hoogte; ++y) {
            for (int x = 0; x < spel.breedte; ++x) {
                if (inDeBuurt[y][x] != 0) {
                    markering = inDeBuurt[y][x];
                    markeerBuren(spel, inDeBuurt, markering, x, y);
                }
            }
        }
        --radius;
    }


    for (int y = 0; y < spel.hoogte; ++y) {
        for (int x = 0; x < spel.breedte; ++x) {
            if (spel.bord[y][x] == speler) {
                if ((inDeBuurt[y][x] & VIJAND_IN_BUURT) != 0) {
                    ++nuttigeVriendStenen;
                }
            } else if (spel.bord[y][x] != -1 && (inDeBuurt[y][x] & VRIEND_IN_BUURT) != 0) {
                ++nuttigeVijandStenen;
            }
        }
    }
}

class ABSpeler : public Basisspeler {
public:
    ABSpeler(Clobber* spelPointer);

    int volgendeZet();

private:
    int evaluatie(const Clobber& huidigSpel, short speler);

    int alphaBeta(const Clobber& huidigSpel, int diepte, int& deZet);

    bool isEindtoestand(const Clobber& huidigSpel, int diepte);

    int mini(const Clobber& huidigSpel, int a, int b, int diepte);

    int maxi(const Clobber& huidigSpel, int a, int b, int diepte);
};


ABSpeler::ABSpeler(Clobber* spelPointer) {
    spel = spelPointer;
}

int ABSpeler::volgendeZet() {
    int besteZet = -1;
    alphaBeta(*spel, 6, besteZet);
    return besteZet;
}


int ABSpeler::evaluatie(const Clobber& huidigSpel, short speler) {
    int waarde = huidigSpel.aantalZetten(spel->aanZet);
    int aanZet = (huidigSpel.vorigeAanZet + 1) % huidigSpel.aantalSpelers;
    if (waarde == 0) {
        //If it's the root players turn and there are no moves possible, then the game is lost.
        if (aanZet == spel->aanZet) {
            return -99999;
        }

    }
    //If it's not the root players turn and there are no moves possible, then the game is won.
    if (waarde == 0 && aanZet != spel->aanZet) {
        return 99999;
    }

    int nuttigeVriendStenen, nuttigeVijandStenen;
    berekenNuttigeStenen(huidigSpel, speler, 12, nuttigeVriendStenen, nuttigeVijandStenen);
    waarde = 1 * nuttigeVriendStenen;
    waarde -= 1 * nuttigeVijandStenen;
    return waarde;
}

int ABSpeler::alphaBeta(const Clobber& huidigSpel, int diepte, int& deZet) {
    int aantalZetten = huidigSpel.aantalZetten(huidigSpel.aanZet);
    //If depth is zero then a random move needs to be chosen.
    if (isEindtoestand(huidigSpel, diepte) && aantalZetten != 0) {
        deZet = rand() % aantalZetten;
        return 0;
    }

    Clobber kopie;
    int a = INT_MIN;
    int b = INT_MAX;
    int waarde;
    for (int i = 0; i < aantalZetten; ++i) {
        kopie = huidigSpel;
        kopie.doeZet(i);
        waarde = mini(kopie, a, b, diepte - 1);

        if (waarde > a) {
            a = waarde;
            deZet = i;
        }
    }
    return a;
}

bool ABSpeler::isEindtoestand(const Clobber& huidigSpel, int diepte) {
    return !huidigSpel.isBezig() || diepte == 0;
}

int ABSpeler::mini(const Clobber& huidigSpel, int a, int b, int diepte) {
    if (isEindtoestand(huidigSpel, diepte)) {
        return evaluatie(huidigSpel, spel->aanZet);
    }

    Clobber kopie;
    int aantalZetten = huidigSpel.aantalZetten(huidigSpel.aanZet);
    for (int i = 0; i < aantalZetten; ++i) {
        kopie = huidigSpel;
        kopie.doeZet(i);
        if (kopie.aanZet == spel->aanZet) {
            b = min(b, maxi(kopie, a, b, diepte - 1));
        } else {
            b = min(b, mini(kopie, a, b, diepte - 1));
        }

        if (a >= b) {
            return b;
        }
    }

    return b;
}

int ABSpeler::maxi(const Clobber& huidigSpel, int a, int b, int diepte) {
    if (isEindtoestand(huidigSpel, diepte)) {
        return evaluatie(huidigSpel, spel->aanZet);
    }

    Clobber kopie;
    int aantalZetten = huidigSpel.aantalZetten(huidigSpel.aanZet);
    for (int i = 0; i < aantalZetten; ++i) {
        kopie = huidigSpel;
        kopie.doeZet(i);

        a = max(a, mini(kopie, a, b, diepte - 1));

        if (a >= b) {
            return a;
        }
    }

    return a;
}
