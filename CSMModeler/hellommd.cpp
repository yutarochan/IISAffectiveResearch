#include "pmd_reader.h"
#include "vmd_reader.h"
#include "mmd_scene.h"
#include "mmd_math.h"

PMDModel* model = NULL;
VMDAnimation* anim = NULL;
MMDScene* scene = NULL;

char* pmdmodel = "data/pmd/input.pmd";
char* vmdmodel = "data/vmd/input.vmd";

PMDReader pmdreader;
model = pmdreader.LoadFromFile(pmdmodel);
assert(model);

VMDReader vmdreader;
anim = vmdreader.LoadFromFile(vmdmodel);
assert(anim);

MMDScene* scene = new MMDScene();
scene->SetModel(model);
scene->AttachAnimation(anim);
