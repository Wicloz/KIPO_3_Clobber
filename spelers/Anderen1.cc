#include <limits.h>

class AlphaBetaSpeler : public Basisspeler {
  public:
    AlphaBetaSpeler(Clobber* spelPointer, short diepte);
    int volgendeZet();
  private: 
    /*const*/ short dezeSpeler;
    const short maxDiepte;

    int getBesteZet(Clobber* kopie);
    int getMin(Clobber* spel, int alpha, int beta, int diepte);
    int getMax(Clobber* spel, int alpha, int beta, int diepte);
    
    bool heeftVijand(Clobber* kopie, int i, int j, const short speler);
    bool isGedekt(Clobber* kopie, int i, int j, const short speler);
    int telGedekteStenen(Clobber* kopie, const short speler);
    
    int telMogelijkhedenVoorSteen(Clobber* kopie, int i , int j, int & zet);
    int minimaalAantalZettenVoorSteen(Clobber* kopie);
    int echteAantalZetten(Clobber* kopie);
    
    int evaluatie(Clobber* kopie);
};

AlphaBetaSpeler::AlphaBetaSpeler(Clobber * spelPointer, const short diepte) :
  maxDiepte(diepte) {
  spel = spelPointer;
  dezeSpeler = spel->aanZet;
}

int AlphaBetaSpeler::volgendeZet() {
  int zet; 
  if(maxDiepte == 0)
    zet = rand() % spel->aantalZetten(spel->aanZet);
  else
    zet = getBesteZet(spel);
  return zet;
}

int AlphaBetaSpeler::getBesteZet(Clobber* spel) {
    int aantal = spel->aantalZetten(spel->aanZet); 
    int max = -1*INT_MAX;
    int bestezet; 
    for (int i = 0; i < aantal; i++) {
      Clobber s = *spel;
      s.doeZet(i);
      int temp = getMin(&s, max, INT_MAX, 1);  
      if (temp  > max) {
        bestezet = i;
        max = temp;
      }
    }
    return bestezet;
}

int AlphaBetaSpeler::getMin(Clobber* spel, int alpha, int beta, int diepte) {
  if (!spel->isBezig() || diepte == maxDiepte) {
    return evaluatie(spel);
  } else {    
    int aantal = spel->aantalZetten(spel->aanZet);
    for (int i = 0; i < aantal; i++) {
      Clobber kopie = *spel;
      kopie.doeZet(i);  
      beta = min(beta, getMax(&kopie, alpha, beta, diepte+1));
      if (alpha >= beta)
        return alpha;   
    }
  }
  return beta;
}

int AlphaBetaSpeler::getMax(Clobber* spel, int alpha, int beta, int diepte) {
  if (!spel->isBezig() || diepte == maxDiepte) {
    return evaluatie(spel);
  } else {
    int aantal = spel->aantalZetten(spel->aanZet);
    for (int i = 0; i < aantal; i++) {
      Clobber kopie = *spel;
      kopie.doeZet(i);
      alpha = max(alpha, getMin(&kopie, alpha, beta, diepte+1));
      if (alpha >= beta)
          return beta;
    }
  }
  return alpha;
}

bool AlphaBetaSpeler::heeftVijand(Clobber* kopie, int i, int j, const short speler) {
  if (i > 0)
    if(kopie->getVakje(i-1, j) != speler && kopie->getVakje(i-1, j) != LEEG_VAKJE) {
      return true;
    }
  if (j > 0)
    if(kopie->getVakje(i, j-1) != speler && kopie->getVakje(i, j-1) != LEEG_VAKJE) {
      return true;
    }
  if (i < kopie->hoogte)
      if (kopie->getVakje(i+1, j) != speler && kopie->getVakje(i+1, j) != LEEG_VAKJE) {
      return true;   
    }
  if (j < kopie->breedte)
    if (kopie->getVakje(i, j+1) != speler && kopie->getVakje(i, j+1) != LEEG_VAKJE) {
      return true;
    }
  return false;
}

bool AlphaBetaSpeler::isGedekt(Clobber* kopie, int i, int j, const short speler) {
  bool gevonden = false;
  if (i > 0) {
    if(kopie->getVakje(i-1, j) != speler && kopie->getVakje(i-1, j) != LEEG_VAKJE)
      return false;
    else if (kopie->getVakje(i-1, j) == speler) {
      gevonden = heeftVijand(kopie, i-1, j, speler);
    }
  }
  if (j > 0 ) {
    if(kopie->getVakje(i, j-1) != speler && kopie->getVakje(i, j-1) != LEEG_VAKJE)
      return false;
    else if (kopie->getVakje(i, j-1) == speler && !gevonden) {
      gevonden = heeftVijand(kopie, i, j-1, speler);  
    }
  }
  if (i < kopie->hoogte) {
    if (kopie->getVakje(i+1, j) != speler && kopie->getVakje(i+1, j) != LEEG_VAKJE)
      return false;
    else if (kopie->getVakje(i+1, j) == speler  && !gevonden) {
      gevonden = heeftVijand(kopie, i+1, j, speler);
    }
  }
  if (j < kopie->breedte ) {
    if (kopie->getVakje(i, j+1) != speler && kopie->getVakje(i, j+1) != LEEG_VAKJE)
      return false;
    else if (kopie->getVakje(i, j+1) == speler && !gevonden) {
      gevonden = heeftVijand(kopie, i, j+1, speler);
    }
  }

  return gevonden;
}

int AlphaBetaSpeler::telGedekteStenen(Clobber* kopie, const short speler) {
  int aantal=0;
  for (int i = 0; i < kopie->hoogte; i++) {
    for (int j = 0; j < kopie->breedte; j++) {
      if (kopie->getVakje(i,j) == speler) {
          if (isGedekt(kopie, i, j, speler)) {
            aantal++;          
          }
      }
    }
  }
  return aantal;
}

int AlphaBetaSpeler::telMogelijkhedenVoorSteen(Clobber* kopie, int i , int j, int & zet) {
  int aantalVijanden =0;
  if (i > 0)
    if(kopie->getVakje(i-1, j) != dezeSpeler && kopie->getVakje(i-1, j) >= 0) {
      aantalVijanden++;
      zet = 0;
    }
  if (j > 0)
    if(kopie->getVakje(i, j-1) != dezeSpeler && kopie->getVakje(i, j-1) >= 0) {
      aantalVijanden++; 
      zet = 1;
    }
  if (i < kopie->hoogte)
    if (kopie->getVakje(i+1, j) != dezeSpeler && kopie->getVakje(i+1, j) >= 0) {
      aantalVijanden++;
      zet = 2;
    }
  if (j < kopie->breedte)
    if (kopie->getVakje(i, j+1) != dezeSpeler && kopie->getVakje(i, j+1) >= 0) {
      aantalVijanden++;
      zet = 3;
    }
  return aantalVijanden;
}

int AlphaBetaSpeler::minimaalAantalZettenVoorSteen(Clobber* kopie) {
  int minimum = kopie->hoogte * kopie->breedte;
  int value;
  int zet;
  for (int i = 0; i < kopie->hoogte; i++) {
    for (int j = 0; j < kopie->breedte; j++) {
      if (kopie->getVakje(i,j) == dezeSpeler) {
        value = telMogelijkhedenVoorSteen(kopie, i, j, zet);
        if (value < minimum && value != 0)
          minimum = value; 
      } 
    }
  }
  return minimum;
}

int AlphaBetaSpeler::echteAantalZetten(Clobber* kopie) {
  int hoogte = kopie->hoogte;
  int breedte = kopie->breedte;
  Clobber bord = *kopie;
  int zet;
  int aantal=0;

  int mogelijkheden = minimaalAantalZettenVoorSteen(&bord);
  while(mogelijkheden != hoogte * breedte) {
    for (int i = 0; i < hoogte; i++) {
      for (int j = 0; j < breedte; j++) {
        if (bord.getVakje(i,j) == dezeSpeler) {
          if (telMogelijkhedenVoorSteen(&bord, i, j, zet) == mogelijkheden && bord.getVakje(i,j) == dezeSpeler) {
            bord.setVakje(i,j,LEEG_VAKJE);
            aantal++;            
            if (zet == 0) {
              bord.setVakje(i-1, j, LEEG_VAKJE);
            } else if (zet == 1) {
              bord.setVakje(i, j-1, LEEG_VAKJE);
            } else if (zet == 2) {
              bord.setVakje(i+1, j, LEEG_VAKJE);
            } else if (zet == 3) {
              bord.setVakje(i, j+1, LEEG_VAKJE);
            }
          }
        }
      }
    }
    mogelijkheden = minimaalAantalZettenVoorSteen(&bord);
  }
  return aantal;
}

int AlphaBetaSpeler::evaluatie(Clobber* kopie) {
  int score = 0;
  if (!kopie->isBezig()) {
    if (kopie->winnaar() == dezeSpeler)
      score = 10000;
    else
      score = -10000;
  } else {
    score += telGedekteStenen(kopie, dezeSpeler);
    for(int speler = 0; speler < kopie->aantalSpelers; speler++){
      if(speler != dezeSpeler)
        score -= 1.5 * telGedekteStenen(kopie, !dezeSpeler);
    }
    
    int aantal = echteAantalZetten(kopie);
    int temp = (kopie->breedte*kopie->hoogte);
    if(kopie->aanZet == dezeSpeler) {
      if(aantal % kopie->aantalSpelers == 0)
        temp *= -1;
    } else {
      if(aantal % kopie->aantalSpelers != 0)
        temp *= -1;
    }
    if (aantal != 0)
      temp /= aantal;
    
    score += temp;
  }
  return score;
}