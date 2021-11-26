#include "Geometry.hh"
#include "UserActionInitialization.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "FTFP_BERT.hh"

// If you want to visualize the setup:
//  1. set nameUISession = "qt"
//  2. uncomment ui->SessionStart() 

//-------------------------------------------------------------------------------
  int main( int argc, char** argv ) 
//-------------------------------------------------------------------------------
{
   // ============= [ Setting up the application environment ] ================
   typedef  FTFP_BERT PhysicsList;                   // Physics List
   G4String nameUISession = "csh";                   // UI Session (qt/tcsh/...)
   G4String nameMainMacro = "GlobalSetup.mac";      // Initialization  macros
   // =========================================================================

// Construct the default run manager
   G4RunManager * runManager = new G4RunManager;

// Set up mandatory user initialization: Geometry
   runManager->SetUserInitialization( new Geometry() );

// Set up mandatory user initialization: Physics-List
   runManager->SetUserInitialization( new PhysicsList );

// Set up user initialization: User Actions 
   runManager->SetUserInitialization( new UserActionInitialization() );
    
// Initialize G4 kernel
   runManager->Initialize();
    
    G4cout<<"HOI"<<G4endl;

// Create visualization environment
   G4VisManager* visManager = new G4VisExecutive;
   visManager->Initialize();
    G4cout<<"HOI  2"<<G4endl;


// Start interactive session
   G4UImanager*   uiManager = G4UImanager::GetUIpointer();
    G4cout<<"HOI  3"<<G4endl;
   G4UIExecutive* ui = new G4UIExecutive(argc, argv, nameUISession);
    G4cout<<"HOI  4"<<G4endl;
   uiManager->ApplyCommand("/control/execute " + nameMainMacro);
    G4cout<<"HOI  5"<<G4endl;
   //ui->SessionStart();


// Job termination
   delete ui;
   delete visManager;
   delete runManager;

   return 0;
}
