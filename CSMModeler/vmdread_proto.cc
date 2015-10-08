/*
 * VMD File Analysis Prototype
 * Goal: To Understand the Structure of a VMD file and attempt to inject a csm
 *       data into a vmd data structure.
 * To see more information on this file format, go to http://mikumikudance.wikia.com/wiki/VMD_file_format
 * Developed by: Yuya Ong
 */

#include <cassert>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <map>
#include <vector>
#include <string>

#include "vmd_animation.h"
#include "vmd_reader.h"
#include "pmd_model.h"

#include "utf8.h"

using namespace mmd;

const bool VERBOSE = true;

struct MotionData {
    char name[20];
    std::vector<VMDMotion> motions_;
};

static bool ReadBytes(unsigned char *out_data, int size, FILE *fp) {
    size_t sz;
    sz = fread(out_data, 1, size, fp);
    assert(sz == size);
    return true;
}

int main(int argc, char **argv) {
    // Check CLI Arguments for Filename
    if (argc < 2) {
        printf("Usage: %s motion_file.vmd\n", argv[0]);
        exit(-1);
    }

    // Attempt to Read File from Specified Directory
    if (VERBOSE) printf("[DEBUG] Attempting to open file, %s.\n", argv[1]);
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        fprintf(stderr, "Can't read VMD file [ %s ]\n", argv[1]);
        exit(-1);
    }
    if (VERBOSE) printf("[DEBUG] File %s successfully opened..\n", argv[1]);

    // Create MotionData Object
    MotionData data;

    /*
     * File Header Metadata Analysis
     */
    // The "Magic" Byte Detection
    if (VERBOSE) printf("[DEBUG] Searching for initial Magic Byte in file...\n");
    const char kMagic[] = "Vocaloid Motion Data 0002";
    char header[30];
    ReadBytes(reinterpret_cast<unsigned char *>(header), 30, fp);
    int ret = strncmp(kMagic, header, strlen(kMagic));
    assert(ret == 0);
    if (VERBOSE && (ret == 0)) printf("[DEBUG] Magic Byte Found - Valid VMD File\n");

    // Get Model Name from VMD File
    char name[20];
    ReadBytes(reinterpret_cast<unsigned char *>(name), 20, fp);
    strncpy(data.name, name, 20);
    printf("[VMDReader] VMD Name = %s\n", data.name);

    /*
     * VMD Motion Parse
     */
    // Analyze Number of Motion Frames
    int numMotions;
    ReadBytes(reinterpret_cast<unsigned char *>(&numMotions), 4, fp);
    printf("[VMDReader] Number Motion Frames: %d\n", numMotions);
    assert(sizeof(VMDMotion) == 111);

    // Resize Motion Vector
    data.motions_.resize(numMotions);
    ReadBytes(reinterpret_cast<unsigned char *>(&(data.motions_[0])), sizeof(VMDMotion) * numMotions, fp);

    // Motion data may not be ordererd in frame number. Sort it here.
    // std::sort(data.motions_.begin(), data.motions_.end(), VMDMotionSorter);
}
