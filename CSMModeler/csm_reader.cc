#include <cassert>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <map>

//#include "vmd_reader.h"
//#include "pmd_model.h"

using namespace mmd;

static bool ReadBytes(unsigned char *out_data, int size, FILE *fp) {
    size_t sz;
    sz = fread(out_data, 1, size, fp);
    assert(sz == size);
    return true;
}
