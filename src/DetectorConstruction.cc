#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(),
      fCfg(5), // <<< BURADAN CONFIG SEÇ
      fThickness(5.0 * cm),
      fShieldMaterialName("")
{
}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    auto nist = G4NistManager::Instance();

    // ================= WORLD =================
    G4double worldSize = 2.0 * m;
    auto worldMat = nist->FindOrBuildMaterial("G4_AIR");

    auto solidWorld = new G4Box("World", worldSize, worldSize, worldSize);
    auto logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");

    auto physWorld = new G4PVPlacement(
        nullptr, {}, logicWorld,
        "World", nullptr, false, 0, true);

    // ================= MATERIALS =================
    auto water = nist->FindOrBuildMaterial("G4_WATER");
    auto hdpe = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
    auto concrete = nist->FindOrBuildMaterial("G4_CONCRETE");
    auto aluminum = nist->FindOrBuildMaterial("G4_Al");
    auto lead = nist->FindOrBuildMaterial("G4_Pb");

    auto melaninFungi =
        nist->BuildMaterialWithNewDensity(
            "MelaninFungi",
            "G4_TISSUE_SOFT_ICRP",
            1.22 * g / cm3);

    const G4double XY = 30 * cm;
    G4double zPos = 0.0;

    // ================= CONFIG SWITCH =================

    if (fCfg == 0) // Single fungus
    {
        auto solid = new G4Box("Fungus", XY, XY, fThickness / 2);
        auto lv = new G4LogicalVolume(solid, melaninFungi, "FungusLV");

        new G4PVPlacement(nullptr, {0, 0, zPos},
                          lv, "FungusPV", logicWorld, false, 0, true);

        fShieldMaterialName = "Single MelaninFungi";
    }

    else if (fCfg == 1) // HDPE | Fungus | HDPE
    {
        G4double side = 1.0 * cm;
        G4double mid = fThickness;

        auto s1 = new G4Box("HDPE1", XY, XY, side / 2);
        auto lv1 = new G4LogicalVolume(s1, hdpe, "HDPE1LV");
        new G4PVPlacement(nullptr, {0, 0, zPos - mid / 2 - side / 2},
                          lv1, "HDPE1PV", logicWorld, false, 0, true);

        auto s2 = new G4Box("Fungus", XY, XY, mid / 2);
        auto lv2 = new G4LogicalVolume(s2, melaninFungi, "FungusLV");
        new G4PVPlacement(nullptr, {0, 0, zPos},
                          lv2, "FungusPV", logicWorld, false, 0, true);

        auto s3 = new G4Box("HDPE2", XY, XY, side / 2);
        auto lv3 = new G4LogicalVolume(s3, hdpe, "HDPE2LV");
        new G4PVPlacement(nullptr, {0, 0, zPos + mid / 2 + side / 2},
                          lv3, "HDPE2PV", logicWorld, false, 0, true);

        fShieldMaterialName = "HDPE-Fungus-HDPE";
    }

    else if (fCfg == 2) // Aluminum | Fungus | Aluminum
    {
        G4double side = 2.0 * mm;

        auto s1 = new G4Box("Al1", XY, XY, side / 2);
        auto lv1 = new G4LogicalVolume(s1, aluminum, "Al1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - side / 2},
                          lv1, "Al1PV", logicWorld, false, 0, true);

        auto s2 = new G4Box("Fungus", XY, XY, fThickness / 2);
        auto lv2 = new G4LogicalVolume(s2, melaninFungi, "FungusLV");
        new G4PVPlacement(nullptr, {0, 0, 0},
                          lv2, "FungusPV", logicWorld, false, 0, true);

        auto s3 = new G4Box("Al2", XY, XY, side / 2);
        auto lv3 = new G4LogicalVolume(s3, aluminum, "Al2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + side / 2},
                          lv3, "Al2PV", logicWorld, false, 0, true);

        fShieldMaterialName = "Al-Fungus-Al";
    }

    else if (fCfg == 4) // Al | HDPE | Fungus | HDPE | Al
    {
        G4double tAl = 2.0 * mm;
        G4double tHd = 1.0 * cm;

        G4double z = 0;

        auto Al1 = new G4LogicalVolume(
            new G4Box("Al1", XY, XY, tAl / 2),
            aluminum, "Al1LV");
        new G4PVPlacement(nullptr, {0, 0, z - fThickness / 2 - tHd - tAl / 2},
                          Al1, "Al1PV", logicWorld, false, 0, true);

        auto Hd1 = new G4LogicalVolume(
            new G4Box("Hd1", XY, XY, tHd / 2),
            hdpe, "Hd1LV");
        new G4PVPlacement(nullptr, {0, 0, z - fThickness / 2 - tHd / 2},
                          Hd1, "Hd1PV", logicWorld, false, 0, true);

        auto Fung = new G4LogicalVolume(
            new G4Box("Fung", XY, XY, fThickness / 2),
            melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, z},
                          Fung, "FungPV", logicWorld, false, 0, true);

        auto Hd2 = new G4LogicalVolume(
            new G4Box("Hd2", XY, XY, tHd / 2),
            hdpe, "Hd2LV");
        new G4PVPlacement(nullptr, {0, 0, z + fThickness / 2 + tHd / 2},
                          Hd2, "Hd2PV", logicWorld, false, 0, true);

        auto Al2 = new G4LogicalVolume(
            new G4Box("Al2", XY, XY, tAl / 2),
            aluminum, "Al2LV");
        new G4PVPlacement(nullptr, {0, 0, z + fThickness / 2 + tHd + tAl / 2},
                          Al2, "Al2PV", logicWorld, false, 0, true);

        fShieldMaterialName = "Al-HDPE-Fungus-HDPE-Al";
    }
    else if (fCfg == 5) // Lead | Fungus | Lead
    {
        G4double tLead = 5.0 * mm;

        auto lead = nist->FindOrBuildMaterial("G4_Pb");

        auto Pb1 = new G4LogicalVolume(
            new G4Box("Pb1", XY, XY, tLead / 2),
            lead, "Pb1LV");

        new G4PVPlacement(nullptr,
                          {0, 0, -fThickness / 2 - tLead / 2},
                          Pb1,
                          "Pb1PV",
                          logicWorld,
                          false,
                          0,
                          true);

        auto Fung = new G4LogicalVolume(
            new G4Box("Fung", XY, XY, fThickness / 2),
            melaninFungi,
            "FungLV");

        new G4PVPlacement(nullptr,
                          {0, 0, 0},
                          Fung,
                          "FungPV",
                          logicWorld,
                          false,
                          0,
                          true);

        auto Pb2 = new G4LogicalVolume(
            new G4Box("Pb2", XY, XY, tLead / 2),
            lead, "Pb2LV");

        new G4PVPlacement(nullptr,
                          {0, 0, fThickness / 2 + tLead / 2},
                          Pb2,
                          "Pb2PV",
                          logicWorld,
                          false,
                          0,
                          true);

        fShieldMaterialName = "Pb-Fungus-Pb";
    }

    // ================= DETECTOR =================
    auto detMat = nist->FindOrBuildMaterial("G4_WATER");
    auto detThickness = 1.0 * cm;

    auto solidDetector =
        new G4Box("Detector", XY, XY, detThickness / 2);

    auto logicDetector =
        new G4LogicalVolume(solidDetector,
                            detMat,
                            "Detector");

    new G4PVPlacement(nullptr,
                      {0, 0, fThickness + 20 * cm},
                      logicDetector,
                      "DetectorPV",
                      logicWorld,
                      false,
                      0,
                      true);

    G4cout << "\n===== CONFIG: " << fShieldMaterialName
           << " | Core thickness: "
           << fThickness / cm << " cm =====\n";

    return physWorld;
}