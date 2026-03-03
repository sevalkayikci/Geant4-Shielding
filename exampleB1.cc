#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "QGSP_BERT.hh"

int main(int argc, char **argv)
{
  auto runManager = new G4RunManager();

  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new QGSP_BERT());
  runManager->SetUserInitialization(new ActionInitialization());

  runManager->Initialize();

  auto visManager = new G4VisExecutive();
  visManager->Initialize();

  auto UImanager = G4UImanager::GetUIpointer();

  if (argc == 1)
  {
    auto ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }
  else
  {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }

  delete visManager;
  delete runManager;
}