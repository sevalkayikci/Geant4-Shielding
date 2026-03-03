#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class EventAction : public G4UserEventAction
{
public:
  EventAction();
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event *) override;
  virtual void EndOfEventAction(const G4Event *) override;

  void AddEdep(G4double edep);
  G4double GetEdep() const { return fEdep; }

private:
  G4double fEdep;
};

#endif