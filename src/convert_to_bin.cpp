/**
 * Copyright (C) by J.Z. 2019-04-17 09:05
 * Distributed under terms of the MIT license.
 *
 * Convert object to binary format.
 *
 */

#include "stdafx.h"
// #include "coverage_obj_fun.h"
#include "tweet_obj_fun.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    // CoverageObjFun obj(FLAGS_obj);
    TweetObjFun obj(osutils::join(FLAGS_dir, "obj.gz"));
    printf("load txt cost time %s\n", tm.getStr().c_str());

    auto bin_file = osutils::join(FLAGS_dir, "obj_bin.gz");
    obj.save(bin_file);

    tm.tick();
    // CoverageObjFun obj2(bin_file, true);
    TweetObjFun obj2(bin_file, true);
    printf("load bin cost time %s\n", tm.getStr().c_str());

    gflags::ShutDownCommandLineFlags();
    return 0;
}
