#include <climits>

class DeBoerSpaink : public Basisspeler {
public:
    DeBoerSpaink(Clobber *spelPointer);

    int volgendeZet();

private:
    const int evalSpeler = 0;
    const int cutoff = 6;

    int evaluatie(Clobber *spel);

    int minimax(Clobber *spel, int &zet, int diepte);

    int alphaBetaMax(Clobber *spel, int alpha, int beta, int &zet, int diepte);

    int alphaBetaMin(Clobber *spel, int alpha, int beta, int &zet, int diepte);
};

DeBoerSpaink::DeBoerSpaink(Clobber *spelPointer) {
    spel = spelPointer;
}

int DeBoerSpaink::volgendeZet() {
    int zet = rand() % spel->aantalZetten(spel->aanZet);
    //minimax(spel, zet, 0);
    alphaBetaMax(spel, INT_MIN, INT_MAX, zet, 0);
    return zet;
}

int DeBoerSpaink::minimax(Clobber *spel, int &zet, int diepte) {
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

int DeBoerSpaink::alphaBetaMax(Clobber *spel, int alpha, int beta, int &zet, int diepte) {
    if (!spel->isBezig() || diepte > cutoff) {
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

int DeBoerSpaink::alphaBetaMin(Clobber *spel, int alpha, int beta, int &zet, int diepte) {
    if (!spel->isBezig() || diepte > cutoff) {
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

int DeBoerSpaink::evaluatie(Clobber *spel) {
    if (spel->winnaar() == evalSpeler)
        return INT_MAX;

    int aantalZettenTegenstanders = 0;
    for (int i = 0; i < spel->aantalSpelers; ++i) {
        if (i != evalSpeler)
            aantalZettenTegenstanders += spel->aantalZetten(i);
    }
    if (aantalZettenTegenstanders != evalSpeler)
        return 100 * (float) ((spel->aantalSpelers - 1) * spel->aantalZetten(evalSpeler)) /
                (float) aantalZettenTegenstanders;
    return INT_MIN;
}