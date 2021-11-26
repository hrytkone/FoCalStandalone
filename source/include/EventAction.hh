
#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "constants.hh"

/// Event action class
class EventAction : public G4UserEventAction
{
public:
    EventAction();
    virtual ~EventAction();

    virtual void    BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);

    inline void AddeDepPAD(G4int i, G4double de) {
        sum_eDep_PAD[i] += de;
    }

    inline G4double GetSumPAD(G4int i){
        return sum_eDep_PAD[i];
    }

    inline void AddeDepPIX(G4int ialpide, G4int i, G4double de) {
        sum_eDep_PIX[ialpide][i] += de;
    }

    inline G4double GetSumPIX(G4int ialpide, G4int i) {
        return sum_eDep_PIX[ialpide][i];
    }

private:
    G4double  sum_eDep_PAD[NpadX*NpadY*NumberPAD];
    G4double  sum_eDep_PIX[NalpideLayer*NumberPixRow*NumberPixCol*NumberPIX][NpixX*NpixY];
};

#endif
