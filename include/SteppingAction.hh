#ifndef STEPPING_ACTION_HH
#define STEPPING_ACTION_HH

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class EventAction;
class DetectorConstruction;
class G4Step;
class G4LogicalVolume;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction, const DetectorConstruction* detector);
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;

  private:
    bool IsDetectorVolume(G4LogicalVolume* vol) const;

    EventAction* fEventAction = nullptr;
    const DetectorConstruction* fDetector = nullptr;

    static constexpr G4double kPromptWindow = 100.0; // ns
};

#endif
