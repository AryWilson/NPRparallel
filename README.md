# NPRparallel

## Sumamry

This repository launches a window that displays nonphotorealistic rendering (NPR) filters over images.


## How to build

*Windows*

Open git bash to the directory containing this repository.

```
NPRparallel $ mkdir build
NPRparallel $ cd build
NPRparallel/build $ cmake ..
NPRparallel/build $ start Viewer.sln
```

Run from the git bash command shell, 

```
NPRparallel/build $ ../bin/Debug/Viewer.exe
```

*macOS*

Open terminal to the directory containing this repository.

```
NPRparallel $ mkdir build
NPRparallel $ cd build
NPRparallel/build $ cmake ..
NPRparallel/build $ make
```

To run each program from build, you would type

```
NPRparallel/build $ ../bin/Viewer
```

## Controls

 - Use the right and left arrows to switch between filters

