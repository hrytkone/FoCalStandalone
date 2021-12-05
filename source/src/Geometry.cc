
//mini-FoCal at SPS setup
//with trigger Scintillators at 0X0 & 20X0

//edited i 20190805  Isshiki
//This code was modified the the length of materialals
//include the 2 Scintirator front of the 1layer and

//edited in 2021.11.23 H. Rytkönen
// Only pixels and pads, corresponding the modified
// setup used in 2021 test beam (tungsten in front)

//LGL Layers
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
	logVol_World->SetVisAttributes (G4VisAttributes::Invisible);

	G4PVPlacement* physVol_World  =
		new G4PVPlacement(G4Transform3D(), "PhysVol_World", logVol_World, 0,
				false, 0);
	//*****************************************************************************************
	//G4box is the material
    G4Box* Tungsten 	= new G4Box("PTungsten",	WidthW/2.0, WidthW/2.0, PAD_Absorber_Z/2.0);
	//PAD layer
	G4Box* PAD_Glue 	= new G4Box("PPAD_Glue",	PAD_MiniFoCal_X/2.0, PAD_MiniFoCal_Y/2.0, PAD_Glue_Z/2.0);
	G4Box* PAD_Silicon 	= new G4Box("PPAD_Silicon",	PAD_X/2.0, 			 PAD_Y/2.0, 		  PAD_Z/2.0);
	G4Box* PAD_FPC 		= new G4Box("PPAD_FPC",		PAD_MiniFoCal_X/2.0, PAD_MiniFoCal_Y/2.0, PAD_FPC_Z/2.0);
	//PIX layer
    G4Box* PIX_Flex  	= new G4Box("PPIX_Flex",	PIX_Flex_X/2.0,      PIX_Flex_Y/2.0,      PIX_Z/2.0);
	G4Box* PIX_Alpide 	= new G4Box("PPIX_Alpide",	PIX_Alpide_X/2.0,    PIX_Alpide_Y/2.0,    PIX_Z/2.0);
    G4Box* PIX_Al       = new G4Box("PPIX_Al",      WidthAl/2.0,         WidthAl/2.0,         PIX_Al_Z/2.0);
	//*****************************************************************************************
	//Unique materials
	//*****************************************************************************************
	//The definition of the W alloy
	G4Material* material_tungsten = new G4Material("tungsten", 18.73*g/cm3, 3);
	G4Element* elW  = material_Man->FindOrBuildElement(74);//density: 19.3  I:727
	G4Element* elNi = material_Man->FindOrBuildElement(28);//density: 8.902   I:311
	G4Element* elCu = material_Man->FindOrBuildElement(29);//G4_Cu  8.96   I:322
	material_tungsten->AddElement(elW,94*perCent);    //the percentage of materialal
	material_tungsten->AddElement(elNi,4*perCent);
	material_tungsten->AddElement(elCu,2*perCent);

	//Definition of the Epoxy Glue
	G4Material* material_PET = new G4Material("PET", 1.38*g/cm3, 3);
	G4Element* elO = material_Man->FindOrBuildElement(8);
	G4Element* elH = new G4Element("Hydrogen","H", 1, 1.00794 *g/mole);
	G4Element* elC = new G4Element("Carbon","C", 6, 12.011 *g/mole);
	material_PET->AddElement(elC,10);
	material_PET->AddElement(elH,8);
	material_PET->AddElement(elO,4);

    //Definition of the Kapton film (flex)
    G4Material* material_Kapton = new G4Material("Kapton", 1.42*g/cm3, 4);
    G4Element* elN = material_Man->FindOrBuildElement(7);
    material_Kapton->AddElement(elH,2.6362*perCent);
    material_Kapton->AddElement(elC,69.1133*perCent);
    material_Kapton->AddElement(elN,7.3270*perCent);
    material_Kapton->AddElement(elO,20.9235*perCent);

	//Other materials
	G4Material* material_Si = material_Man->FindOrBuildMaterial("G4_Si");
	G4Material* material_Al = material_Man->FindOrBuildMaterial("G4_Al");
	//*****************************************************************************************
	//   Logical volumes
	//*****************************************************************************************
    G4LogicalVolume* logVol_Tungsten 	= new G4LogicalVolume( Tungsten, 	material_tungsten, "LogVol_Tungsten", 0, 0, 0 );
	//PAD
	G4LogicalVolume* logVol_PAD_Silicon	= new G4LogicalVolume( PAD_Silicon, material_Si,  "LogVol_PAD_Silicon", 0, 0, 0 );
	G4LogicalVolume* logVol_PAD_Glue 	= new G4LogicalVolume( PAD_Glue, 	material_PET, "LogVol_PAD_Glue", 0, 0, 0 );
	G4LogicalVolume* logVol_PAD_FPC 	= new G4LogicalVolume( PAD_FPC, 	material_PET, "LogVol_PAD_FPC", 0, 0, 0 );
	fScoringVol_PAD = logVol_PAD_Silicon;
	//PIX
    G4LogicalVolume* logVol_PIX_Al      = new G4LogicalVolume( PIX_Al,      material_Al,     "LogVol_PIX_Al", 0, 0, 0 );
    G4LogicalVolume* logVol_PIX_Alpide	= new G4LogicalVolume( PIX_Alpide,  material_Si,     "LogVol_PIX_Alpide", 0, 0, 0 );
    G4LogicalVolume* logVol_PIX_Flex	= new G4LogicalVolume( PIX_Flex,    material_Kapton, "LogVol_PIX_Flex", 0, 0, 0 );
	fScoringVol_PIX = logVol_PIX_Alpide;
	//*****************************************************************************************
	//							 Construct E-Cal detector
	//*****************************************************************************************
	//The PAD and PIX layer thicknesses can be different - this variable remembers the previous layer thickness either PIX or PAD
	//It is changed at the end of the loops
	G4double TotalLayerThickness = 0;

    // ----------------------
    //    Tungsten layers   |
    // ----------------------
    G4cout << "\n=================================================================" << G4endl;
    G4cout << "LAYERS : " << G4endl;
	for(G4int ilayer=0; ilayer<NumberW; ilayer++){
		G4double position_Z_W = Start_Z + PAD_Absorber_Z/2 + TotalLayerThickness;
		G4cout << ilayer << " W at " << position_Z_W << G4endl;

		G4ThreeVector threeVect_PAD_W 	= G4ThreeVector(Start_X, Start_Y, position_Z_W);
		G4RotationMatrix rotMtrx_PAD_W 	= G4RotationMatrix();
		G4Transform3D trans3D_PAD_W 	= G4Transform3D(rotMtrx_PAD_W, 		threeVect_PAD_W);
        new G4PVPlacement(trans3D_PAD_W, "PhysVol_W", logVol_Tungsten, physVol_World, false, 0);

		//Which layer thickness
		TotalLayerThickness += PAD_Absorber_Z + GapW;
	}
    TotalLayerThickness += GapBtwWandPad - GapW;

    // -----------------
    //    Single pad   |
    // -----------------
	G4double position_Z_PAD_Silicon		= Start_Z + PAD_Z/2 + TotalLayerThickness;
    G4double position_Z_PAD_Glue	= Start_Z + PAD_Z + PAD_Glue_Z/2 + TotalLayerThickness;
    G4double position_Z_PAD_FPC		= Start_Z + PAD_Z + PAD_Glue_Z + PAD_FPC_Z/2 + TotalLayerThickness;
	G4cout << NumberW << " PAD at " << position_Z_PAD_Silicon << G4endl;
	for(int iy = 0; iy < NpadY; iy++){
		for(int ix = 0; ix < NpadX; ix++){
			G4double position_X_PAD_Silicon = (Start_X - PAD_MiniFoCal_X/2 + ix*PAD_X + PAD_X/2)*mm;
			G4double position_Y_PAD_Silicon = (Start_Y - PAD_MiniFoCal_Y/2 + iy*PAD_Y + PAD_Y/2)*mm;
			G4ThreeVector    threeVect_PAD_Silicon	= G4ThreeVector(position_X_PAD_Silicon, position_Y_PAD_Silicon, position_Z_PAD_Silicon);
			G4RotationMatrix rotMtrx_PAD_Silicon	= G4RotationMatrix();
			G4Transform3D    trans3D_PAD_W			= G4Transform3D(rotMtrx_PAD_Silicon, threeVect_PAD_Silicon);

			G4int IDnumber_PAD = IDnumber_PAD_First + ix + NpadX*iy;
			//G4cout << "\tChannel id " << IDnumber_PAD << "  , ix  iy = " << ix << "  " << iy << G4endl;
			new G4PVPlacement(trans3D_PAD_W, "PhysVol_Si", logVol_PAD_Silicon, physVol_World, false, IDnumber_PAD);
		}
	}

	G4ThreeVector threeVect_PAD_Glue 	= G4ThreeVector(Start_X, Start_Y, position_Z_PAD_Glue);
	G4ThreeVector threeVect_PAD_FPC 	= G4ThreeVector(Start_X, Start_Y, position_Z_PAD_FPC);
	G4RotationMatrix rotMtrx_PAD_Glue 	= G4RotationMatrix();
	G4RotationMatrix rotMtrx_PAD_FPC 	= G4RotationMatrix();
	G4Transform3D trans3D_PAD_Glue      = G4Transform3D(rotMtrx_PAD_Glue, 	threeVect_PAD_Glue);
	G4Transform3D trans3D_PAD_FPC 	    = G4Transform3D(rotMtrx_PAD_FPC, 	threeVect_PAD_FPC);

	new G4PVPlacement(trans3D_PAD_Glue, 	"PhysVol_Glue", 	logVol_PAD_Glue, 	physVol_World, false, 0);
	new G4PVPlacement(trans3D_PAD_FPC, 		"PhysVol_FPC", 		logVol_PAD_FPC,     physVol_World, false, 0);

    TotalLayerThickness += GapBtwPadAndPixel + PAD_Layer_Thickness;

    // -----------------------------------------------------------------------------------
    //    Pixel layers:
    //      - 1 mm aluminum
    //      - 0.97 mm air
    //      - 0.005 mm sensor/flex
    //      - 3.5 mm tungsten
    //      - 0.005 mm sensor/flex
    //      - 0.97 mm air
    //      - 1 mm aluminum
    // -----------------------------------------------------------------------------------
    G4AssemblyVolume *assemblyPixel = new G4AssemblyVolume();

    G4double position_Z_PIX_Absorber	= PIX_Absorber_Z/2 + PIX_Z + PIX_AirGap + PIX_Al_Z;
    G4double position_Z_PIX_Al1         = PIX_Al_Z/2;
    G4double position_Z_PIX_Al2         = PIX_Al_Z/2 + PIX_Absorber_Z + 2*PIX_Z + 2*PIX_AirGap + PIX_Al_Z;

    G4ThreeVector threeVect_PIX_W 		= G4ThreeVector(Start_X, Start_Y, position_Z_PIX_Absorber);
    G4ThreeVector threeVect_PIX_Al1 	= G4ThreeVector(Start_X, Start_Y, position_Z_PIX_Al1);
    G4ThreeVector threeVect_PIX_Al2 	= G4ThreeVector(Start_X, Start_Y, position_Z_PIX_Al2);

    G4RotationMatrix rotMtrx_PIX_W 		= G4RotationMatrix();
    G4RotationMatrix rotMtrx_PIX_Al1 	= G4RotationMatrix();
    G4RotationMatrix rotMtrx_PIX_Al2 	= G4RotationMatrix();

    G4Transform3D trans3D_PIX_W       	= G4Transform3D(rotMtrx_PIX_W, 		 threeVect_PIX_W);
    G4Transform3D trans3D_PIX_Al1    	= G4Transform3D(rotMtrx_PIX_Al1, 	 threeVect_PIX_Al1);
    G4Transform3D trans3D_PIX_Al2 	    = G4Transform3D(rotMtrx_PIX_Al2, 	 threeVect_PIX_Al2);

    assemblyPixel->AddPlacedVolume( logVol_PIX_Al,    trans3D_PIX_Al1 );
    assemblyPixel->AddPlacedVolume( logVol_Tungsten,  trans3D_PIX_W );
    assemblyPixel->AddPlacedVolume( logVol_PIX_Al,    trans3D_PIX_Al2 );

    // In front of tungsten
    for (int irow=0; irow<NumberPixRow; irow++) {
        G4double position_Z_layer	= PIX_Z/2 + PIX_AirGap + PIX_Al_Z;
        G4double position_Y_flex    = -PIX_Full_Length/2. + PIX_Alpide_Y + PIX_Flex_Y/2. + irow*PIX_And_Flex_length;
        G4ThreeVector threeVect_PIX_Flex 		= G4ThreeVector(Start_X, position_Y_flex, position_Z_layer);
        G4RotationMatrix rotMtrx_PIX_Flex 		= G4RotationMatrix();
        G4Transform3D trans3D_PIX_Flex       	= G4Transform3D(rotMtrx_PIX_Flex, threeVect_PIX_Flex);
        assemblyPixel->AddPlacedVolume( logVol_PIX_Flex, trans3D_PIX_Flex );

        for (int icol=0; icol<NumberPixCol; icol++) {
            G4double position_X_Alpide = -PIX_Full_Width/2. + PIX_Alpide_X/2. + icol*PIX_Alpide_X;
            G4double position_Y_Alpide = -PIX_Full_Length/2. + PIX_Alpide_Y/2. + irow*PIX_And_Flex_length;

            G4ThreeVector threeVect_PIX_Alpide 	= G4ThreeVector(position_X_Alpide, position_Y_Alpide, position_Z_layer);
            G4RotationMatrix rotMtrx_PIX_Alpide	= G4RotationMatrix();
            G4Transform3D trans3D_PIX_Alpide   	= G4Transform3D(rotMtrx_PIX_Alpide, threeVect_PIX_Alpide);

            assemblyPixel->AddPlacedVolume( logVol_PIX_Alpide, trans3D_PIX_Alpide );
        }
    }

    // Behind of tungsten
    for (int irow=0; irow<NumberPixRow; irow++) {
        G4double position_Z_layer	=  PIX_Z/2. + PIX_Absorber_Z + PIX_Z + PIX_AirGap + PIX_Al_Z;
        G4double position_Y_flex    = -PIX_Full_Length/2. + PIX_Flex_Y/2. + irow*PIX_And_Flex_length + PIX_Layers_Offset;
        G4ThreeVector threeVect_PIX_Flex 		= G4ThreeVector(Start_X, position_Y_flex, position_Z_layer);
        G4RotationMatrix rotMtrx_PIX_Flex 		= G4RotationMatrix();
        G4Transform3D trans3D_PIX_Flex       	= G4Transform3D(rotMtrx_PIX_Flex, threeVect_PIX_Flex);
        assemblyPixel->AddPlacedVolume( logVol_PIX_Flex, trans3D_PIX_Flex );

        for (int icol=0; icol<NumberPixCol; icol++) {
            G4double position_X_Alpide = -PIX_Full_Width/2. + PIX_Alpide_X/2. + icol*PIX_Alpide_X;
            G4double position_Y_Alpide = -PIX_Full_Length/2. + PIX_Flex_Y + PIX_Alpide_Y/2. + irow*PIX_And_Flex_length + PIX_Layers_Offset;

            G4ThreeVector threeVect_PIX_Alpide 	= G4ThreeVector(position_X_Alpide, position_Y_Alpide, position_Z_layer);
            G4RotationMatrix rotMtrx_PIX_Alpide	= G4RotationMatrix();
            G4Transform3D trans3D_PIX_Alpide   	= G4Transform3D(rotMtrx_PIX_Alpide, threeVect_PIX_Alpide);

            assemblyPixel->AddPlacedVolume( logVol_PIX_Alpide, trans3D_PIX_Alpide );
        }
    }

    for (int ipixlayer=0; ipixlayer<NumberPIX; ipixlayer++) {
        G4double position_Z_PIX      = Start_Z + PIX_Layer_Thickness/2. + TotalLayerThickness;
        G4cout << NumberW+NumberPAD+ipixlayer << " PIX at " << position_Z_PIX << G4endl;

        G4ThreeVector threeVect_PIX  = G4ThreeVector(Start_X, Start_Y, position_Z_PIX);
        G4RotationMatrix rotMtrx_PIX = G4RotationMatrix();
        G4Transform3D trans3D_PIX    = G4Transform3D(rotMtrx_PIX, threeVect_PIX);
        assemblyPixel->MakeImprint( logVol_World, trans3D_PIX );
        TotalLayerThickness += PIX_Layer_Thickness + 5.*PIX_Absorber_Z;
    }
    G4cout << "\nAlpide size is x=" << PIX_Alpide_X << " (" << NpixX << " pixels)  y=" << PIX_Alpide_Y << " (" << NpixY << " pixels) " << G4endl;
    G4cout << "\t--> One channel size is then x=" << PIX_Alpide_X/NpixX << " mm  y=" << PIX_Alpide_Y/NpixY << " mm" << G4endl;
    G4cout << "=================================================================" << G4endl;

    // Change alpide IDs
    int new_id = IDnumber_PIX_First;
    std::vector<G4VPhysicalVolume*>::iterator iter = assemblyPixel->GetVolumesIterator();
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

    fAssemblyVol_PIX = assemblyPixel;

	return physVol_World;
}
