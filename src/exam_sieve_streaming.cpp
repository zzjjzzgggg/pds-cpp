/**
 * Copyright (C) by J.Z. 2018-08-24 21:27
 * Distributed under terms of the MIT license.
 */

#include "sieve_streaming.h"
#include "coverage_obj_fun.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input data file");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 1000, "end time");
DEFINE_double(eps, 0.2, "epsilon");
DEFINE_bool(save, true, "save results or not");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    auto obj_file = osutils::join(strutils::getBasePath(FLAGS_dat), "obj.gz");
    CoverageObjFun obj(obj_file);
    SieveStreaming sieve(FLAGS_B, FLAGS_eps, &obj);

    int t = 0;
    std::vector<std::tuple<int, double, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s\n", "time", "value", "#calls", "|Theta|");

    ioutils::TSVParser ss(FLAGS_dat);
    while (ss.next()) {
        sieve.feed(ss.get<int>(0));
        ++t;

        int calls = sieve.getOCalls();
        double val = sieve.getResult().second;
        rst.emplace_back(t, val, calls);

        printf("\t%-12d%-12.0f%-12d%-12d\r", t, val, calls,
               sieve.getNumThresholds());
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    if (FLAGS_save) {
        std::string ofnm = strutils::insertMiddle(
            FLAGS_dat, "SieveStreaming_k{}e{:g}"_format(FLAGS_B, FLAGS_eps),
            "dat");
        std::string ano = fmt::format(
            "#graph: {}\n#budget: {}\n#end time: {}\n#epsilon: {:.2f}\n",
            FLAGS_dat, FLAGS_B, FLAGS_T, FLAGS_eps);
        ioutils::saveTupleVec(rst, ofnm, true, "{}\t{:.1f}\t{}\n", ano);
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
