#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

EventAction::EventAction(const PrimaryGeneratorAction* generator)
  : fGenerator(generator)
{}

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    fPromptEdep  = 0.0;
    fDelayedEdep = 0.0;
    fCaptureTime = -1.0;
    fCapturePos  = G4ThreeVector();
    fCapturedOnGd = -1;
    fCaptureRecorded = false;
    fHasIBDVertex = false;
    fTrueENu = fGenerator->GetLastNeutrinoEnergy();
}

void EventAction::RegisterCapture(G4double time, const G4ThreeVector& pos, G4int onGd)
{
    if (fCaptureRecorded) return; // record only the first capture
    fCaptureTime = time;
    fCapturePos  = pos;
    fCapturedOnGd = onGd;
    fCaptureRecorded = true;
}

void EventAction::EndOfEventAction(const G4Event* /*event*/)
{
    G4double captureDR = -1.0;
    if (fCaptureRecorded && fHasIBDVertex) {
        captureDR = (fCapturePos - fIBDVertex).mag() / cm;
    }

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(0, fPromptEdep);
    man->FillNtupleDColumn(1, fDelayedEdep);
    man->FillNtupleDColumn(2, fCaptureRecorded ? fCaptureTime / microsecond : -1.0);
    man->FillNtupleDColumn(3, captureDR);
    man->FillNtupleIColumn(4, fCapturedOnGd);
    man->FillNtupleDColumn(5, fTrueENu);
    man->FillNtupleDColumn(6, fIBDVertex.mag() / cm); // vertexR, cm
    man->AddNtupleRow();
}
