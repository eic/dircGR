## dircGR -- standalone geometric reconstruction (GR) for DIRC PID

This code does Geometric Reconstruction (GR) in npsim simulations of
the ePIC hpDIRC. It is a standalone code that compiles and runs
inside or outside the eic-shell, and it reads two TTrees and
calculates thetaC (mrad) for every particle incident on hpDIRC bars.
This code was developed to explore the hpDIRC PID performance and to
inform about viable approaches for doing this same reconstruction inside
eicrecon soon. 

## Installation

Once you have a working eic-shell, first install [ddDircAction](https://github.com/eic/ddDircAction/tree/main),
the stepping action plugin that writes the "incidence tree."
To compile ddDircAction:
```
cd eic
git clone git@github.com:eic/ddDircAction.git
cd ddDircAction
mkdir build install
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install -DCMAKE_C_COMPILER=/usr/bin/cc -DCMAKE_CXX_COMPILER=/usr/bin/g++
cmake --build build -- install
cd .. 
```
Make sure that the ddDircAction location is added to LD_LIBRARY_PATH (update the
path used to match your case):
```
export LD_LIBRARY_PATH=/Users/wjllope/eic/ddDircAction/install/lib:${LD_LIBRARY_PATH}
```
This line should be added to your local mysetup script. 

To install [dircGR](https://github.com/eic/dircGR/tree/main):
```
cd ~/eic
git clone git@github.com:eic/dircGR.git
cd build
cmake ..
cmake --build .
```
This will install the folder "dircGR" inside your "eic" subdirectory,
and build the application "ddircGR_app" which will be found in 
~/eic/dircGR/run/ when cmake completes. 

Note the steps above can be done either inside or outside the eic-shell.

Once the app is built, change directories to ../run/

You will notice the subdirectory named "LUT" there! Inside
it are the ten look-up tables for the ten bars in each bar box. 
There is thus no need for you to generate LUTs. If you would like
to do this anyway though, some helper files are also in dircGR/run, 
feel free to contact me for additional details. 

##Doing a single particle simulation##

One mode to run npsim is the "gun", where single particles with
specific momenta and direction can be fired from the vertex.

You may need to install and build the epic geometry (do the cmake commands
and source thisepic.sh from inside the shell):
```
cd eic
git clone git@github.com:eic/epic.git
cd epic
mkdir build install
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install
cmake --build build -- install
cd ..
source install/bin/thisepic.sh
```

If you haven't already, start the shell. Don't forget
to make sure the LD_LIBRARY_PATH etc for ddDircAction is defined
correctly when you are inside the shell (i.e. always source your
mysetup script immediately after you start the shell, every time).

Let's throw 3 GeV pions at a polar angle of 70 degrees. The
azimuthal angle is set for this polar angle and momentum so
that the particles strike the center (in azimuth) of bar "5",
one of the "middle" bars in each bar box. For convenience
in the subsequent steps, i am running npsim from within the 
.../eic/dircGR/run/ directory here: 
```
cd dircGR/run
npsim.py --runType batch \
--printLevel WARNING \
--action.step '{"name":"ddDIRCactionStep","parameter":{"OutputBase":"sim_dirconly_500evt_pi+3GeV70deg.incidence", "fileNumber":1, "DetailLevel":1}}' \
--compactFile $DETECTOR_PATH/epic.xml -G -N 500 --gun.particle "pi+" \
--gun.momentumMin 3*GeV --gun.momentumMax 3*GeV --gun.phiMin 354.73*deg --gun.phiMax 354.73*deg \
--gun.thetaMin 70*deg --gun.thetaMax 70*deg --gun.distribution uniform --gun.position 0*cm,0*cm,0*cm \
--outputFile sim_dirconly_500evt_pi+3GeV70deg.edm4hep.root
```

This will take a few minutes to complete. When it does, you should
see these files in this same directory:
```
 24351777 Aug  7 14:57 sim_dirconly_500evt_pi+3GeV70deg.edm4hep.root
   529160 Aug  7 14:57 sim_dirconly_500evt_pi+3GeV70deg.incidence.root
```
Note these files have the same basename, and the extensions 'edm4hep.root'
for the standard npsim output (large file) and 'incidence.root' for
the charged particle incidence information (small file). 

To do the PID, you then run the edm4hep.root file through dircGR.
I generally build dircGR and run it outside the shell.
```
./dircGR_app sim_dirconly_500evt_pi+3GeV70deg.edm4hep.root epic
```

You will now see two new files in this same directory:
```
sim_dirconly_500evt_pi+3GeV70deg.gr.pdf
sim_dirconly_500evt_pi+3GeV70deg.gr.root
```
Note the basename is the same, and the extensions are ".gr.(pdf/root)".



