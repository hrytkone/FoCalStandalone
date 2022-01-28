# Program installation

These are needed to run the simulation program:
 * Geant4
 * ROOT6

It's important that both have been compiled with the same compiler version, otherwise there will be a linking problem when the program tries to create the dictionary for the custom datatype in which some of the data is saved.

To build the program do this:

```
cd FoCalStandalone
cmake -DGeant4_DIR=$GEANT4_DIR source/
make
```

Environmental variable GEANT4_DIR needs to be set to the Geant4 installation directory.

# Running & modifying the simulation

Running simulation is done just by running
 ```
./FoCal_Main
```
in the main directory.

## Setting number of events, partcle gun, etc.:

In the main directory you'll find 'GlobalSetup.mac'. There you can change for example the event number or which particle is used.

## Changing geometry
There are lots of variables in source/include/constants.hh which change the simulation setup. These might be the most relevant ones:
 * UseFoCalE: swith FoCal-E on/off
 * UseFoCalH: swith FoCal-E on/off
 * UseModifiedSetup
    * if __true__ use 2021 test beam setup (set NumberPAD=1 and NumberPIX=2, NumberW is the number of W plates in front of pad and pixels)
    * if __false__ use default setup (set NumberOfLayers so that it is sum of the pad and pixel layers, also set LayerLayout correctly)

# Other notes

The code is modified from https://github.com/novitzky/FoCalStandalone and FoCal-H part is from https://github.com/nschmidtALICE/fun4all_eicdetectors/tree/focal_TB/simulation/g4simulation/g4focal.
