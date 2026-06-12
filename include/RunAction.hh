#ifndef RUN_ACTION_HH
#define RUN_ACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Sets up the ROOT output file and ntuple structure using
/// G4AnalysisManager. Columns store, per event:
///   - promptEdep   : energy deposited in LS by e+ + annihilation gammas (MeV)
///   - delayedEdep  : energy deposited in LS by neutron capture gamma(s) (MeV)
///   - captureTime  : time of neutron capture relative to event start (ns)
///   - captureDR    : distance between IBD vertex and capture vertex (cm)
///   - capturedOnGd : 1 if capture occurred on Gd, 0 if on H, -1 if no capture
class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;
};

#endif
