#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

DetectorConstruction::DetectorConstruction()
{}

void DetectorConstruction::DefineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();

    // ── Water (buffer/veto) ─────────────────────────────────────────────
    fWater = nist->FindOrBuildMaterial("G4_WATER");

    // ── Acrylic vessel (PMMA) ───────────────────────────────────────────
    fAcrylic = nist->FindOrBuildMaterial("G4_PLEXIGLASS"); // PMMA

    // ── Gd-doped Liquid Scintillator ────────────────────────────────────
    // Approximate as LAB (linear alkylbenzene, C18H30) base doped with
    // ~0.1% Gd by mass (as Gd-carboxylate complex, simplified here as
    // elemental Gd loading mixed into the LAB matrix).
    //
    // Step 1: build base LAB material (C18H30), density ~0.86 g/cm3
    G4Element* elC = nist->FindOrBuildElement("C");
    G4Element* elH = nist->FindOrBuildElement("H");
    G4Element* elGd = nist->FindOrBuildElement("Gd");

    G4double labDensity = 0.86 * g/cm3;
    G4Material* lab = new G4Material("LAB", labDensity, 2);
    lab->AddElement(elC, 18);
    lab->AddElement(elH, 30);

    // Step 2: dope with 0.1% Gd by mass -> final GdLS material
    // Final density approximated as slightly higher than pure LAB.
    G4double gdMassFraction = 0.001; // 0.1%
    G4double gdLSDensity = 0.862 * g/cm3;

    fGdLS = new G4Material("GdLS", gdLSDensity, 2);
    fGdLS->AddMaterial(lab, 1.0 - gdMassFraction);
    fGdLS->AddElement(elGd, gdMassFraction);
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    DefineMaterials();

    // ── World ────────────────────────────────────────────────────────────
    G4double worldSize = 1.2 * fBufferRadius;
    G4Material* worldMat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    G4Box* solidWorld = new G4Box("World", worldSize, worldSize, worldSize);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(
        nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0);
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

    // ── Water buffer (outer sphere) ─────────────────────────────────────
    G4Sphere* solidBuffer = new G4Sphere(
        "Buffer", 0., fBufferRadius, 0., 360.*deg, 0., 180.*deg);
    fLogicBuffer = new G4LogicalVolume(solidBuffer, fWater, "Buffer");
    new G4PVPlacement(nullptr, G4ThreeVector(), fLogicBuffer, "Buffer",
                      logicWorld, false, 0);

    G4VisAttributes* bufferVis = new G4VisAttributes(G4Colour(0.3, 0.5, 1.0, 0.15));
    bufferVis->SetForceSolid(false);
    fLogicBuffer->SetVisAttributes(bufferVis);

    // ── Acrylic vessel (outer radius = LS radius + vessel thickness) ────
    G4double vesselOuterR = fLSRadius + fVesselThick;
    G4Sphere* solidVessel = new G4Sphere(
        "Vessel", 0., vesselOuterR, 0., 360.*deg, 0., 180.*deg);
    fLogicVessel = new G4LogicalVolume(solidVessel, fAcrylic, "Vessel");
    new G4PVPlacement(nullptr, G4ThreeVector(), fLogicVessel, "Vessel",
                      fLogicBuffer, false, 0);

    G4VisAttributes* vesselVis = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.3));
    vesselVis->SetForceSolid(false);
    fLogicVessel->SetVisAttributes(vesselVis);

    // ── Gd-LS active volume (innermost sphere) ──────────────────────────
    G4Sphere* solidLS = new G4Sphere(
        "LS", 0., fLSRadius, 0., 360.*deg, 0., 180.*deg);
    fLogicLS = new G4LogicalVolume(solidLS, fGdLS, "LS");
    new G4PVPlacement(nullptr, G4ThreeVector(), fLogicLS, "LS",
                       fLogicVessel, false, 0);

    G4VisAttributes* lsVis = new G4VisAttributes(G4Colour(1.0, 0.8, 0.0, 0.4));
    lsVis->SetForceSolid(true);
    fLogicLS->SetVisAttributes(lsVis);

    return physWorld;
}
