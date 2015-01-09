C-Launcher-API
==============

##Overview

This library allows programatic access to some USB foam dart launchers. 
The library itself currently only supports one type of launcher. 
However it is designed to allow easy addition of additional types of launchers. 
If your type isn't supported, feel free to add the code in and submit a pull request!

##Compiling

###Supported Systems

* Linux
* OSX - Experimental (Needs Testing) 
* Windows - Experimental (Needs Work)

###Requirementes

This library depends on libusb-1.0.

###Instructions

Use cmake to generate makefiles for your system. On Linux, OSX, and Cygwin it is as easy as:

      mkdir build && cd build
      ccmake .. # Configure to your desire

On Linux and Cygwin you can use cpack to make a nice distributable. 
I'm working on support for Windows and OSX. Run cpack --help for more info on cpack options.

##Examples

Checkout the examples directory for an annotated example.

There is also [Qt-Missile-Launcher] (http://www.github.com/coderlane/Qt-Missile-Launcher)
which provides an example with a GUI.
