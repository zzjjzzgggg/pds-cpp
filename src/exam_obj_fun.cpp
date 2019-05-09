/**
 * Copyright (C) by J.Z. 2019-05-01 19:39
 * Distributed under terms of the MIT license.
 */
#include "stdafx.h"

#define FEATURE_DIM 6

#include "active_set_obj_fun.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input file full path");
DEFINE_double(lambda, .5, "lambda");
int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    FeatureData dat;
    ioutils::TSVParser ss(FLAGS_dat);
    while (ss.next()) {
        int id = ss.get<int>(0);
        FeatureVector v;
        for (int i = 0; i < FEATURE_DIM; ++i) v(i) = ss.get<double>(i + 1);
        dat[id] = v;
        if (ss.getLineNO() > 10) break;
    }

    ActiveSetObjFun obj(FLAGS_lambda, &dat);

    double val = obj.getVal({0, 1});
    printf("val: %.4f\n", val);

    std::vector<int> S = {1};
    double gain = obj.getGain(0, S);
    printf("gain: %.4f\n", gain);

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
