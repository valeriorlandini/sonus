# sonus
A collection of various Max/MSP objects for creative patching. This package was born as a collection of some custom externals
I made for different projects, so they are not really linked by a common thread. They anyway more or less deal with math,
artificial intelligence and weird sounds.

Currently finished externals are:

### Generators
* sonus.vaosc~
* sonus.window~

### Effects
* sonus.quadpan~

### Sample Manipulation
* sonus.wavesets~

### Neural Networks and AI
* sonus.acti
* sonus.acti~
* sonus.ca
* sonus.neuron
* sonus.perceptron~

### Math and Utilities
* sonus.fselector~
* sonus.hop
* sonus.mima~
* sonus.quadpanner
* sonus.tent


## How to build

`mkdir build` to create a folder with your various build files
`cd build` to put yourself into that folder

### Mac 

Run `cmake -G Xcode ..`

Next run `cmake --build .` or open the Xcode project from this "build" folder and use the GUI.

Note: you can add the `-j4` option where "4" is the number of cores to use.  This can help to speed up your builds, though sometimes the error output is interleaved in such a way as to make troubleshooting more difficult.

If you are running on a Mac M1 machine, you will likely see an error `cannot be loaded due to system security policy` when loading your externals in Max. To resolve this, you can ad-hoc codesign your external with `codesign --force --deep -s - myobject.mxo`.

### Windows

You can run `cmake --help` to get a list of the options available. 

Visual Studio 2022:

`cmake -G "Visual Studio 17 2022" ..`

Visual Studio 2019:

`cmake -G "Visual Studio 16 2019" ..`

Visual Studio 2017:

`cmake -G "Visual Studio 15 2017 Win64" ..`

Having generated the projects, you can now build by opening the .sln file in the build folder with the Visual Studio app (just double-click the .sln file) or you can build on the command line like this:

`cmake --build . --config Release`

