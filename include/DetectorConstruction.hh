#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4Material;

/// Builds a simplified near-field antineutrino detector:
///   - Inner sphere: Gd-loaded liquid scintillator (LS) - active volume
///   - Acrylic vessel shell around the LS
///   - Outer sphere: water buffer / veto
///   - World: air
class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;

    // Accessors for logical volumes (used for hit identification in SD/SteppingAction)
    G4LogicalVolume* GetLSLogicalVolume()    const { return fLogicLS; }
    G4LogicalVolume* GetVesselLogicalVolume() const { return fLogicVessel; }
    G4LogicalVolume* GetBufferLogicalVolume() const { return fLogicBuffer; }

  private:
    void DefineMaterials();

    G4Material* fGdLS      = nullptr; // Gd-doped liquid scintillator
    G4Material* fAcrylic   = nullptr; // PMMA vessel
    G4Material* fWater     = nullptr; // buffer/veto

    G4LogicalVolume* fLogicLS     = nullptr;
    G4LogicalVolume* fLogicVessel = nullptr;
    G4LogicalVolume* fLogicBuffer = nullptr;


    G4double fLSRadius     = 50.0 * cm; // 50 cm diameter active volume
    G4double fVesselThick  = 1.0  * cm; // acrylic shell thickness
    G4double fBufferRadius = 70.0 * cm; // outer water buffer radius
};

#endif
