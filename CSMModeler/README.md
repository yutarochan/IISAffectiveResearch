# MMDLoader

![MMDLoaderExample](https://github.com/syoyo/MMDLoader/blob/master/mmdview.gif?raw=true)

(c) model : http://mikumikudance.wikia.com/wiki/Miku_Hatsune_(Lat)
(c) motion: http://mmd.nomeu.org/thumbs/sm13622845

Simple MMD(PMD, VMD) loader in C++.
MMDLoader is written in portable manner. No dependency except for C++ STL.

## Compilation Procedures
To compile the sample program in this repo, perform the following.
Assumes you are on a UNIX based machine (Linux or Mac - only verified with Ubuntu though...)

1. Make sure you have the following dependencies already installed:
    * GCC, G++, etc
    * premake4
    * OpenGL/GLUT Libraries

2. Go into trackball.cpp and fix `#include "TrackBall.h"` to `#include "trackball.h"`.

3. Execute `premake gmake` to auto-generate a makefile. This should generate a file called MMDTest.make

4. Go into the generated makefile and append `-lGL -lGLU -lglut` at the very end of `LINKCMD`.
This ensures that the linker will properly link the OpenGL and GLUT dependencies upon compilation process.

## Usage

Simply copy `mmd_*`, `pmd_*` and `vmd_*` files to your project.

## Code sample in quick

```
#include "pmd_reader.h"
#include "vmd_reader.h"
#include "mmd_scene.h"
#include "mmd_math.h"

PMDModel* model = NULL;
VMDAnimation* anim = NULL;
MMDScene* scene = NULL;

char* pmdmodel = "input.pmd";
char* vmdmodel = "input.vmd";

PMDReader pmdreader;
model = pmdreader.LoadFromFile(pmdmodel);
assert(model);

VMDReader vmdreader;
anim = vmdreader.LoadFromFile(vmdmodel);
assert(anim);

MMDScene* scene = new MMDScene();
scene->SetModel(model);
scene->AttachAnimation(anim);
```

## Example

OpenGL(GLUT) example viewer is included(see viewer_main.cc).


## Features

Supported

* PMD model loading.
* VMD motion loading.
* Bone animation(IK).
* Morph.

TODOs
(Contributors welcome!)

* [ ] Physics(Bullet)

## Author

Syoyo Fujita(syoyo@lighttransport.com)

## License

3-clause BSD.
