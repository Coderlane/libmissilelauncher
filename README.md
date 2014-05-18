C-Launcher-API
==============

##Overview

This library allows programatic access to some USB foam dart launchers. The library itself currently only supports one type of launcher. However it is designed to allow easy addition of additional types of launchers. If your type isn't supported, feel free to add the code in and submit a pull request!


##Compiling

###Supported Systems

* Linux
* OSX - Experimental (Needs Testing) 
* Windows - Experimental (Needs Work)

###Requirementes

This library depends on libusb-1.0.

###Instructions

Use cmake to generate makefiles for your system. On Linux and OSX it is as easy as:

`
mkdir release && cd release
ccmake ..
`

Work on Windows support is ongoing, however with cygwin it should be the same as Linux or OSX.

##Examples

I'm working on a few basic examples.
