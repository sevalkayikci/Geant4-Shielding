#include "SteppingAction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

SteppingAction::SteppingAction(EventAction *eventAction)
    : G4UserSteppingAction(),
      fEventAction(eventAction)
{
}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
  auto preVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  if (!preVol)
    return;

  auto lv = preVol->GetLogicalVolume();
  if (!lv)
    return;

  // Sadece Detector içinde bırakılan enerjiyi topla
  if (lv->GetName() != "Detector")
    return;

  G4double edep = step->GetTotalEnergyDeposit();
  if (edep > 0.)
    fEventAction->AddEdep(edep);
}