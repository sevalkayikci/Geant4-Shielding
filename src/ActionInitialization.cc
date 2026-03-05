#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization(DetectorConstruction* det, bool writeRoot)
    : G4VUserActionInitialization(), fDetector(det), fWriteRoot(writeRoot)
{
}

ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const
{
  auto gen = new PrimaryGeneratorAction();
  SetUserAction(gen);

  auto runAction = new RunAction(fDetector, fWriteRoot);
  runAction->SetGenerator(gen);
  SetUserAction(runAction);

  auto eventAction = new EventAction();
  SetUserAction(eventAction);

  SetUserAction(new SteppingAction(eventAction));
}
