#include "ActionInitialization.hh"

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

#include "G4RunManager.hh"

ActionInitialization::ActionInitialization()
    : G4VUserActionInitialization()
{
}

ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const
{
  // Primary (kaynak)
  SetUserAction(new PrimaryGeneratorAction());

  // Run
  auto runAction = new RunAction();
  SetUserAction(runAction);

  // Event
  auto eventAction = new EventAction();
  SetUserAction(eventAction);

  // Stepping (EventAction'a enerji ekler)
  SetUserAction(new SteppingAction(eventAction));
}