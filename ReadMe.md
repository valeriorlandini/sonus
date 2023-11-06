# sonus
A collection of various Max/MSP objects for creative patching coded in C++. This package was born as a collection of some custom externals
I made for different projects, so they are not really linked by a common thread. They anyway more or less deal with math,
artificial intelligence and weird sounds.

Currently finished externals are:

### Generators
* `sonus.lorenz~` Lorenz attractor based generator
* `sonus.nwosc~` User defined wavetable oscillator
* `sonus.pulsar~` Pulsar oscillator
* `sonus.roessler~` Rössler attractor based generator
* `sonus.rsosc~` Oscillator with random shapes
* `sonus.vaosc~` Multishape bandlimited oscillator
* `sonus.window~` Window functions generator

### Effects and Sample Manipulation
* `sonus.8bitinv~` Bitcrusher and bit inverter
* `sonus.bitman~` Bit manipulator
* `sonus.buffx` Apply effects to buffers
* `sonus.cheby~` Chebyshev waveshaper
* `sonus.cryptoverb~` Eerie stereo reverberator
* `sonus.quadpan~` Quadraphonic panner
* `sonus.wavesets` Wavesets based sample manipulation

### Neural Networks and AI
* `sonus.acti` Activation functions
* `sonus.acti~` Activation functions (signal rate)
* `sonus.ca` Cellular automata system
* `sonus.ga` Genetic algorithm implementation
* `sonus.ga.num` Genetic algorithm implementation (numbers version)
* `sonus.linden` Lindenmayer generative system
* `sonus.neuron` Artificial neuron implementation
* `sonus.perceptron~` Classic perceptron implementation

### Math and Utilities
* `sonus.blist` Output different items from a list
* `sonus.fselector~` Continuous selector/mixer
* `sonus.hop` Hot inlets operations
* `sonus.llist` Similar to line, but for list of numbers
* `sonus.lurn` Non repeating random sequences
* `sonus.mathc` Mathematical constants
* `sonus.mima~` Signal extremes analyzer
* `sonus.quadpanner` Quadraphonic panner coefficients
* `sonus.summer~` Simple signal mixer
* `sonus.tent` Tent map calculator


## How to build

Grab the source inside your `Documents/Max 8/Packages` folder with `git clone https://github.com/valeriorlandini/sonus.git --recursive` 

`cd sonus` and then `mkdir build` to create a folder with your various build files.

`cd build` to put yourself into that folder.

### Mac 

Run `cmake -G Xcode ..`

Next run `cmake --build . --config Release` or open the Xcode project from this "build" folder and use the GUI.

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

