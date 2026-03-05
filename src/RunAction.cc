#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

#include "TFile.h"
#include "TTree.h"

#include <filesystem>
#include <cstring>

RunAction::RunAction(const DetectorConstruction* det, bool writeRoot)
    : G4UserRunAction(),
      fDet(det), fGen(nullptr),
      fWriteRoot(writeRoot),
      fTotalEdep(0.), fTransmitted(0),
      fFile(nullptr), fTree(nullptr)
{
  if (!fWriteRoot) return; // GA modu: ROOT yok

  std::filesystem::create_directories("results");
  fFile = new TFile("results/melanin_results.root", "RECREATE");
  fTree = new TTree("T", "Melanin Shield Sweep");

  fTree->Branch("cfg",          &fB_cfg,          "cfg/I");
  fTree->Branch("material",      fB_material,      "material/C");
  fTree->Branch("thickness_cm", &fB_thickness_cm,  "thickness_cm/D");
  fTree->Branch("energy_keV",   &fB_energy_keV,    "energy_keV/D");
  fTree->Branch("events",       &fB_events,        "events/I");
  fTree->Branch("transmitted",  &fB_transmitted,   "transmitted/I");
  fTree->Branch("transmission", &fB_transmission,  "transmission/D");
  fTree->Branch("mean_edep_MeV",&fB_mean_edep_MeV, "mean_edep_MeV/D");
}

RunAction::~RunAction()
{
  if (fFile) {
    fFile->Write();
    fFile->Close();
    delete fFile;
  }
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  fTotalEdep   = 0.;
  fTransmitted = 0;
}

void RunAction::AddRunEdep(G4double edep)  { fTotalEdep += edep; }
void RunAction::CountTransmission()        { fTransmitted++; }

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nEv = run->GetNumberOfEvent();
  if (nEv == 0) return;

  G4double meanEdep     = fTotalEdep / nEv;
  G4double transmission = (G4double)fTransmitted / nEv;

  fB_cfg           = fDet->GetCfg();
  fB_thickness_cm  = fDet->GetThickness() / cm;
  fB_energy_keV    = fGen ? fGen->GetEnergy() / keV : 662.0;
  fB_events        = nEv;
  fB_transmitted   = fTransmitted;
  fB_transmission  = transmission;
  fB_mean_edep_MeV = meanEdep / MeV;
  std::strncpy(fB_material, fDet->GetShieldMaterial().c_str(), 63);
  fB_material[63]  = '\0';

  if (fWriteRoot) fTree->Fill();

  // GA Python'un parse edecegi format — degistirme!
  G4cout << "\n================ RUN RESULT ================\n"
         << "Config:      " << fB_cfg << " (" << fDet->GetShieldMaterial() << ")\n"
         << "Thickness:   " << fB_thickness_cm << " cm\n"
         << "Energy:      " << fB_energy_keV   << " keV\n"
         << "Events:      " << nEv              << "\n"
         << "Transmitted: " << fTransmitted     << "\n"
         << "Transmission:" << transmission      << "\n"
         << "Mean Edep:   " << meanEdep / MeV   << " MeV\n"
         << "============================================\n\n";
}
