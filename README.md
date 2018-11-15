# libmissilelauncher

## Overview

This library allows programatic access to some USB foam dart launchers. 
The library itself currently only supports one type of launcher. 
However it is designed to allow easy addition of additional types of launchers. 
If your type isn't supported, feel free to add the code in and submit a pull request!

## Compiling

### Supported Systems

* Linux
* OSX - Experimental (Needs Testing) 
* Windows - Experimental (Needs Work)

### Requirementes

This library depends on libusb-1.0.

### Instructions

This project uses CMake to manage building an what not. Simply change into the libmissilelauncher directory and follow the following instructions.

      mkdir build && cd build
      cmake .. -DCMAKE_BUILD_TYPE=Release

On Linux you can use CPack to make a nice distributable. 
I'm working on support for Windows and OSX. Run CPack --help for more info on CPack options.

## Examples

Checkout the examples directory for an annotated example.

There is also [Qt-Missile-Launcher](http://www.github.com/coderlane/Qt-Missile-Launcher)
which provides an example with a GUI.
