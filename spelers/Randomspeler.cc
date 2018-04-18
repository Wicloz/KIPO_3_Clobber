
class Randomspeler : public Basisspeler {
public:
    Randomspeler(Clobber* spelPointer);
    int volgendeZet();
};

Randomspeler::Randomspeler(Clobber * spelPointer) {
    spel = spelPointer;
}

int Randomspeler::volgendeZet() {
    return rand() % spel->aantalZetten(spel->aanZet);
}
