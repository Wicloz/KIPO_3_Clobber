
//
// Clobber.cc
// Roy van Hal / Walter Kosters, 23 maart 2018
// De map spelers bevat verschillende clobber-spelers;
// bestudeer deze eerst, in het bijzonder de random speler
// zie TODO voor aanpassingen in deze file
// bord[0][0] zit linksboven
// spelers heten 0, 1, ...
// compileer: g++ -Wall -o programma Clobber.cc
// run: ./programma <hoogte> <breedte> <seed>
//

#include <iostream>
#include <cstdlib>

using namespace std;

const int MAX_BORD = 100;
const int MAX_SPELERS = 50;
const int ONBEKEND = -1;
const int LEEG_VAKJE = -1;

class Basisspeler;

class Clobber {
private:
    int bord[MAX_BORD][MAX_BORD];
    int beurt;
    int vorigeAanZet;
    Basisspeler *spelers[MAX_SPELERS];

    int initSpelers();

public:
    int aantalSpelers;
    int hoogte;
    int breedte;
    int aanZet;

    Clobber();

    Clobber(const int h, const int b);

    void init();

    void print() const;

    int winnaar() const;

    int aantalZetten(const int speler) const;

    void doeZet(const int zetNummer);

    int volgendeSpeler() const;

    void volgendeZet();

    bool isBezig() const;

    int speelSpel();
};

class Basisspeler {
protected:
    Clobber *spel;

public:
    virtual int volgendeZet() = 0;
};

#include "spelers/Dommespeler.cc"
#include "spelers/MCSpeler.cc"
#include "spelers/Randomspeler.cc"
//TODO voeg regel(s) toe met #include van eigen speler(s)

//TODO bepaal hier welke spelers 0, 1, ... gaan spelen
int Clobber::initSpelers() {
    spelers[0] = new Randomspeler(this);
    spelers[1] = new Randomspeler(this);
    return 2; // aantal spelers
}

Clobber::Clobber() {}

Clobber::Clobber(const int h, const int b) {
    hoogte = h;
    breedte = b;
    init();
}

void Clobber::init() {
    beurt = 0;
    aanZet = 0;
    aantalSpelers = initSpelers();
    for (int i = 0; i < hoogte; i++) {
        for (int j = 0; j < breedte; j++) {
            bord[i][j] = rand() % aantalSpelers; // vul bord random
        }
    }
}

int Clobber::volgendeSpeler() const {
    int volgende = (aanZet + 1) % aantalSpelers;
    while (aantalZetten(volgende) == 0 && volgende != aanZet) {
        volgende = (volgende + 1) % aantalSpelers;
    }
    if (volgende == aanZet) {
        return ONBEKEND;
    }
    return volgende;
}

void Clobber::volgendeZet() {
    vorigeAanZet = aanZet;
    aanZet = volgendeSpeler();
    beurt++;
}

//is spel nog bezig?
bool Clobber::isBezig() const {
    for (int i = 0; i < aantalSpelers; i++) {
        if (aantalZetten(i) > 0) {
            return true;
        }
    }
    return false;
}

//wie heeft gewonnen?
int Clobber::winnaar() const {
    if (isBezig()) {
        return ONBEKEND;
    }
    return vorigeAanZet;
}

//hoeveel zetten heeft speler?
int Clobber::aantalZetten(const int speler) const {
    int teller = 0;
    for (int i = 0; i < hoogte; i++)
        for (int j = 0; j < breedte; j++)
            if (bord[i][j] == speler) {
                if (i > 0 && bord[i - 1][j] != bord[i][j] && bord[i - 1][j] != LEEG_VAKJE)
                    teller++;
                if (j > 0 && bord[i][j - 1] != bord[i][j] && bord[i][j - 1] != LEEG_VAKJE)
                    teller++;
                if (j < breedte - 1 && bord[i][j + 1] != bord[i][j] && bord[i][j + 1] != LEEG_VAKJE)
                    teller++;
                if (i < hoogte - 1 && bord[i + 1][j] != bord[i][j] && bord[i + 1][j] != LEEG_VAKJE)
                    teller++;
            }
    return teller;
}

//doe zetNummer van de mogelijke zetten
void Clobber::doeZet(const int zetNummer) {
    int teller = 0;
    for (int i = 0; i < hoogte; i++) {
        for (int j = 0; j < breedte; j++) {
            if (bord[i][j] == aanZet) {
                if (i > 0 && bord[i - 1][j] != bord[i][j] && bord[i - 1][j] != LEEG_VAKJE) {
                    if (teller == zetNummer) {
                        bord[i][j] = LEEG_VAKJE;
                        bord[i - 1][j] = aanZet;
                        volgendeZet();
                        return;
                    }
                    teller++;
                }
                if (j > 0 && bord[i][j - 1] != bord[i][j] && bord[i][j - 1] != LEEG_VAKJE) {
                    if (teller == zetNummer) {
                        bord[i][j] = LEEG_VAKJE;
                        bord[i][j - 1] = aanZet;
                        volgendeZet();
                        return;
                    }
                    teller++;
                }
                if (j < breedte - 1 && bord[i][j + 1] != bord[i][j] && bord[i][j + 1] != LEEG_VAKJE) {
                    if (teller == zetNummer) {
                        bord[i][j] = LEEG_VAKJE;
                        bord[i][j + 1] = aanZet;
                        volgendeZet();
                        return;
                    }
                    teller++;
                }
                if (i < hoogte - 1 && bord[i + 1][j] != bord[i][j] && bord[i + 1][j] != LEEG_VAKJE) {
                    if (teller == zetNummer) {
                        bord[i][j] = LEEG_VAKJE;
                        bord[i + 1][j] = aanZet;
                        volgendeZet();
                        return;
                    }
                    teller++;
                }
            }
        }
    }
}

void Clobber::print() const {
    cout << "Toestand " << beurt << endl << "   ";
    for (int j = 0; j < breedte; j++)
        cout << "  " << j % 10;
    cout << endl;
    for (int i = 0; i < hoogte; i++) {
        if (i < 10)
            cout << " " << i << " ";
        else
            cout << i << " ";
        for (int j = 0; j < breedte; j++)
            if (bord[i][j] == LEEG_VAKJE)
                cout << "  .";
            else if (bord[i][j] < 10)
                cout << "  " << bord[i][j];
            else
                cout << " " << bord[i][j];
        cout << endl;
    }
    if (aanZet == ONBEKEND) {
        cout << "Niemand is aan de beurt" << endl << endl;
    } else {
        cout << "Speler " << aanZet << " is aan de beurt" << endl << endl;
    }
}

int Clobber::speelSpel() {
    while (isBezig()) {
        print();
        doeZet(spelers[aanZet]->volgendeZet());
    }
    print();
    return winnaar();
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Gebruik: " << argv[0] << " <hoogte> <breedte> <seed>" << endl;
        return 1;
    }
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    srand(atoi(argv[3]));
    Clobber potje(m, n);
    int winnaar = potje.speelSpel();

    cout << "Winnaar: " << winnaar << endl;
    return 0;
}
