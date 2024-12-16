#pragma once

#include <iostream>

#include "core/glmimport.h"
#include "fastwfc/tiling_wfc.hpp"
#include "fastwfc/overlapping_wfc.hpp"
#include "fastwfc/utils/array2D.hpp"
#include "fastwfc/wfc.hpp"
#include "core/util/randomutils.h"

class WFC2 {
public:
    WFC2() { }

    void run(void);
};