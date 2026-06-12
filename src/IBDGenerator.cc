#include "IBDGenerator.hh"

#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <cmath>

IBDGenerator::IBDGenerator()
{}

G4double IBDGenerator::ReactorFluxU235(G4double eNu) const
{
    // Huber (2011) / Mueller et al. polynomial parametrization:
    //   phi(E) = exp(a0 + a1*E + a2*E^2 + a3*E^3 + a4*E^4 + a5*E^5)
    // Coefficients for U-235 (E in MeV), commonly tabulated form.
    static const G4double a0 =  4.367;
    static const G4double a1 = -4.577;
    static const G4double a2 =  2.100;
    static const G4double a3 = -0.5294;
    static const G4double a4 =  0.06186;
    static const G4double a5 = -0.002777;

    G4double e  = eNu;
    G4double e2 = e * e;
    G4double e3 = e2 * e;
    G4double e4 = e3 * e;
    G4double e5 = e4 * e;

    G4double exponent = a0 + a1*e + a2*e2 + a3*e3 + a4*e4 + a5*e5;
    return std::exp(exponent);
}

G4double IBDGenerator::IBDCrossSection(G4double eNu) const
{
    // Simplified leading-order IBD cross-section energy dependence
    // (Strumia-Vissani / Vogel-Beacom approximation):
    //   sigma(E) ~ E_e * p_e
    // where E_e = E_nu - Delta (positron total energy) and
    //   p_e = sqrt(E_e^2 - m_e^2)
    // Returns 0 below threshold.

    if (eNu < kEThresh) return 0.0;

    G4double Ee = eNu - kDelta; // positron total energy
    if (Ee <= kMe) return 0.0;

    G4double pe = std::sqrt(Ee*Ee - kMe*kMe);
    return Ee * pe;
}

G4double IBDGenerator::SampleNeutrinoEnergy() const
{
    // Build the (unnormalized) event spectrum = reactor flux * IBD cross-section,
    // then rejection-sample from it.

    // Find an approximate maximum of flux*xsec over [fEMin, fEMax] for rejection envelope.
    G4double maxVal = 0.0;
    const int nScan = 200;
    for (int i = 0; i <= nScan; ++i) {
        G4double e = fEMin + (fEMax - fEMin) * i / nScan;
        G4double val = ReactorFluxU235(e) * IBDCrossSection(e);
        if (val > maxVal) maxVal = val;
    }
    maxVal *= 1.05; // small safety margin

    // Rejection sampling
    for (int attempt = 0; attempt < 10000; ++attempt) {
        G4double e = fEMin + (fEMax - fEMin) * G4UniformRand();
        G4double val = ReactorFluxU235(e) * IBDCrossSection(e);
        G4double y = maxVal * G4UniformRand();
        if (y <= val) {
            return e;
        }
    }

    // Fallback (should not normally happen)
    return 3.0;
}

G4double IBDGenerator::PositronKineticEnergy(G4double eNu) const
{
    // Leading-order: E_e (total) = E_nu - Delta
    G4double Ee = eNu - kDelta;
    if (Ee < kMe) Ee = kMe; // guard against unphysical sub-threshold values

    G4double Te = Ee - kMe; // kinetic energy
    return Te;
}

G4ThreeVector IBDGenerator::SampleDirection() const
{
    // Isotropic direction sampling
    G4double cosTheta = 2.0 * G4UniformRand() - 1.0;
    G4double sinTheta = std::sqrt(1.0 - cosTheta*cosTheta);
    G4double phi = CLHEP::twopi * G4UniformRand();

    return G4ThreeVector(sinTheta * std::cos(phi),
                          sinTheta * std::sin(phi),
                          cosTheta);
}

G4ThreeVector IBDGenerator::SampleVertexInSphere(G4double radius) const
{
    // Uniform sampling within a sphere via rejection in a cube
    G4double x, y, z;
    do {
        x = (2.0 * G4UniformRand() - 1.0) * radius;
        y = (2.0 * G4UniformRand() - 1.0) * radius;
        z = (2.0 * G4UniformRand() - 1.0) * radius;
    } while (x*x + y*y + z*z > radius*radius);

    return G4ThreeVector(x, y, z);
}
