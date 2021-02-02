/**
 * Copyright (C) by J.Z. 2019-05-01 19:39
 * Distributed under terms of the MIT license.
 */
#include "stdafx.h"

#include "tweet_obj_fun.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    TweetObjFun obj(osutils::join(FLAGS_dir, "obj_bin.gz"), true);
    double val = obj.getVal({0, 1});
    printf("val: %.4f\n", val);

    std::vector<int> S = {1};
    double gain = obj.getGain(0, S);
    printf("gain: %.4f\n", gain);

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
