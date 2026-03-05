#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class DetectorConstruction;
class PrimaryGeneratorAction;
class TFile;
class TTree;

class RunAction : public G4UserRunAction
{
public:
  // writeRoot=true  → sweep modu: ROOT dosyasina yaz
  // writeRoot=false → GA modu:    sadece stdout, ROOT dosyasina dokunma
  RunAction(const DetectorConstruction* det, bool writeRoot = true);
  virtual ~RunAction();

  virtual void BeginOfRunAction(const G4Run*) override;
  virtual void EndOfRunAction(const G4Run*)   override;

  void SetGenerator(PrimaryGeneratorAction* gen) { fGen = gen; }
  void AddRunEdep(G4double edep);
  void CountTransmission();

private:
  const DetectorConstruction* fDet;
  PrimaryGeneratorAction*     fGen;
  bool                        fWriteRoot;

  G4double fTotalEdep;
  G4int    fTransmitted;

  TFile* fFile;
  TTree* fTree;

  // TTree branch variables
  G4int    fB_cfg;
  G4double fB_thickness_cm;
  G4double fB_energy_keV;
  G4int    fB_events;
  G4int    fB_transmitted;
  G4double fB_transmission;
  G4double fB_mean_edep_MeV;
  char     fB_material[64];
};

#endif
