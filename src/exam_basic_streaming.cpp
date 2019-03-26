/**
 * Copyright (C) by J.Z. 2018-08-25 10:06
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"
#include "basic_streaming.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input data");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 1000, "end time");
DEFINE_int32(L, 1000, "maximum lifetime");
DEFINE_double(eps, 0.1, "epsilon");
DEFINE_bool(save, true, "save results or not");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    auto obj_file = osutils::join(strutils::getBasePath(FLAGS_dat), "obj.gz");
    CoverageObjFun obj(obj_file);
    BasicStreaming basic(FLAGS_L, FLAGS_B, FLAGS_eps, &obj);

    std::vector<std::tuple<int, double, int>> rst;

    printf("\t%-12s%-12s%-12s\n", "time", "value", "#calls");

    int t = 0;
    ioutils::TSVParser ss(FLAGS_dat);
    while (ss.next()) {
        int v = ss.get<int>(0), l = ss.get<int>(1);
        ++t;

        basic.feed(v, l);
        int calls = basic.statOCalls();
        double val = basic.getResult().second;

        basic.next();

        rst.emplace_back(t, val, calls);

        printf("\t%-12d%-12.0f%-12d\r", t, val, calls);
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    // save results
    if (FLAGS_save) {
        std::string ofnm = strutils::insertMiddle(
            FLAGS_dat,
            "basic_streaming_K{}T{}e{:g}"_format(
                FLAGS_B, strutils::prettyNumber(FLAGS_T), FLAGS_eps),
            "dat");
        ioutils::saveTripletVec(rst, ofnm, true, "{}\t{:.2f}\t{}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
