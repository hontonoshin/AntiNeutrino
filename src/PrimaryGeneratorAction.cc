#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4UserEventAction.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(const DetectorConstruction* detector)
  : fDetector(detector)
{
    fParticleGun = new G4ParticleGun(1);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    // ── Sample IBD event kinematics ─────────────────────────────────────
    G4double eNu = fIBDGen.SampleNeutrinoEnergy();  
    fLastENu = eNu;
    G4double Te  = fIBDGen.PositronKineticEnergy(eNu);          // MeV, positron KE

    // Random vertex inside the LS sphere (radius set to match
    // DetectorConstruction's LS radius)
    const G4double lsRadius = 50.0 * cm; // keep in sync with DetectorConstruction
    G4ThreeVector vertex = fIBDGen.SampleVertexInSphere(lsRadius);

    // Isotropic direction for the positron
    G4ThreeVector eDir = fIBDGen.SampleDirection();

    // ── Fire the positron (prompt signal) ───────────────────────────────
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* positron = particleTable->FindParticle("e+");

    fParticleGun->SetParticleDefinition(positron);
    fParticleGun->SetParticlePosition(vertex);
    fParticleGun->SetParticleMomentumDirection(eDir);
    fParticleGun->SetParticleEnergy(Te);
    fParticleGun->GeneratePrimaryVertex(event);

    // ── Fire the neutron (delayed signal precursor) ─────────────────────
    // The recoil neutron from IBD typically has a small kinetic energy
    // (tens of keV), nearly opposite direction to the positron at
    // leading order. We approximate with a fixed small KE and
    // back-to-back direction; Geant4/FTFP_BERT_HP then handles its
    // thermalization and capture (on Gd or H) naturally.
    G4ParticleDefinition* neutron = particleTable->FindParticle("neutron");

    G4double neutronKE = 0.02 * MeV; // ~20 keV typical recoil scale
    G4ThreeVector nDir = -eDir;       // approx back-to-back

    fParticleGun->SetParticleDefinition(neutron);
    fParticleGun->SetParticlePosition(vertex);
    fParticleGun->SetParticleMomentumDirection(nDir);
    fParticleGun->SetParticleEnergy(neutronKE);
    fParticleGun->GeneratePrimaryVertex(event);

   
}
