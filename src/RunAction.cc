#include "RunAction.hh"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"

RunAction::RunAction()
    : G4UserRunAction(),
      fTotalEdep(0.),
      fTransmitted(0)
{
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run *)
{
  fTotalEdep = 0.;
  fTransmitted = 0;
}

void RunAction::AddRunEdep(G4double edep)
{
  fTotalEdep += edep;
}

void RunAction::CountTransmission()
{
  fTransmitted++;
}

void RunAction::EndOfRunAction(const G4Run *run)
{
  G4int nEv = run->GetNumberOfEvent();
  if (nEv == 0)
    return;

  G4double meanEdep = fTotalEdep / nEv;
  G4double transmission = (G4double)fTransmitted / nEv;

  auto detector =
      static_cast<const DetectorConstruction *>(
          G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  G4cout << "\n================ RUN RESULT ================\n";
  G4cout << "Material:            " << detector->GetShieldMaterial() << "\n";
  G4cout << "Thickness:           "
         << detector->GetThickness() / cm << " cm\n";
  G4cout << "Events:              " << nEv << "\n";
  G4cout << "Transmitted events:  " << fTransmitted << "\n";
  G4cout << "Transmission ratio:  " << transmission << "\n";
  G4cout << "Mean Edep:           "
         << meanEdep / MeV << " MeV\n";
  G4cout << "===========================================\n\n";
}