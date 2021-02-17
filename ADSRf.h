#pragma once

class ADSRM {
public:
  ADSRM() {
    attack = 0;
    decay = 2000;
    sustain = 0;
    point = 0;
  }

  void retrigger() {
    point = 0;
  }

  double tick() {
    if (point < attack + decay) {
      point++;
      if (point < attack) {
        return point / attack;
  
      }
      else {
        if (point < attack + decay) {
          return LinearInterpolate(1, sustain, (point - attack) / (decay));
        }
        else {
          return sustain;
        }
      }
    }
    else {
      return sustain;
    }
  }
  void setAttack(float a) {
    attack = a;
  }
  void setDecay(float d) {
    decay = d;
  }
  void setSustain(double s) {
    sustain = s;
  }
  double getPoint() {
    if (point < attack + decay) {
      return point / (attack + decay);
    }
    else {
      return 1;
    }
  }

  double LinearInterpolate(
    double y1, double y2,
    double mu)
  {
    return(y1 * (1 - mu) + y2 * mu);
  }
private:
  float attack;
  float decay;
  float sustain;
  float point;
};
