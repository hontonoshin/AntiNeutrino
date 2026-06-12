#include "PhysicsList.hh"
#include "G4RadioactiveDecayPhysics.hh"

PhysicsList::PhysicsList() : FTFP_BERT_HP()
{
    // FTFP_BERT_HP already registers EM, hadronic (FTFP/BERT + HP neutron),
    // and decay physics. Add radioactive decay physics on top to ensure
    // de-excitation gamma cascades from neutron capture on Gd/H are
    // fully simulated.
    RegisterPhysics(new G4RadioactiveDecayPhysics());
}
