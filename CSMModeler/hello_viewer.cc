#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include <iostream>
#include <vector>
#include <cmath>

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef ENABLE_BULLET
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#endif

#include <assert.h>
#include "trackball.h"

#include "pmd_reader.h"
#include "vmd_reader.h"
#include "mmd_scene.h"
#include "mmd_math.h"

// image loader
//#include "stb_image.c"

using namespace mmd;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/*
 * global variable
 */
static int mouse_x, mouse_y; /* x, y location		*/
static float curr_quat[4];   /* current quaternion state	*/
static float prev_quat[4];   /* previous quaternion state	*/
static int mouse_moving;     /* Is mouse moving?		*/
static int spinning;         /* Is scene spinning?		*/
static int width, height;    /* current window width&height	*/
static float zoom;           /* zoom factor			*/
static int mouse_r_pressed;  /* Is right mouse pressed?	*/
static int mouse_m_pressed;  /* Is middle mouse pressed?	*/
static GLfloat view_org[3];  /* view origin			*/
static GLfloat view_tgt[3];  /* view target			*/
static float bmin[3] = {-1, -1, -1}, bmax[3] = {1, 1, 1};
static float center[3] = {0.0, 0.0, 0.0};
static float maxval;
static float scenesize = 20.0f;
static int current_frame = 0;
static int sub_frame = 0;
static int frame_step = 5; // less = faster playback.
static bool do_animate = true;

PMDModel *model = NULL;
VMDAnimation *anim = NULL;
MMDScene *scene = NULL;

float *renderVertices = NULL;

static void reshape(int w, int h);
static void animate();

void load(char *pmdmodel, char *vmdmodel) {
    PMDReader pmdreader;
    model = pmdreader.LoadFromFile(pmdmodel);
    assert(model);

    // TODO: Subvert from loading a VMD file and instead load CSM file.
    // Instead parse it and load it as a VMD data structure.
    /*
    VMDReader vmdreader;
    anim = vmdreader.LoadFromFile(vmdmodel);
    assert(anim);

    MMDScene *scene = new MMDScene();
    scene->SetModel(model);
    scene->AttachAnimation(anim);

    renderVertices = new float[3 * model->vertices_.size()];

    DumpIK();
    DumpBone(); */
}


int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s input.pmd input.vmd\n", argv[0]);
        exit(-1);
    }

    // Initially load dataset and analyze bone structure
}
