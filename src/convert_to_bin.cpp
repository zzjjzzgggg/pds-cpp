/**
 * Copyright (C) by J.Z. 2019-04-17 09:05
 * Distributed under terms of the MIT license.
 *
 * Convert object to binary format.
 *
 */

#include "stdafx.h"
#include "coverage_obj_fun.h"

#include <gflags/gflags.h>

DEFINE_string(obj, "", "input obj data (two columns)");
int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    CoverageObjFun obj(FLAGS_obj);
    printf("load txt cost time %s\n", tm.getStr().c_str());

    auto bin_file =
        osutils::join(strutils::getBasePath(FLAGS_obj), "obj_bin.gz");
    obj.save(bin_file);

    tm.tick();
    CoverageObjFun obj2(bin_file, true);
    printf("load bin cost time %s\n", tm.getStr().c_str());

    gflags::ShutDownCommandLineFlags();
    return 0;
}
