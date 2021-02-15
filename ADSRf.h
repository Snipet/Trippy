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
        return LinearInterpolate(1, sustain, (point-attack) / (decay));
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
  void setSustain(float s) {
    sustain = s;
  }
  double getPoint() {
    return point / (attack + decay);
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
