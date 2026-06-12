#ifndef PHYSICS_LIST_HH
#define PHYSICS_LIST_HH

#include "FTFP_BERT_HP.hh"

/// Physics list based on FTFP_BERT_HP.
///
/// The _HP (High Precision) neutron physics is essential here: it
/// provides accurate thermal neutron transport and radiative capture
/// cross-sections (n + Gd -> gamma cascade ~8 MeV, n + H -> 2.2 MeV gamma),
/// which form the delayed-coincidence signal in this study.
///
/// FTFP_BERT_HP already includes:
///   - EM physics (G4EmStandardPhysics)
///   - Hadronic physics (FTFP for high energy, Bertini for intermediate)
///   - High-precision neutron transport/capture (G4HadronPhysicsFTFP_BERT_HP)
///   - Decay and radioactive decay physics
///
/// We subclass it directly and additionally register
/// G4RadioactiveDecayPhysics to ensure excited-nucleus de-excitation
/// gammas following neutron capture (e.g. Gd-158* -> Gd-158 + gammas)
/// are fully tracked.
class PhysicsList : public FTFP_BERT_HP
{
  public:
    PhysicsList();
    ~PhysicsList() override = default;
};

#endif
