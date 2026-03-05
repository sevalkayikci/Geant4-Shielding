#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* event) override;

  void     SetEnergy(G4double e) { fEnergy = e; }
  G4double GetEnergy()     const { return fEnergy; }

private:
  G4ParticleGun* fParticleGun;
  G4double       fEnergy;
};

#endif
