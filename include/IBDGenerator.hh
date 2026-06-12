#ifndef IBD_GENERATOR_HH
#define IBD_GENERATOR_HH

#include "globals.hh"
#include "G4ThreeVector.hh"

/// Samples a reactor antineutrino energy from a Huber-Mueller-style
/// parametrization (U-235 fission spectrum), applies the inverse beta
/// decay (IBD) cross-section as a sampling weight, and computes the
/// resulting positron kinetic energy and emission direction via the
/// Strumia-Vissani leading-order kinematics.
///
/// Reaction: nu_e_bar + p -> e+ + n
class IBDGenerator
{
  public:
    IBDGenerator();
    ~IBDGenerator() = default;

    /// Sample an antineutrino energy (MeV) from the reactor spectrum,
    /// weighted by the IBD cross-section, using rejection sampling.
    G4double SampleNeutrinoEnergy() const;

    /// Given E_nu (MeV), return the positron total kinetic energy (MeV)
    /// using the leading-order (delta-function) approximation:
    ///   E_e ~ E_nu - DELTA, with DELTA = m_n - m_p = 1.293 MeV
    /// and rest mass subtracted to give kinetic energy.
    G4double PositronKineticEnergy(G4double eNu) const;

    /// Sample an isotropic random direction (positron emission angle is
    /// only weakly correlated with E_nu at these energies; isotropic is
    /// a reasonable first-order approximation for this simplified study).
    G4ThreeVector SampleDirection() const;

    /// Sample a random vertex position uniformly inside a sphere of
    /// given radius (cm, returned in G4 length units).
    G4ThreeVector SampleVertexInSphere(G4double radius) const;

  private:
    /// Huber-Mueller-style polynomial parametrization for the U-235
    /// reactor antineutrino flux spectrum (per fission, per MeV).
    /// Valid roughly over 1.8 - 8.0 MeV.
    G4double ReactorFluxU235(G4double eNu) const;

    /// Strumia-Vissani IBD cross-section, simplified leading-order
    /// energy dependence (relative shape only, used as sampling weight).
    G4double IBDCrossSection(G4double eNu) const;

    // Physical constants (MeV)
    static constexpr G4double kDelta   = 1.293;   // m_n - m_p
    static constexpr G4double kMe      = 0.511;   // electron mass
    static constexpr G4double kEThresh = 1.806;   // IBD threshold energy

    // Sampling range
    G4double fEMin = 1.806; // MeV, IBD threshold
    G4double fEMax = 10.0;  // MeV
};

#endif
