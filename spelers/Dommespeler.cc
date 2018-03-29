
class Dommespeler : public Basisspeler {
public:
    Dommespeler(Clobber* spelPointer);
    int volgendeZet();
};

Dommespeler::Dommespeler(Clobber * spelPointer) {
    spel = spelPointer;
}

int Dommespeler::volgendeZet() {
    return 0;
}
