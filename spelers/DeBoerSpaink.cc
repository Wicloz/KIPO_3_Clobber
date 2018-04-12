#include <climits>

class DeBoerSpaink : public Basisspeler {
public:
    DeBoerSpaink(Clobber *spelPointer);

    int volgendeZet();

private:
    int evaluatie(Clobber *spel);

    int minimax(Clobber *spel, int &zet, int diepte);
};

DeBoerSpaink::DeBoerSpaink(Clobber *spelPointer) {
    spel = spelPointer;
}

int DeBoerSpaink::volgendeZet() {
    int zet = rand() % spel->aantalZetten(spel->aanZet);
    minimax(spel, zet, 0);
    return zet;
}

int DeBoerSpaink::minimax(Clobber *spel, int &zet, int diepte) {
    if (!spel->isBezig() || diepte > 4) {
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

int DeBoerSpaink::evaluatie(Clobber *spel) {
    if (spel->winnaar() == 0)
        return INT_MAX;

    int aantalZettenTegenstanders = 0;
    for (int i = 0; i < spel->aantalSpelers; ++i) {
        if (i != 0)
            aantalZettenTegenstanders += spel->aantalZetten(i);
    }
    if (aantalZettenTegenstanders != 0)
        return 100 * ((float)spel->aantalSpelers - 1.0f) * (float)spel->aantalZetten(0) / (float)aantalZettenTegenstanders;
    return INT_MIN;
}