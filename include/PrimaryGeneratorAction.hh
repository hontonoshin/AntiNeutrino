#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "IBDGenerator.hh"

class G4ParticleGun;
class G4Event;
class DetectorConstruction;

/// Generates IBD final-state primaries directly:
///   - e+ with kinetic energy from IBDGenerator, at a random vertex
///     inside the LS volume, isotropic direction
///   - a free neutron with a small (~few keV - sub MeV recoil) kinetic
///     energy, emitted back-to-back (approx.) from the same vertex
///
/// Note: Geant4 does not transport neutrinos, so the IBD reaction itself
/// is not simulated as a physics process. Instead this generator produces
/// the IBD *products* (e+ and n) with kinematics sampled from the
/// reactor antineutrino spectrum + IBD cross-section, which is the
/// standard approach for this type of study.
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(const DetectorConstruction* detector);
    ~PrimaryGeneratorAction() override;
    G4double GetLastNeutrinoEnergy() const { return fLastENu; }

    void GeneratePrimaries(G4Event* event) override;

  private:
    G4ParticleGun* fParticleGun = nullptr;
    const DetectorConstruction* fDetector = nullptr;
    IBDGenerator fIBDGen;
    G4double fLastENu = 0.0;
};

#endif
