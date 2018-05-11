# dijkstra
Implementation of Dijkstra's shortest path algorithm applied to the WSN lifetime maximation problem.  

## Dependencies
This program makes use of three libraries: 
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp): This allows the writing and (in the future) the reading of data in json format.  This is used to write output data from the simulations in a file to be read later by post-processing programs, usually in Python, to make graphs and statistical summaries. As of 2018/may/11, version 1.8.4 (released on 2017/dec/20) is being used.A **IMPORTANT**: I've just found out that ubuntu 18.04 has version 1.7.4 in its repositories.  You can try to use it although for now it is untested.
* [tclap](http://tclap.sourceforge.net/): A library that provides a simple interface for defining and accessing command line arguments, making it easy to customize the behaviour of the simulations. As of 2018/may/11, version 1.2.2 is being used.
* [sfmt](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/): SFMT is a Linear Feedbacked Shift Register (LFSR) generator that generates a 128-bit pseudorandom integer at one step. It is used to generate the pseudorandom numbers in the simulation program.  As of 2018/may/11, version 1.5.1 is being used.

You'll need to download at least the jsoncpp and sfmt libraries and compile them acocording with the instructions below.  The tclap library consists of include files only, which have been added to this program in the include directory for simplicity.  Furthermore, the include files for the other two libraries have also been added to the include directory, and you only need to compile the SFMT.c source file and the json library according to the instructions below.  The resulting files should be put in the lib directory.

## Build instructions
The first thing is to download the packages from the links above and unpacking it in a temporary directory.

### SFMT
Go to the appropriate directory (currently, SFMT-src-1.5.1) and run the two commands below:

```
gcc -O3 -msse2 -fno-strict-aliasing -DHAVE_SSE2=1 -DSFMT_MEXP=19937 -c SFMT.c
ar cr libsfmt.a SFMT.o
```

This will generate a file named libsfmt.a in the current directory, which should be copied or moved to this project's lib directory.

### Jsoncpp
You'll need two programs to compile this library: meson and ninja (refer to the library's README.md file for details). Both are available as ubuntu packages so you just need to install them using apt or another package manager.  For example, for apt you can run the following command:

sudo apt install ninja-build meson

After you install these programs, run this sequence of commands in a terminal:

```bash
BUILD_TYPE=debug
LIB_TYPE=static
meson --buildtype ${BUILD_TYPE} --default-library ${LIB_TYPE} . build-${LIB_TYPE}
ninja -v -C build-${LIB_TYPE}
```

This will create the file libjsoncpp.a in the directory build-static.  Copy or move it to this project's lib directory.

## Final comments
With the two libraries above compiled and copied to the lib directory, run `make` in this project's root directory.  This will generate an executable called `wsmain` which will run the simulations.
