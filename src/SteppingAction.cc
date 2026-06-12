#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4Ions.hh"

SteppingAction::SteppingAction(EventAction* eventAction, const DetectorConstruction* detector)
  : fEventAction(eventAction), fDetector(detector)
{}

bool SteppingAction::IsDetectorVolume(G4LogicalVolume* vol) const
{
    // LS-only accounting: realistic detector signal (PMTs see light
    // generated in the liquid scintillator volume only).
    return vol == fDetector->GetLSLogicalVolume();
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()
                                  ->GetVolume()->GetLogicalVolume();

    const G4Track* track = step->GetTrack();
    const G4ParticleDefinition* pdef = track->GetParticleDefinition();

    // ── Detect neutron capture (only relevant in LS) ────────────────────
    if (volume == fDetector->GetLSLogicalVolume() &&
        pdef->GetParticleName() == "neutron")
    {
        const G4VProcess* proc = step->GetPostStepPoint()->GetProcessDefinedStep();
        if (proc && proc->GetProcessName() == "nCapture") {
            G4ThreeVector capturePos = step->GetPostStepPoint()->GetPosition();
            G4double captureTime = step->GetPostStepPoint()->GetGlobalTime();

            G4int onGd = -1;
            const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();
            for (const G4Track* sec : *secondaries) {
                const G4ParticleDefinition* secDef = sec->GetParticleDefinition();
                G4String name = secDef->GetParticleName();
                if (name.find("Gd") != std::string::npos) {
                    onGd = 1;
                } else if (name == "deuteron" || name.find("H2") != std::string::npos) {
                    onGd = 0;
                }
            }
            if (onGd == -1) onGd = 0;

            fEventAction->RegisterCapture(captureTime, capturePos, onGd);
        }
    }

    // ── Energy deposit classification (LS + Vessel + Buffer) ────────────
    if (IsDetectorVolume(volume)) {
        G4double edep = step->GetTotalEnergyDeposit();
        G4double time = step->GetPostStepPoint()->GetGlobalTime() / ns;

        if (edep > 0.0) {
            if (time < kPromptWindow) {
                // Only count prompt energy from the LS itself (avoid
                // including buffer/vessel energy in the prompt signal)
                if (volume == fDetector->GetLSLogicalVolume()) {
                    fEventAction->AddPromptEdep(edep / MeV);
                }
            } else if (fEventAction->HasCapture()) {
                fEventAction->AddDelayedEdep(edep / MeV);
            }
        }
    }

    // ── Record IBD vertex ─────────────────────────────────────────────
    if (volume == fDetector->GetLSLogicalVolume() &&
        track->GetTrackID() == 1 && track->GetCurrentStepNumber() == 1)
    {
        fEventAction->SetIBDVertex(step->GetPreStepPoint()->GetPosition());
    }
}
