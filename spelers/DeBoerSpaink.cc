#include <climits>

class DeBoerSpaink : public Basisspeler {
public:
    DeBoerSpaink(Clobber* spelPointer);
    int volgendeZet();

private:
    int evaluatie(int speler);
    int minimax(Clobber* spel, int& zet, int diepte);
};

DeBoerSpaink::DeBoerSpaink(Clobber* spelPointer) {
    spel = spelPointer;
}

int DeBoerSpaink::volgendeZet() {
    int zet = rand() % spel->aantalZetten(spel->aanZet);
    minimax(spel, zet, 0);
    return zet;
}

int DeBoerSpaink::minimax(Clobber* spel, int& zet, int diepte) {
    if (!spel->isBezig() || diepte > 4) {
        return evaluatie(spel->aanZet);
    }

    int waarde = spel->aanZet ? INT_MAX;

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

int DeBoerSpaink::evaluatie(int speler) {
    if (spel->winnaar() == speler)
        return INT_MAX;

    int aantalZettenTegenstanders = 0;
    for (int i = 0; i < spel->aantalSpelers; ++i) {
        if (i != speler)
            aantalZettenTegenstanders += spel->aantalZetten(i);
    }

    return (spel->aantalSpelers - 1) * spel->aantalZetten(speler) / aantalZettenTegenstanders;
}