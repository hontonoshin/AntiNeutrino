#ifndef EVENT_ACTION_HH
#define EVENT_ACTION_HH

#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "PrimaryGeneratorAction.hh"

class G4Event;

/// Accumulates per-event quantities filled by SteppingAction:
///   - prompt energy deposit (within first ~100 ns: e+ track + annihilation gammas)
///   - delayed energy deposit (neutron capture gamma cascade)
///   - capture time and position (to compute Delta-t, Delta-R)
///   - capture isotope flag (Gd vs H)
///
/// At EndOfEventAction, fills the ROOT ntuple via G4AnalysisManager.
class EventAction : public G4UserEventAction
{
  public:
    EventAction(const PrimaryGeneratorAction* generator);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;
    
    const PrimaryGeneratorAction* fGenerator = nullptr;

    // ── Accessors used by SteppingAction to accumulate data ─────────────
    void AddPromptEdep(G4double edep)   { fPromptEdep   += edep; }
    void AddDelayedEdep(G4double edep)  { fDelayedEdep  += edep; }

    void SetIBDVertex(const G4ThreeVector& pos) { fIBDVertex = pos; fHasIBDVertex = true; }

    void RegisterCapture(G4double time, const G4ThreeVector& pos, G4int onGd);

    G4bool HasCapture() const { return fCaptureRecorded; }
    G4double GetTrueNeutrinoEnergy() const { return fTrueENu; }
    void SetTrueNeutrinoEnergy(G4double e) { fTrueENu = e; }
    G4double GetVertexRadius() const { return fHasIBDVertex ? fIBDVertex.mag() : -1.0; }

  private:
    G4double fPromptEdep  = 0.0; // MeV
    G4double fDelayedEdep = 0.0; // MeV

    G4double fCaptureTime = -1.0; // ns
    G4ThreeVector fCapturePos;
    G4int fCapturedOnGd = -1; // -1 = no capture, 0 = H, 1 = Gd
    G4bool fCaptureRecorded = false;

    G4ThreeVector fIBDVertex;
    G4bool fHasIBDVertex = false;

    G4double fTrueENu = 0.0; // MeV, set by PrimaryGeneratorAction via run-level pass-through
};

#endif
