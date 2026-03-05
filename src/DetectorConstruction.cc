#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(),
      fCfg(7), // <<< BURADAN CONFIG SEC (0-15)
      fThickness(5.0 * cm),
      fWaterThickness(2.0 * cm),
      fShieldMaterialName("")
{
}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    // ================= GEOMETRY CLEANUP (multi-run icin) =================
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    auto nist = G4NistManager::Instance();

    // ================= WORLD =================
    G4double worldSize = 2.0 * m;
    auto worldMat = nist->FindOrBuildMaterial("G4_AIR");
    auto solidWorld = new G4Box("World", worldSize, worldSize, worldSize);
    auto logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
    auto physWorld = new G4PVPlacement(nullptr, {}, logicWorld,
                                       "World", nullptr, false, 0, true);

    // ================= MATERIALS =================
    auto water = nist->FindOrBuildMaterial("G4_WATER");
    auto hdpe = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
    auto concrete = nist->FindOrBuildMaterial("G4_CONCRETE");
    auto aluminum = nist->FindOrBuildMaterial("G4_Al");
    auto lead = nist->FindOrBuildMaterial("G4_Pb");
    auto steel = nist->FindOrBuildMaterial("G4_Fe");

    // MelaninFungi: material store temizlenmez, sadece yoksa yarat
    auto melaninFungi = G4Material::GetMaterial("MelaninFungi");
    if (!melaninFungi)
        melaninFungi = nist->BuildMaterialWithNewDensity(
            "MelaninFungi", "G4_TISSUE_SOFT_ICRP", 1.22 * g / cm3);

    const G4double XY = 30 * cm;
    const G4double detThickness = 1.0 * cm;
    const G4double detGap = 5.0 * cm;

    G4double zBackFace = 0.0;

    // ================= CONFIG SWITCH =================

    if (fCfg == 0) // ---- Tek Fungus ----
    {
        auto lv = new G4LogicalVolume(
            new G4Box("Fungus", XY, XY, fThickness / 2), melaninFungi, "FungusLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lv, "FungusPV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2;
        fShieldMaterialName = "Single-MelaninFungi";
    }

    else if (fCfg == 1) // ---- HDPE | Fungus | HDPE ----
    {
        G4double tHd = 1.0 * cm;
        auto lv1 = new G4LogicalVolume(new G4Box("HDPE1", XY, XY, tHd / 2), hdpe, "HDPE1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - tHd / 2}, lv1, "HDPE1PV", logicWorld, false, 0, true);
        auto lv2 = new G4LogicalVolume(new G4Box("Fungus", XY, XY, fThickness / 2), melaninFungi, "FungusLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lv2, "FungusPV", logicWorld, false, 0, true);
        auto lv3 = new G4LogicalVolume(new G4Box("HDPE2", XY, XY, tHd / 2), hdpe, "HDPE2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tHd / 2}, lv3, "HDPE2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + tHd;
        fShieldMaterialName = "HDPE-Fungus-HDPE";
    }

    else if (fCfg == 2) // ---- Al | Fungus | Al ----
    {
        G4double tAl = 2.0 * mm;
        auto lv1 = new G4LogicalVolume(new G4Box("Al1", XY, XY, tAl / 2), aluminum, "Al1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - tAl / 2}, lv1, "Al1PV", logicWorld, false, 0, true);
        auto lv2 = new G4LogicalVolume(new G4Box("Fungus", XY, XY, fThickness / 2), melaninFungi, "FungusLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lv2, "FungusPV", logicWorld, false, 0, true);
        auto lv3 = new G4LogicalVolume(new G4Box("Al2", XY, XY, tAl / 2), aluminum, "Al2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tAl / 2}, lv3, "Al2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + tAl;
        fShieldMaterialName = "Al-Fungus-Al";
    }

    else if (fCfg == 3) // ---- Concrete | Fungus | Concrete ----
    {
        G4double tConc = 2.0 * cm;
        auto lv1 = new G4LogicalVolume(new G4Box("Conc1", XY, XY, tConc / 2), concrete, "Conc1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - tConc / 2}, lv1, "Conc1PV", logicWorld, false, 0, true);
        auto lv2 = new G4LogicalVolume(new G4Box("Fungus", XY, XY, fThickness / 2), melaninFungi, "FungusLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lv2, "FungusPV", logicWorld, false, 0, true);
        auto lv3 = new G4LogicalVolume(new G4Box("Conc2", XY, XY, tConc / 2), concrete, "Conc2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tConc / 2}, lv3, "Conc2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + tConc;
        fShieldMaterialName = "Concrete-Fungus-Concrete";
    }

    else if (fCfg == 4) // ---- Al | HDPE | Fungus | HDPE | Al ----
    {
        G4double tAl = 2.0 * mm;
        G4double tHd = 1.0 * cm;
        auto Al1 = new G4LogicalVolume(new G4Box("Al1", XY, XY, tAl / 2), aluminum, "Al1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - tHd - tAl / 2}, Al1, "Al1PV", logicWorld, false, 0, true);
        auto Hd1 = new G4LogicalVolume(new G4Box("Hd1", XY, XY, tHd / 2), hdpe, "Hd1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - tHd / 2}, Hd1, "Hd1PV", logicWorld, false, 0, true);
        auto Fung = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, Fung, "FungPV", logicWorld, false, 0, true);
        auto Hd2 = new G4LogicalVolume(new G4Box("Hd2", XY, XY, tHd / 2), hdpe, "Hd2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tHd / 2}, Hd2, "Hd2PV", logicWorld, false, 0, true);
        auto Al2 = new G4LogicalVolume(new G4Box("Al2", XY, XY, tAl / 2), aluminum, "Al2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tHd + tAl / 2}, Al2, "Al2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + tHd + tAl;
        fShieldMaterialName = "Al-HDPE-Fungus-HDPE-Al";
    }

    else if (fCfg == 5) // ---- Pb | Fungus | Pb ----
    {
        G4double tPb = 5.0 * mm;
        auto lv1 = new G4LogicalVolume(new G4Box("Pb1", XY, XY, tPb / 2), lead, "Pb1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - tPb / 2}, lv1, "Pb1PV", logicWorld, false, 0, true);
        auto lv2 = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lv2, "FungPV", logicWorld, false, 0, true);
        auto lv3 = new G4LogicalVolume(new G4Box("Pb2", XY, XY, tPb / 2), lead, "Pb2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tPb / 2}, lv3, "Pb2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + tPb;
        fShieldMaterialName = "Pb-Fungus-Pb";
    }

    else if (fCfg == 6) // ---- HDPE | Water | Fungus | HDPE ----
    {
        G4double tHd = 1.0 * cm;
        auto lvHd1 = new G4LogicalVolume(new G4Box("HDPE1", XY, XY, tHd / 2), hdpe, "HDPE1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tHd / 2}, lvHd1, "HDPE1PV", logicWorld, false, 0, true);
        auto lvWat = new G4LogicalVolume(new G4Box("Water", XY, XY, fWaterThickness / 2), water, "WaterLV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness / 2}, lvWat, "WaterPV", logicWorld, false, 0, true);
        auto lvFung = new G4LogicalVolume(new G4Box("Fungus", XY, XY, fThickness / 2), melaninFungi, "FungusLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvFung, "FungusPV", logicWorld, false, 0, true);
        auto lvHd2 = new G4LogicalVolume(new G4Box("HDPE2", XY, XY, tHd / 2), hdpe, "HDPE2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tHd / 2}, lvHd2, "HDPE2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + tHd;
        fShieldMaterialName = "HDPE-Water-Fungus-HDPE";
    }

    else if (fCfg == 7) // ---- Al | Water | Fungus | Al ----
    {
        G4double tAl = 2.0 * mm;
        auto lvAl1 = new G4LogicalVolume(new G4Box("Al1", XY, XY, tAl / 2), aluminum, "Al1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tAl / 2}, lvAl1, "Al1PV", logicWorld, false, 0, true);
        auto lvWat = new G4LogicalVolume(new G4Box("Water", XY, XY, fWaterThickness / 2), water, "WaterLV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness / 2}, lvWat, "WaterPV", logicWorld, false, 0, true);
        auto lvFung = new G4LogicalVolume(new G4Box("Fungus", XY, XY, fThickness / 2), melaninFungi, "FungusLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvFung, "FungusPV", logicWorld, false, 0, true);
        auto lvAl2 = new G4LogicalVolume(new G4Box("Al2", XY, XY, tAl / 2), aluminum, "Al2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tAl / 2}, lvAl2, "Al2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + tAl;
        fShieldMaterialName = "Al-Water-Fungus-Al";
    }

    else if (fCfg == 8) // ---- Pb Only (baseline) ----
    {
        auto lv = new G4LogicalVolume(new G4Box("PbOnly", XY, XY, fThickness / 2), lead, "PbOnlyLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lv, "PbOnlyPV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2;
        fShieldMaterialName = "Pb-Only";
    }

    else if (fCfg == 9) // ---- Al | Water | Fungus | Water | Al ----
    {
        G4double tAl = 2.0 * mm;
        auto lvAl1 = new G4LogicalVolume(new G4Box("Al1", XY, XY, tAl / 2), aluminum, "Al1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tAl / 2}, lvAl1, "Al1PV", logicWorld, false, 0, true);
        auto lvW1 = new G4LogicalVolume(new G4Box("Wat1", XY, XY, fWaterThickness / 2), water, "Wat1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness / 2}, lvW1, "Wat1PV", logicWorld, false, 0, true);
        auto lvF = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvF, "FungPV", logicWorld, false, 0, true);
        auto lvW2 = new G4LogicalVolume(new G4Box("Wat2", XY, XY, fWaterThickness / 2), water, "Wat2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness / 2}, lvW2, "Wat2PV", logicWorld, false, 0, true);
        auto lvAl2 = new G4LogicalVolume(new G4Box("Al2", XY, XY, tAl / 2), aluminum, "Al2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness + tAl / 2}, lvAl2, "Al2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + fWaterThickness + tAl;
        fShieldMaterialName = "Al-Water-Fungus-Water-Al";
    }

    else if (fCfg == 10) // ---- Concrete | Water | Fungus | Water | Concrete ----
    {
        G4double tConc = 2.0 * cm;
        auto lvC1 = new G4LogicalVolume(new G4Box("Conc1", XY, XY, tConc / 2), concrete, "Conc1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tConc / 2}, lvC1, "Conc1PV", logicWorld, false, 0, true);
        auto lvW1 = new G4LogicalVolume(new G4Box("Wat1", XY, XY, fWaterThickness / 2), water, "Wat1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness / 2}, lvW1, "Wat1PV", logicWorld, false, 0, true);
        auto lvF = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvF, "FungPV", logicWorld, false, 0, true);
        auto lvW2 = new G4LogicalVolume(new G4Box("Wat2", XY, XY, fWaterThickness / 2), water, "Wat2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness / 2}, lvW2, "Wat2PV", logicWorld, false, 0, true);
        auto lvC2 = new G4LogicalVolume(new G4Box("Conc2", XY, XY, tConc / 2), concrete, "Conc2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness + tConc / 2}, lvC2, "Conc2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + fWaterThickness + tConc;
        fShieldMaterialName = "Concrete-Water-Fungus-Water-Concrete";
    }

    else if (fCfg == 11) // ---- Pb | Water | Fungus | Water | Pb ----
    {
        G4double tPb = 5.0 * mm;
        auto lvPb1 = new G4LogicalVolume(new G4Box("Pb1", XY, XY, tPb / 2), lead, "Pb1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tPb / 2}, lvPb1, "Pb1PV", logicWorld, false, 0, true);
        auto lvW1 = new G4LogicalVolume(new G4Box("Wat1", XY, XY, fWaterThickness / 2), water, "Wat1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness / 2}, lvW1, "Wat1PV", logicWorld, false, 0, true);
        auto lvF = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvF, "FungPV", logicWorld, false, 0, true);
        auto lvW2 = new G4LogicalVolume(new G4Box("Wat2", XY, XY, fWaterThickness / 2), water, "Wat2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness / 2}, lvW2, "Wat2PV", logicWorld, false, 0, true);
        auto lvPb2 = new G4LogicalVolume(new G4Box("Pb2", XY, XY, tPb / 2), lead, "Pb2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness + tPb / 2}, lvPb2, "Pb2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + fWaterThickness + tPb;
        fShieldMaterialName = "Pb-Water-Fungus-Water-Pb";
    }

    else if (fCfg == 12) // ---- Steel | Fungus | Steel ----
    {
        G4double tFe = 2.0 * mm;
        auto lvFe1 = new G4LogicalVolume(new G4Box("Fe1", XY, XY, tFe / 2), steel, "Fe1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - tFe / 2}, lvFe1, "Fe1PV", logicWorld, false, 0, true);
        auto lvF = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvF, "FungPV", logicWorld, false, 0, true);
        auto lvFe2 = new G4LogicalVolume(new G4Box("Fe2", XY, XY, tFe / 2), steel, "Fe2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + tFe / 2}, lvFe2, "Fe2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + tFe;
        fShieldMaterialName = "Steel-Fungus-Steel";
    }

    else if (fCfg == 13) // ---- Steel | Water | Fungus | Water | Steel ----
    {
        G4double tFe = 2.0 * mm;
        auto lvFe1 = new G4LogicalVolume(new G4Box("Fe1", XY, XY, tFe / 2), steel, "Fe1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tFe / 2}, lvFe1, "Fe1PV", logicWorld, false, 0, true);
        auto lvW1 = new G4LogicalVolume(new G4Box("Wat1", XY, XY, fWaterThickness / 2), water, "Wat1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness / 2}, lvW1, "Wat1PV", logicWorld, false, 0, true);
        auto lvF = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvF, "FungPV", logicWorld, false, 0, true);
        auto lvW2 = new G4LogicalVolume(new G4Box("Wat2", XY, XY, fWaterThickness / 2), water, "Wat2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness / 2}, lvW2, "Wat2PV", logicWorld, false, 0, true);
        auto lvFe2 = new G4LogicalVolume(new G4Box("Fe2", XY, XY, tFe / 2), steel, "Fe2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness + tFe / 2}, lvFe2, "Fe2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + fWaterThickness + tFe;
        fShieldMaterialName = "Steel-Water-Fungus-Water-Steel";
    }

    else if (fCfg == 14) // ---- Al | Concrete | Water | Fungus | Water | Concrete | Al ----
    {
        G4double tAl = 2.0 * mm;
        G4double tConc = 1.0 * cm;
        auto lvAl1 = new G4LogicalVolume(new G4Box("Al1", XY, XY, tAl / 2), aluminum, "Al1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tConc - tAl / 2}, lvAl1, "Al1PV", logicWorld, false, 0, true);
        auto lvC1 = new G4LogicalVolume(new G4Box("Conc1", XY, XY, tConc / 2), concrete, "Conc1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tConc / 2}, lvC1, "Conc1PV", logicWorld, false, 0, true);
        auto lvW1 = new G4LogicalVolume(new G4Box("Wat1", XY, XY, fWaterThickness / 2), water, "Wat1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness / 2}, lvW1, "Wat1PV", logicWorld, false, 0, true);
        auto lvF = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvF, "FungPV", logicWorld, false, 0, true);
        auto lvW2 = new G4LogicalVolume(new G4Box("Wat2", XY, XY, fWaterThickness / 2), water, "Wat2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness / 2}, lvW2, "Wat2PV", logicWorld, false, 0, true);
        auto lvC2 = new G4LogicalVolume(new G4Box("Conc2", XY, XY, tConc / 2), concrete, "Conc2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness + tConc / 2}, lvC2, "Conc2PV", logicWorld, false, 0, true);
        auto lvAl2 = new G4LogicalVolume(new G4Box("Al2", XY, XY, tAl / 2), aluminum, "Al2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness + tConc + tAl / 2}, lvAl2, "Al2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + fWaterThickness + tConc + tAl;
        fShieldMaterialName = "Al-Concrete-Water-Fungus-Water-Concrete-Al";
    }

    else if (fCfg == 15) // ---- Pb | Concrete | Water | Fungus | Water | Concrete | Pb ----
    {
        G4double tPb = 5.0 * mm;
        G4double tConc = 1.0 * cm;
        auto lvPb1 = new G4LogicalVolume(new G4Box("Pb1", XY, XY, tPb / 2), lead, "Pb1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tConc - tPb / 2}, lvPb1, "Pb1PV", logicWorld, false, 0, true);
        auto lvC1 = new G4LogicalVolume(new G4Box("Conc1", XY, XY, tConc / 2), concrete, "Conc1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness - tConc / 2}, lvC1, "Conc1PV", logicWorld, false, 0, true);
        auto lvW1 = new G4LogicalVolume(new G4Box("Wat1", XY, XY, fWaterThickness / 2), water, "Wat1LV");
        new G4PVPlacement(nullptr, {0, 0, -fThickness / 2 - fWaterThickness / 2}, lvW1, "Wat1PV", logicWorld, false, 0, true);
        auto lvF = new G4LogicalVolume(new G4Box("Fung", XY, XY, fThickness / 2), melaninFungi, "FungLV");
        new G4PVPlacement(nullptr, {0, 0, 0}, lvF, "FungPV", logicWorld, false, 0, true);
        auto lvW2 = new G4LogicalVolume(new G4Box("Wat2", XY, XY, fWaterThickness / 2), water, "Wat2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness / 2}, lvW2, "Wat2PV", logicWorld, false, 0, true);
        auto lvC2 = new G4LogicalVolume(new G4Box("Conc2", XY, XY, tConc / 2), concrete, "Conc2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness + tConc / 2}, lvC2, "Conc2PV", logicWorld, false, 0, true);
        auto lvPb2 = new G4LogicalVolume(new G4Box("Pb2", XY, XY, tPb / 2), lead, "Pb2LV");
        new G4PVPlacement(nullptr, {0, 0, fThickness / 2 + fWaterThickness + tConc + tPb / 2}, lvPb2, "Pb2PV", logicWorld, false, 0, true);
        zBackFace = fThickness / 2 + fWaterThickness + tConc + tPb;
        fShieldMaterialName = "Pb-Concrete-Water-Fungus-Water-Concrete-Pb";
    }

    // ================= DETECTOR =================
    auto solidDetector = new G4Box("Detector", XY, XY, detThickness / 2);
    auto logicDetector = new G4LogicalVolume(solidDetector,
                                             nist->FindOrBuildMaterial("G4_WATER"), "Detector");
    G4double detZ = zBackFace + detGap + detThickness / 2;
    new G4PVPlacement(nullptr, {0, 0, detZ}, logicDetector,
                      "DetectorPV", logicWorld, false, 0, true);

    G4cout << "\n===== CONFIG " << fCfg << ": " << fShieldMaterialName
           << " | Core: " << fThickness / cm << " cm"
           << " | Det z: " << detZ / cm << " cm =====\n";

    return physWorld;
}
