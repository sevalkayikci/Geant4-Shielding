#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManager.hh"
#include "QGSP_BERT.hh"
#include "G4SystemOfUnits.hh"

#include <vector>
#include <string>
#include <cstdlib>

int main(int argc, char** argv)
{
  // ================= ARG PARSING =================
  // Sweep modu (default): ./exampleB1
  // GA single modu:       ./exampleB1 --single --cfg 5 --thickness 10.0 --events 20000
  bool     singleMode  = false;
  G4int    s_cfg       = 0;
  G4double s_thickness = 5.0;   // cm
  G4int    s_events    = 20000;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if      (arg == "--single")                     singleMode  = true;
    else if (arg == "--cfg"       && i+1 < argc)    s_cfg       = std::atoi(argv[++i]);
    else if (arg == "--thickness" && i+1 < argc)    s_thickness = std::atof(argv[++i]);
    else if (arg == "--events"    && i+1 < argc)    s_events    = std::atoi(argv[++i]);
  }

  // ================= SETUP =================
  auto detector   = new DetectorConstruction();
  auto runManager = new G4RunManager();

  if (singleMode) {
    // GA modu: cfg ve thickness onceden set edilir, ROOT yazilmaz
    detector->SetCfg(s_cfg);
    detector->SetThickness(s_thickness * cm);
    runManager->SetUserInitialization(detector);
    runManager->SetUserInitialization(new QGSP_BERT());
    runManager->SetUserInitialization(new ActionInitialization(detector, /*writeRoot=*/false));
    runManager->Initialize();
    runManager->BeamOn(s_events);
  }
  else {
    // ================= SWEEP MODU =================
    runManager->SetUserInitialization(detector);
    runManager->SetUserInitialization(new QGSP_BERT());
    runManager->SetUserInitialization(new ActionInitialization(detector, /*writeRoot=*/true));
    runManager->Initialize();

    const std::vector<G4int>    cfgs        = {0, 1, 2, 3, 4, 5, 6, 7, 8,
                                                9, 10, 11, 12, 13, 14, 15};
    const std::vector<G4double> thicknesses = {2.0, 5.0, 10.0, 15.0};  // cm
    const G4int                 nEvents     = 80000;

    G4int total = (G4int)(cfgs.size() * thicknesses.size());
    G4int done  = 0;

    for (G4int cfg : cfgs) {
      for (G4double t : thicknesses) {
        detector->SetCfg(cfg);
        detector->SetThickness(t * cm);
        runManager->ReinitializeGeometry(true);

        G4cout << "\n[" << ++done << "/" << total << "]"
               << "  cfg=" << cfg
               << "  thickness=" << t << " cm\n";

        runManager->BeamOn(nEvents);
      }
    }

    G4cout << "\n===== SWEEP TAMAMLANDI =====\n"
           << "Sonuclar: results/melanin_results.root\n"
           << "TTree adi: T\n"
           << "Branches: cfg, material, thickness_cm, energy_keV,\n"
           << "          events, transmitted, transmission, mean_edep_MeV\n"
           << "============================\n";
  }

  delete runManager;
  return 0;
}
