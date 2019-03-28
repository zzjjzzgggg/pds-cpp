/**
 * Copyright (C) by J.Z. 2019-03-27 10:53
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input data file");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    CoverageObjFun obj(FLAGS_dat);
    printf("%.1f\n", obj.getVal(4));

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
