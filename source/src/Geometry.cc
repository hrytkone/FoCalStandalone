
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Element.hh"
#include "G4Material.hh"
//#include "SensitiveVolume.hh"
#include "G4SDManager.hh"
#include "constants.hh"
#include "G4SystemOfUnits.hh"

//------------------------------------------------------------------------------
Geometry::Geometry() {}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Geometry::~Geometry() {}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
G4VPhysicalVolume* Geometry::Construct()
{
	G4NistManager* material_Man = G4NistManager::Instance();  //NistManager: start element destruction
	G4Box* solid_World =
		new G4Box("Solid_World", World_X/2.0, World_Y/2.0, World_Z/2.0);

	G4Material* material_World = material_Man->FindOrBuildMaterial("G4_AIR");
	G4LogicalVolume* logVol_World = new G4LogicalVolume(solid_World, material_World, "LogVol_World");
	logVol_World->SetVisAttributes(G4VisAttributes::Invisible);

	G4PVPlacement* physVol_World  =
		new G4PVPlacement(G4Transform3D(), "PhysVol_World", logVol_World, 0, false, 0);

    CreateMaterials(material_Man);

	G4double TotalLayerThickness = 0.;
    if (UseFoCalE) {
        G4cout << "FoCal-E included ";
        if (UseModifiedSetup) {
            G4cout << ", modified setup with " << NumberW << " tungsten layers in front" << G4endl;
            TotalLayerThickness = ConstructFoCalEmodified(logVol_World);
        } else {
            G4cout << ", default setup with " << NumberOfLayers << " layers in total" << G4endl;
            TotalLayerThickness = ConstructFoCalE(logVol_World);
        }
    }

    TotalLayerThickness += GapBtwEcalAndHcal;

    G4VSolid* solid_HCAL_envelope = new G4Box("solid_HCAL_envelope", 2620*mm, 2620*mm, Tower_dz/2.);
    G4LogicalVolume* logVol_HCAL_envelope = new G4LogicalVolume(solid_HCAL_envelope,
                                            material_World, "logVol_HCAL_envelope", 0, 0, 0);
    logVol_HCAL_envelope->SetVisAttributes(G4VisAttributes::Invisible);

    //Define rotation attributes for envelope cone
    G4RotationMatrix *rotMtrx_HCAL_envelope = new G4RotationMatrix();
    rotMtrx_HCAL_envelope->rotateX(HCAL_Rot_X);
    rotMtrx_HCAL_envelope->rotateY(HCAL_Rot_Y);
    rotMtrx_HCAL_envelope->rotateZ(HCAL_Rot_Z);

    new G4PVPlacement(rotMtrx_HCAL_envelope,
                      G4ThreeVector(HCAL_Placement_X, HCAL_Placement_Y, Start_Z+TotalLayerThickness+Tower_dz/2.),
                      logVol_HCAL_envelope,
                      "PhysVol_HCAL_envelope",
                       logVol_World, false, 0);

    if (UseFoCalH) {
        G4cout << "FoCal-H included with a " << GapBtwEcalAndHcal << " mm gap to FoCal-E" << G4endl;
        ConstructFoCalH(logVol_HCAL_envelope);
    }

	return physVol_World;
}

G4double Geometry::ConstructFoCalE(G4LogicalVolume* envelope)
{
    G4cout << "\n=================================================================" << G4endl;
    G4cout << "FoCal-E LAYERS : " << G4endl;

    G4Box* PAD_Absorber 	= new G4Box("PPAD_Absorber",	PAD_MiniFoCal_X/2.0, PAD_MiniFoCal_Y/2.0, PAD_Absorber_Z/2.0);
    G4Box* PIX_Absorber 	= new G4Box("PPIX_Absorber",	PAD_MiniFoCal_X/2.0, PAD_MiniFoCal_Y/2.0, PIX_Absorber_Z/2.0);
    G4LogicalVolume* logVol_PAD_Absorber 	= new G4LogicalVolume( PAD_Absorber, 	material_tungsten, "logVol_PAD_Absorber", 0, 0, 0 );
    G4LogicalVolume* logVol_PIX_Absorber 	= new G4LogicalVolume( PIX_Absorber, 	material_tungsten, "logVol_PIX_Absorber", 0, 0, 0 );

    G4VisAttributes* wVisAttributes = new G4VisAttributes(G4Color::Grey());
    logVol_PAD_Absorber->SetVisAttributes(wVisAttributes);
    logVol_PIX_Absorber->SetVisAttributes(wVisAttributes);

    G4AssemblyVolume *assemblyPad = ConstructPadLayer();
    G4AssemblyVolume *assemblyPixel = ConstructPixelLayer();

    G4double TotalLayerThickness = 0;
    for (G4int ilayer=0; ilayer<NumberOfLayers; ilayer++) {
        //This is the PAD layer
        if (LayerLayout[ilayer] == 0) {
            G4double position_Z_PAD_Absorber = Start_Z + PAD_Absorber_Z/2. + TotalLayerThickness;
            G4double position_Z_PAD		     = Start_Z + PAD_Absorber_Z + TotalLayerThickness;

            new G4PVPlacement(0, G4ThreeVector(Start_X, Start_Y, position_Z_PAD_Absorber), logVol_PAD_Absorber, "PhysVol_W", envelope, false, 0);

            G4cout << ilayer << " PAD at " << position_Z_PAD << " " << TotalLayerThickness << G4endl;
            G4ThreeVector trans_PAD = G4ThreeVector(Start_X, Start_Y, position_Z_PAD);
            assemblyPad->MakeImprint(envelope, trans_PAD, 0, false, true);

            TotalLayerThickness += PAD_Absorber_Z;
            TotalLayerThickness += PAD_Layer_Thickness;
            TotalLayerThickness += PAD_AirGap;
        } else if (LayerLayout[ilayer] == 1) {
            G4double position_Z_PIX_Absorber = Start_Z + PIX_Absorber_Z/2. + TotalLayerThickness;
            G4double position_Z_PIX          = Start_Z + PIX_Absorber_Z + TotalLayerThickness;

            new G4PVPlacement(0, G4ThreeVector(Start_X, Start_Y, position_Z_PIX_Absorber), logVol_PIX_Absorber, "PhysVol_W", envelope, false, 0);

            G4cout << ilayer << " PIX at " << position_Z_PIX << "   " << PIX_Layer_Thickness << G4endl;
            G4ThreeVector trans_PIX = G4ThreeVector(Start_X, Start_Y, position_Z_PIX);
            assemblyPixel->MakeImprint(envelope, trans_PIX, 0, false, true);

            TotalLayerThickness += PIX_Absorber_Z;
            TotalLayerThickness += PIX_Layer_Thickness;
            TotalLayerThickness += PIX_AirGap;
        }
    }
    G4cout << "\nAlpide size is x=" << PIX_Alpide_X << " (" << NpixX << " pixels)  y=" << PIX_Alpide_Y << " (" << NpixY << " pixels) " << G4endl;
    G4cout << "\t--> One channel size is then x=" << PIX_Alpide_X/NpixX << " mm  y=" << PIX_Alpide_Y/NpixY << " mm" << G4endl;
    G4cout << "=================================================================" << G4endl;

    // Change Pad layer IDs
    int new_id = IDnumber_PAD_First;
    std::vector<G4VPhysicalVolume*>::iterator iter = assemblyPad->GetVolumesIterator();
    for (int i=0; i<(int)assemblyPad->TotalImprintedVolumes(); iter++,i++)
    {
        G4VPhysicalVolume* ptr = *iter;
        if ( ptr ) {
            if (ptr->GetLogicalVolume()->GetMaterial()->GetName()=="G4_Si") {
                ptr->SetCopyNo(new_id);
                new_id++;
                //G4cout << ptr->GetCopyNo() << "   name : " << ptr->GetName() << G4endl;
            } else {
                ptr->SetCopyNo(0);
            }
        }
    }

    // Change alpide IDs
    new_id = IDnumber_PIX_First;
    iter = assemblyPixel->GetVolumesIterator();
    for (int i=0; i<(int)assemblyPixel->TotalImprintedVolumes(); iter++,i++)
    {
        G4VPhysicalVolume* ptr = *iter;
        if ( ptr ) {
            if (ptr->GetLogicalVolume()->GetMaterial()->GetName()=="G4_Si") {
                ptr->SetCopyNo(new_id);
                new_id++;
                //G4cout << ptr->GetCopyNo() << "   name : " << ptr->GetName() << G4endl;
            } else {
                ptr->SetCopyNo(0);
            }
        }
    }

    return TotalLayerThickness;
}

G4double Geometry::ConstructFoCalEmodified(G4LogicalVolume* envelope)
{
    G4double TotalLayerThickness = 0;

    G4cout << "\n=================================================================" << G4endl;
    G4cout << "FoCal-E LAYERS : " << G4endl;
    // ----------------------
    //    Tungsten layers   |
    // ----------------------
    G4Box* Tungsten 	= new G4Box("PTungsten",	WidthW/2.0, WidthW/2.0, PAD_Absorber_Z/2.0);
    G4LogicalVolume* logVol_Tungsten 	= new G4LogicalVolume( Tungsten, 	material_tungsten, "LogVol_Tungsten", 0, 0, 0 );

    for(G4int ilayer=0; ilayer<NumberW; ilayer++){
		G4double position_Z_W = Start_Z + PAD_Absorber_Z/2 + TotalLayerThickness;
		G4cout << ilayer << " W at " << position_Z_W << G4endl;
        new G4PVPlacement(0, G4ThreeVector(Start_X, Start_Y, position_Z_W), logVol_Tungsten, "PhysVol_W", envelope, false, 0);
		TotalLayerThickness += PAD_Absorber_Z + GapW;
	}
    TotalLayerThickness += GapBtwWandPad - GapW;

    // ----------------------
    //       PAD layer      |
    // ----------------------
    G4AssemblyVolume *assemblyPad = ConstructPadLayer();
    G4double position_Z_PAD       = Start_Z + PAD_Layer_Thickness_mod/2. + TotalLayerThickness;
    G4cout << NumberW << " PAD at " << position_Z_PAD << G4endl;
    G4ThreeVector trans_PAD = G4ThreeVector(Start_X, Start_Y, position_Z_PAD);
    assemblyPad->MakeImprint(envelope, trans_PAD, 0);
    TotalLayerThickness += GapBtwPadAndPixel;

		// Change pad IDs
    int new_id = IDnumber_PAD_First;
    std::vector<G4VPhysicalVolume*>::iterator iter = assemblyPad->GetVolumesIterator();
    for (int i=0; i<(int)assemblyPad->TotalImprintedVolumes(); iter++,i++)
    {
        G4VPhysicalVolume* ptr = *iter;
        if ( ptr ) {
            if (ptr->GetLogicalVolume()->GetMaterial()->GetName()=="G4_Si") {
                ptr->SetCopyNo(new_id);
                new_id++;
                //G4cout << ptr->GetCopyNo() << "   name : " << ptr->GetName() << G4endl;
            } else {
                ptr->SetCopyNo(0);
            }
        }
    }

    // ----------------------
    //     PIXEL layers     |
    // ----------------------
    G4AssemblyVolume *assemblyPixel = ConstructPixelLayer();
    for (int ipixlayer=0; ipixlayer<NumberPIX; ipixlayer++) {
        G4double position_Z_PIX      = Start_Z + PIX_Layer_Thickness/2. + TotalLayerThickness;
        G4cout << NumberW+NumberPAD+ipixlayer << " PIX at " << position_Z_PIX << G4endl;
        G4ThreeVector trans_PIX = G4ThreeVector(Start_X, Start_Y, position_Z_PIX);
        assemblyPixel->MakeImprint(envelope, trans_PIX, 0);
        TotalLayerThickness += PIX_Layer_Thickness + 5.*PIX_Absorber_Z;
    }
    G4cout << "\nAlpide size is x=" << PIX_Alpide_X << " (" << NpixX << " pixels)  y=" << PIX_Alpide_Y << " (" << NpixY << " pixels) " << G4endl;
    G4cout << "\t--> One channel size is then x=" << PIX_Alpide_X/NpixX << " mm  y=" << PIX_Alpide_Y/NpixY << " mm" << G4endl;
    G4cout << "=================================================================" << G4endl;

    // Change alpide IDs
    new_id = IDnumber_PIX_First;
    iter = assemblyPixel->GetVolumesIterator();
    for (int i=0; i<(int)assemblyPixel->TotalImprintedVolumes(); iter++,i++)
    {
        G4VPhysicalVolume* ptr = *iter;
        if ( ptr ) {
            if (ptr->GetLogicalVolume()->GetMaterial()->GetName()=="G4_Si") {
                ptr->SetCopyNo(new_id);
                new_id++;
                //G4cout << ptr->GetCopyNo() << "   name : " << ptr->GetName() << G4endl;
            } else {
                ptr->SetCopyNo(0);
            }
        }
    }
    return TotalLayerThickness;
}

G4AssemblyVolume* Geometry::ConstructPadLayer()
{
    G4AssemblyVolume *assemblyPad = new G4AssemblyVolume();

    G4Box* PAD_Glue1 	= new G4Box("PPAD_Glue1",	PAD_MiniFoCal_X/2.0, PAD_MiniFoCal_Y/2.0, PAD_Glue1_Z/2.0);
    G4Box* PAD_Silicon 	= new G4Box("PPAD_Silicon",	PAD_X/2.0, 			 PAD_Y/2.0, 		  PAD_Z/2.0);
    G4Box* PAD_Glue2 	= new G4Box("PPAD_Glue2",	PAD_MiniFoCal_X/2.0, PAD_MiniFoCal_Y/2.0, PAD_Glue2_Z/2.0);
    G4Box* PAD_FPC 		= new G4Box("PPAD_FPC",		PAD_MiniFoCal_X/2.0, PAD_MiniFoCal_Y/2.0, PAD_FPC_Z/2.0);

    G4LogicalVolume* logVol_PAD_Glue1 	= new G4LogicalVolume( PAD_Glue1, 	material_PET, "LogVol_PAD_Glue1", 0, 0, 0 );
    G4LogicalVolume* logVol_PAD_Silicon	= new G4LogicalVolume( PAD_Silicon, material_Si,  "LogVol_PAD_Silicon", 0, 0, 0 );
    G4LogicalVolume* logVol_PAD_Glue2 	= new G4LogicalVolume( PAD_Glue2, 	material_PET, "LogVol_PAD_Glue2", 0, 0, 0 );
    G4LogicalVolume* logVol_PAD_FPC 	= new G4LogicalVolume( PAD_FPC, 	material_PET, "LogVol_PAD_FPC", 0, 0, 0 );
    fScoringVol_PAD = logVol_PAD_Silicon;

    G4double position_Z_PAD_Glue1	= PAD_Glue1_Z/2;
	G4double position_Z_PAD_Silicon	= PAD_Glue1_Z + PAD_Z/2;
    G4double position_Z_PAD_Glue2	= PAD_Glue1_Z + PAD_Z + PAD_Glue2_Z/2;
    G4double position_Z_PAD_FPC		= PAD_Glue1_Z + PAD_Z + PAD_Glue2_Z + PAD_FPC_Z/2;

    G4Colour yellow(1., 1., 0.);
    G4VisAttributes* padVisAttributes = new G4VisAttributes(yellow);
    logVol_PAD_Silicon->SetVisAttributes(padVisAttributes);

    G4ThreeVector trans_PAD_Glue1 = G4ThreeVector(Start_X, Start_Y, position_Z_PAD_Glue1);
    G4ThreeVector trans_PAD_Glue2 = G4ThreeVector(Start_X, Start_Y, position_Z_PAD_Glue2);
    G4ThreeVector trans_PAD_FPC   = G4ThreeVector(Start_X, Start_Y, position_Z_PAD_FPC);
    if (!UseModifiedSetup)
        assemblyPad->AddPlacedVolume(logVol_PAD_Glue1, trans_PAD_Glue1, 0);
    assemblyPad->AddPlacedVolume(logVol_PAD_Glue2, trans_PAD_Glue2, 0);
    assemblyPad->AddPlacedVolume(logVol_PAD_FPC, trans_PAD_FPC, 0);

    for(int iy = 0; iy < NpadY; iy++){
		for(int ix = 0; ix < NpadX; ix++){
			G4double position_X_PAD_Silicon = (Start_X - PAD_MiniFoCal_X/2 + ix*PAD_X + PAD_X/2)*mm;
			G4double position_Y_PAD_Silicon = (Start_Y - PAD_MiniFoCal_Y/2 + iy*PAD_Y + PAD_Y/2)*mm;
            G4ThreeVector trans_PAD_Silicon = G4ThreeVector(position_X_PAD_Silicon, position_Y_PAD_Silicon, position_Z_PAD_Silicon);
            assemblyPad->AddPlacedVolume(logVol_PAD_Silicon, trans_PAD_Silicon, 0);
			//G4int IDnumber_PAD = IDnumber_PAD_First + ix + NpadX*iy;
		}
	}
    return assemblyPad;
}

G4AssemblyVolume* Geometry::ConstructPixelLayer()
{
    G4AssemblyVolume *assemblyPixel = new G4AssemblyVolume();

    G4Box* PIX_Flex  	= new G4Box("PPIX_Flex",	PIX_Flex_X/2.0,      PIX_Flex_Y/2.0,      PIX_Z/2.0);
    G4Box* PIX_Alpide 	= new G4Box("PPIX_Alpide",	PIX_Alpide_X/2.0,    PIX_Alpide_Y/2.0,    PIX_Z/2.0);
    G4Box* PIX_Al       = new G4Box("PPIX_Al",      WidthAl/2.0,         WidthAl/2.0,         PIX_Al_Z/2.0);

    G4LogicalVolume* logVol_PIX_Flex	= new G4LogicalVolume( PIX_Flex,    material_Kapton, "LogVol_PIX_Flex", 0, 0, 0 );
    G4LogicalVolume* logVol_PIX_Alpide	= new G4LogicalVolume( PIX_Alpide,  material_Si,     "LogVol_PIX_Alpide", 0, 0, 0 );
    G4LogicalVolume* logVol_PIX_Al      = new G4LogicalVolume( PIX_Al,      material_Al,     "LogVol_PIX_Al", 0, 0, 0 );
    fScoringVol_PIX = logVol_PIX_Alpide;

    G4double position_Z_PIX_Al1         = PIX_Al_Z/2;
    G4double position_Z_PIX_Al2         = PIX_Al_Z/2 + PIX_AirGap_AlPlates + PIX_Al_Z;

    G4ThreeVector trans_PIX_Al1      = G4ThreeVector(Start_X, Start_Y, position_Z_PIX_Al1);
    G4ThreeVector trans_PIX_Al2      = G4ThreeVector(Start_X, Start_Y, position_Z_PIX_Al2);
    assemblyPixel->AddPlacedVolume(logVol_PIX_Al,    trans_PIX_Al1, 0);
    assemblyPixel->AddPlacedVolume(logVol_PIX_Al,    trans_PIX_Al2, 0);

    G4Colour blue(0., 0., 1.);
    G4Colour yellow(1., 1., 0.);
    G4VisAttributes* flexVisAttributes = new G4VisAttributes(blue);
    G4VisAttributes* alpideVisAttributes = new G4VisAttributes(yellow);
    logVol_PIX_Flex->SetVisAttributes(flexVisAttributes);
    logVol_PIX_Alpide->SetVisAttributes(alpideVisAttributes);

    // First layer of strips
    for (int irow=0; irow<NumberPixRow; irow++) {
        G4double position_Z_layer	= PIX_Z/2. + PIX_Al_Z;
        G4double position_Y_flex    = -PIX_Full_Length/2. + PIX_Alpide_Y + PIX_Flex_Y/2. + irow*PIX_And_Flex_length;
        G4ThreeVector trans_PIX_flex = G4ThreeVector(Start_X, position_Y_flex, position_Z_layer);
        assemblyPixel->AddPlacedVolume(logVol_PIX_Flex, trans_PIX_flex, 0);

        for (int icol=0; icol<NumberPixCol; icol++) {
            G4double position_X_Alpide = -PIX_Full_Width/2. + PIX_Alpide_X/2. + icol*PIX_Alpide_X;
            G4double position_Y_Alpide = -PIX_Full_Length/2. + PIX_Alpide_Y/2. + irow*PIX_And_Flex_length;
            G4ThreeVector trans_PIX_Alpide = G4ThreeVector(position_X_Alpide, position_Y_Alpide, position_Z_layer);
            assemblyPixel->AddPlacedVolume(logVol_PIX_Alpide, trans_PIX_Alpide, 0);
        }
    }

    // Second layer of strips
    for (int irow=0; irow<NumberPixRow; irow++) {
        G4double position_Z_layer	=  PIX_Al_Z + PIX_AirGap_AlPlates - PIX_Z/2.;
        G4double position_Y_flex    = -PIX_Full_Length/2. + PIX_Flex_Y/2. + irow*PIX_And_Flex_length + PIX_Layers_Offset;
        G4ThreeVector trans_PIX_flex = G4ThreeVector(Start_X, position_Y_flex, position_Z_layer);
        assemblyPixel->AddPlacedVolume(logVol_PIX_Flex, trans_PIX_flex, 0);

        for (int icol=0; icol<NumberPixCol; icol++) {
            G4double position_X_Alpide = -PIX_Full_Width/2. + PIX_Alpide_X/2. + icol*PIX_Alpide_X;
            G4double position_Y_Alpide = -PIX_Full_Length/2. + PIX_Flex_Y + PIX_Alpide_Y/2. + irow*PIX_And_Flex_length + PIX_Layers_Offset;
            G4ThreeVector trans_PIX_Alpide = G4ThreeVector(position_X_Alpide, position_Y_Alpide, position_Z_layer);
            assemblyPixel->AddPlacedVolume( logVol_PIX_Alpide, trans_PIX_Alpide, 0);
        }
    }
    return assemblyPixel;
}

bool Geometry::ConstructFoCalH(G4LogicalVolume* envelope)
{
    //create geometry volumes to place inside single_tower
    G4double copperTubeOuterDiam = 2.5*mm;
    G4double copperTubeInnerDiam = 1.2*mm;
    G4double diameter_fiber = 1.0*mm;

    // fibre cutout
    //G4VSolid* HCAL_FilledCap     = new G4Tubs("PHCAL_FilledCap", 0, copperTubeOuterDiam / 2.0, Tower_dz/2., 0., 2*M_PI*rad);
    G4VSolid* HCAL_Absorber      = new G4Tubs("PHCAL_CopperTube", copperTubeInnerDiam/2., copperTubeOuterDiam/2., Tower_dz/2., 0.,2*M_PI*rad);
    G4VSolid* HCAL_Scintillator  = new G4Tubs("PHCAL_ScintFiber", 0, diameter_fiber/2., Tower_dz/2., 0.,2*M_PI*rad);

    G4NistManager* man = G4NistManager::Instance();
    G4Material* material_absorber = man->FindOrBuildMaterial("G4_Cu");

    //G4LogicalVolume* logVol_FilledCap = new G4LogicalVolume(HCAL_FilledCap,
    //        G4Material::GetMaterial("G4_AIR"),
    //        "logVol_FilledCap",
    //        0, 0, 0);

    G4LogicalVolume* logVol_Absorber = new G4LogicalVolume(HCAL_Absorber,
            material_absorber,
            "logVol_Absorber",
            0, 0, 0);

    G4LogicalVolume* logVol_Scint = new G4LogicalVolume(HCAL_Scintillator,
            material_scintillator,
            "logVol_Scint",
            0, 0, 0);
    fScoringVol_SCINT = logVol_Scint;

    G4AssemblyVolume *assemblyHCAL = new G4AssemblyVolume();
    G4Transform3D trans3D_tube     = G4Transform3D(G4RotationMatrix(), G4ThreeVector(0, 0, 0));

    G4VisAttributes* copperVisAttributes = new G4VisAttributes(G4Color::Brown());
    G4VisAttributes* scintillatorVisAttributes = new G4VisAttributes(G4Color::Yellow());
    logVol_Absorber->SetVisAttributes(copperVisAttributes);
    logVol_Scint->SetVisAttributes(scintillatorVisAttributes);

    //assemblyHCAL->AddPlacedVolume(logVol_FilledCap, trans3D_tube);
    assemblyHCAL->AddPlacedVolume(logVol_Absorber, trans3D_tube);
    assemblyHCAL->AddPlacedVolume(logVol_Scint, trans3D_tube);

    //place physical volumes for absorber and scintillator fiber
    G4double xposition,yposition;
    int icapil = 0;
    for (int icap_x=-NtowerX/2; icap_x<NtowerX/2; icap_x++) {
        xposition = icap_x * copperTubeOuterDiam; //offset for odd/even rows
        for (int icap_y=-NtowerY/2; icap_y<NtowerY/2; icap_y++) {
            yposition = icap_y * copperTubeOuterDiam * sqrt(3)/2;
            icap_y%2==0 ? xposition+=copperTubeOuterDiam/2 : xposition-=copperTubeOuterDiam/2;
            G4ThreeVector threeVect_tower  = G4ThreeVector(xposition, yposition, 0.);
            assemblyHCAL->MakeImprint( envelope, threeVect_tower, 0);
            icapil++;
        }
    }

    // Change fiber IDs
    int new_id = IDnumber_SCINT_First;
    std::vector<G4VPhysicalVolume*>::iterator iter = assemblyHCAL->GetVolumesIterator();
    for (int i=0; i<(int)assemblyHCAL->TotalImprintedVolumes(); iter++,i++)
    {
        G4VPhysicalVolume* ptr = *iter;
        if ( ptr ) {
            if (ptr->GetLogicalVolume()->GetMaterial()->GetName()=="G4_POLYSTYRENE") {
                ptr->SetCopyNo(new_id);
                new_id++;
                //G4cout << ptr->GetCopyNo() << "   name : " << ptr->GetName() << G4endl;
            } else {
                ptr->SetCopyNo(0);
            }
        }
    }

    return true;
}

void Geometry::CreateMaterials(G4NistManager* manager)
{
    G4Element* elH = new G4Element("Hydrogen","H", 1, 1.00794 *g/mole);
    G4Element* elC = new G4Element("Carbon","C", 6, 12.011 *g/mole);
    G4Element* elN = manager->FindOrBuildElement(7);
    G4Element* elO = manager->FindOrBuildElement(8);
    G4Element* elNi = manager->FindOrBuildElement(28);//density: 8.902   I:311
    G4Element* elCu = manager->FindOrBuildElement(29);//G4_Cu  8.96   I:322
    G4Element* elW  = manager->FindOrBuildElement(74);//density: 19.3  I:727

    material_Si = manager->FindOrBuildMaterial("G4_Si");
	material_Al = manager->FindOrBuildMaterial("G4_Al");

    // The Kapton film (flex)
    material_Kapton = new G4Material("Kapton", 1.42*g/cm3, 4);
    material_Kapton->AddElement(elH,2.6362*perCent);
    material_Kapton->AddElement(elC,69.1133*perCent);
    material_Kapton->AddElement(elN,7.3270*perCent);
    material_Kapton->AddElement(elO,20.9235*perCent);

    // The W alloy
    material_tungsten = new G4Material("tungsten", 18.73*g/cm3, 3);
    material_tungsten->AddElement(elW,94*perCent);    //the percentage of materialal
    material_tungsten->AddElement(elNi,4*perCent);
    material_tungsten->AddElement(elCu,2*perCent);

    // The Epoxy Glue
    material_PET = new G4Material("PET", 1.38*g/cm3, 3);
    material_PET->AddElement(elC,10);
    material_PET->AddElement(elH,8);
    material_PET->AddElement(elO,4);

    // The scintillator (FoCal-H)
    G4MaterialPropertiesTable *tab = new G4MaterialPropertiesTable();

    const G4int ntab = 31;
    tab->AddConstProperty("FASTTIMECONSTANT", 2.8*ns); // was 6
    // tab->AddConstProperty("SCINTILLATIONYIELD", 13.9/keV); // was 200/MEV nominal  10
    tab->AddConstProperty("SCINTILLATIONYIELD", 200/MeV); // was 200/MEV nominal, should maybe be 13.9/keV
    tab->AddConstProperty("RESOLUTIONSCALE", 1.0);

    G4double opt_en[] =
        { 1.37760*eV, 1.45864*eV, 1.54980*eV, 1.65312*eV, 1.71013*eV, 1.77120*eV, 1.83680*eV, 1.90745*eV, 1.98375*eV, 2.06640*eV,
          2.10143*eV, 2.13766*eV, 2.17516*eV, 2.21400*eV, 2.25426*eV, 2.29600*eV, 2.33932*eV, 2.38431*eV, 2.43106*eV, 2.47968*eV,
          2.53029*eV, 2.58300*eV, 2.63796*eV, 2.69531*eV, 2.75520*eV, 2.81782*eV, 2.88335*eV, 2.95200*eV, 3.09960*eV, 3.54241*eV,
          4.13281*eV }; // 350 - 800 nm
    G4double scin_fast[] =
        { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0003, 0.0008, 0.0032,
          0.0057, 0.0084, 0.0153, 0.0234, 0.0343, 0.0604, 0.0927, 0.1398, 0.2105, 0.2903,
          0.4122, 0.5518, 0.7086, 0.8678, 1.0000, 0.8676, 0.2311, 0.0033, 0.0012, 0.0000,
          0 };
    tab->AddProperty("FASTCOMPONENT", opt_en, scin_fast, ntab);

    G4double opt_r[] =
        { 1.5749, 1.5764, 1.5782, 1.5803, 1.5815, 1.5829, 1.5845, 1.5862, 1.5882, 1.5904,
          1.5914, 1.5924, 1.5935, 1.5947, 1.5959, 1.5972, 1.5986, 1.6000, 1.6016, 1.6033,
          1.6051, 1.6070, 1.6090, 1.6112, 1.6136, 1.6161, 1.6170, 1.6230, 1.62858, 1.65191,
          1.69165 };
    tab->AddProperty("RINDEX", opt_en, opt_r, ntab);

    G4double opt_abs[] =
        { 2.714*m, 3.619*m, 5.791*m, 4.343*m, 7.896*m, 5.429*m, 36.19*m, 17.37*m, 36.19*m, 5.429*m,
          13.00*m, 14.50*m, 16.00*m, 18.00*m, 16.50*m, 17.00*m, 14.00*m, 16.00*m, 15.00*m, 14.50*m,
          13.00*m, 12.00*m, 10.00*m, 8.000*m, 7.238*m, 4.000*m, 1.200*m, 0.500*m, 0.200*m, 0.200*m,
          0.100*m };
    tab->AddProperty("ABSLENGTH", opt_en, opt_abs, ntab);

    G4double density;
    G4int ncomponents;
    G4Material* material_G4_POLYSTYRENE = new G4Material("G4_POLYSTYRENE", density = 1.05 * g / cm3, ncomponents = 2);
    material_G4_POLYSTYRENE->AddElement(elC, 8);
    material_G4_POLYSTYRENE->AddElement(elH, 8);
    material_G4_POLYSTYRENE->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
    material_G4_POLYSTYRENE->SetMaterialPropertiesTable(tab);

    material_scintillator = material_G4_POLYSTYRENE;
}
