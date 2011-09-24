// Twister.h

#pragma once

#ifndef Twisterh
#define Twisterh

typedef unsigned int DWORD ;
// Mersenne Twister MT19937 with some subtle changes.

int SEED = 512;

struct Twister {
  static int Seed(DWORD Seed=SEED); //      a+(b-a)*t is from Global::Parameterize.
  static DWORD  Get(DWORD  a, DWORD  b) {return DWORD(a+(b-a)*GetDouble());}
  static double Get(double a, double b) {return       a+(b-a)*GetDouble();}
  static double GetDouble()             {return Get()*(1.0/ULONG_MAX);} // Returns in the Interval [0,1]
  static DWORD  Get(); // The fundamental method (quickest return)
private:
  static const int StateCount;
  static DWORD State[];
  static int Index; // Index to the State array.
  static void FillStates();
  static inline void SetState(int i, int j, int k);
};

#endif // def Twisterh
