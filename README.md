#P-Threads Power Fault Tester

##What is it?
*P-Threads Power Fault Tester* is a simulator that mimics what happens when power is cut from a hard drive. This software package will create a *device-file* which should be considered as a storage medium for this simulation. The *device-file* will have 8kB records written continuously until "power is lost". At this point, a validation program will run and output the results of the simulation. These results will include statistics about complete writes and partial writes, the percentage of records written, if records were written to bad portions of the *device-file*, etc.

##The latest version
The latest version can always be obtained at the GitHub repo [Pthread Power Fault Tester](https://github.com/russellfolk/Pthread_Power_Fault_Tester).

##Documentation
Documentation is provided throughout the code and may later be extended to utilize Doxygen.

##Obtaining the program
The latest version of this program may be obtained at anytime by cloning the repo: `git clone https://github.com/russellfolk/Pthread_Power_Fault_Tester.git`

##Running the program
A completely automated way to run this simulation is provided in the `run_experiments.sh` bash script. If you run with no arguments, a `device-file` will be created inside the `bin/` directory, the *writer*, located at `bin/writer`, will use 16 threads and write continuously for 1 minute, and the *checker*, located at `bin/checker` will then check the file and output results to the screen. For more advanced usage please use the help flag: `./run_experiments.sh -?`.

To run each piece individually, the following commands are provided:

###*device-file* creation
A *device-file* may be created using the `create_device.sh` bash script. For instructions, utilize the help menu: `./create_device.sh -?`

###compilation
To compile the programs, use the supplied makefile, `Makefile`. To use, simply enter the command `make`. If you need to clean the outputs of previous runs first, use `make clean; make`.

###*writer* running
The *writer* is found, after compilation, in the `bin/` directory. For instructions, utilize the help menu: `./bin/writer -?`

###*checker* running
The *checker* is found, after compilation, in the `bin/` directory. For instructions, utilize the help menu: `./bin/checker -?`


##Licensing
See LICENSE.md for license information.

##Authors
Russell Y. Folk
