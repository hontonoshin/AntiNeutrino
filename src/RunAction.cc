#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction()
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->SetVerboseLevel(0);
    man->SetNtupleMerging(true);

    // Create ntuple "antinu" with one row per event
    man->CreateNtuple("antinu", "IBD prompt/delayed signal data");
    man->CreateNtupleDColumn("promptEdep");   // 0: MeV
    man->CreateNtupleDColumn("delayedEdep");  // 1: MeV
    man->CreateNtupleDColumn("captureTime");  // 2: ns
    man->CreateNtupleDColumn("captureDR");    // 3: cm
    man->CreateNtupleIColumn("capturedOnGd"); // 4: 1=Gd, 0=H, -1=none
    man->CreateNtupleDColumn("eNuTrue");      // 5: true sampled E_nu (MeV)
    man->CreateNtupleDColumn("vertexR");      // 6: IBD vertex radius (cm)
    man->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    G4String fileName = "antinu_output.root";
    man->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();
}
