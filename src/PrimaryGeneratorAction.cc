#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(nullptr)
{
  fParticleGun = new G4ParticleGun(1);

  auto particleTable = G4ParticleTable::GetParticleTable();
  auto gamma = particleTable->FindParticle("gamma");

  fParticleGun->SetParticleDefinition(gamma);

  // Cs-137 gibi 662 keV örnek (istersen değiştir)
  fParticleGun->SetParticleEnergy(0.662 * MeV);

  // Kaynak: Shield'in önüne koy (z ekseni negatiften pozitife at)
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, -20 * cm));

  // Yön: +z (shield ve detector'a doğru)
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *event)
{
  fParticleGun->GeneratePrimaryVertex(event);
}