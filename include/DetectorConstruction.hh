#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume* Construct() override;

  void     SetCfg(G4int cfg)        { fCfg = cfg; }
  G4int    GetCfg()           const { return fCfg; }
  void     SetThickness(G4double t) { fThickness = t; }
  G4double GetThickness()     const { return fThickness; }
  G4String GetShieldMaterial()const { return fShieldMaterialName; }

private:
  G4int    fCfg;
  G4double fThickness;
  G4double fWaterThickness;
  G4String fShieldMaterialName;
};

#endif
