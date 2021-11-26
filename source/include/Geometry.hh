//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef Geometry_h
#define Geometry_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4AssemblyVolume.hh"
#include "constants.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
//------------------------------------------------------------------------------
  class Geometry : public G4VUserDetectorConstruction
//------------------------------------------------------------------------------
{
  public:
    Geometry();
   ~Geometry();

    G4VPhysicalVolume* Construct();

	//Active volumes
    G4LogicalVolume* GetScoringVol_PAD() const { return fScoringVol_PAD; }
    G4LogicalVolume* GetScoringVol_PIX() const { return fScoringVol_PIX; }
    G4AssemblyVolume* GetAssemblyVol_PIX() const { return fAssemblyVol_PIX; }

  protected:

    G4LogicalVolume*  fScoringVol_PAD;
    G4LogicalVolume*  fScoringVol_PIX;
    G4AssemblyVolume* fAssemblyVol_PIX;
};
#endif
