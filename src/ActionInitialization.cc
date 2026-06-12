#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

ActionInitialization::ActionInitialization(const DetectorConstruction* detector)
  : fDetector(detector)
{}

void ActionInitialization::BuildForMaster() const
{
    SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
    auto* generator = new PrimaryGeneratorAction(fDetector);
    SetUserAction(generator);
    SetUserAction(new RunAction());

    auto* eventAction = new EventAction(generator);
    SetUserAction(eventAction);

    SetUserAction(new SteppingAction(eventAction, fDetector));
}
