#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

EventAction::EventAction()
    : G4UserEventAction(),
      fEdep(0.)
{
}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event *)
{
  fEdep = 0.;
}

void EventAction::AddEdep(G4double edep)
{
  fEdep += edep;
}

void EventAction::EndOfEventAction(const G4Event *)
{
  auto runAction =
      const_cast<RunAction *>(
          static_cast<const RunAction *>(
              G4RunManager::GetRunManager()->GetUserRunAction()));
  runAction->AddRunEdep(fEdep);

  // Geçiş sayısını say (detector'a enerji bıraktıysa)
  if (fEdep > 0.)
    runAction->CountTransmission();
}