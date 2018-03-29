
const int MC_POTJES = 1000;

class MCSpeler : public Basisspeler {
public:
    int speelRandomSpel(Clobber &randomspel);

public:
    MCSpeler(Clobber *spelPointer);

    int volgendeZet();
};

MCSpeler::MCSpeler(Clobber *spelPointer) {
    spel = spelPointer;
}

int MCSpeler::speelRandomSpel(Clobber &randomspel) {
    while (randomspel.isBezig()) {
        randomspel.doeZet(rand() % randomspel.aantalZetten(randomspel.aanZet));
    }
    return randomspel.winnaar();
}


int MCSpeler::volgendeZet() {
    Clobber kopie;
    int huidigeScore;
    int besteZet = 0;
    int besteScore = 0;
    int aantal = spel->aantalZetten(spel->aanZet);
    for (int i = 0; i < aantal; i++) {
        huidigeScore = 0;
        for (int k = 0; k < MC_POTJES; k++) {
            kopie = *spel;
            kopie.doeZet(i);
            if (speelRandomSpel(kopie) == spel->aanZet) {
                huidigeScore++;
            }
        }
        if (huidigeScore > besteScore) {
            besteZet = i;
            besteScore = huidigeScore;
        }
    }
    return besteZet;
}
