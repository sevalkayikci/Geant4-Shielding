#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume *Construct() override;

  // Getter (istersen RunAction için kullanırsın)
  G4String GetShieldMaterial() const { return fShieldMaterialName; }
  G4double GetThickness() const { return fThickness; }

private:
  // ====== EKLENMESİ GEREKENLER ======
  G4int fCfg;          // config seçimi
  G4double fThickness; // fungus core thickness
  G4String fShieldMaterialName;
};

#endif