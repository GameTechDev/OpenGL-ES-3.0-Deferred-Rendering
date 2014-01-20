# OpenGL ES 3.0 Deferred Renderer

This is a sample demonstrating how to create a deferred renderer on OpenGL ES 3.0 devices. The sample shows off three differerent renderers: forward rendering, deferred lighting and deferrred shading.

## Building the code

### Android

There's a makefile in `projects/android/` used to simplify calling all the Android commands. 

1. Run `make init` to call `android update project`, updating the project for your environment
2. `make all` will call both `ndk-build` and `ant debug`
3. `make install` to install the sample onto your device
4. `make run` runs the installed executable
5. `make kill` stops the executable

## Running the Sample

The sample has a few important controls:

* 1 finger pan - rotate camera
* 2 finger pan - pan camera (forward, backward, strafe)
* Tap top left quadrant - cycle between different renderers (forward, deferred lighting and deferred rendering)
* Tap bottom left quadrant - toggle the movement of the lights
* Tap top right quadrant - tobble between native device resolution and 720p

## Known Issues

* Toggling resolution on certain Android devices leads to inconsistant screen size.