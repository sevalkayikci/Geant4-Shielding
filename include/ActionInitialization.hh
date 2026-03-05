#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class DetectorConstruction;

class ActionInitialization : public G4VUserActionInitialization
{
public:
  // writeRoot=true  → sweep modu
  // writeRoot=false → GA single-run modu
  ActionInitialization(DetectorConstruction* det, bool writeRoot = true);
  virtual ~ActionInitialization();

  virtual void Build() const override;

private:
  DetectorConstruction* fDetector;
  bool                  fWriteRoot;
};

#endif
