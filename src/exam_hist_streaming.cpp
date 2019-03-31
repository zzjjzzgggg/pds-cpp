/**
 * Copyright (C) by J.Z. 2018-08-25 11:11
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"
#include "lifespan_generator.h"
#include "bernoulli_segment.h"
#include "hist_streaming.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input data");
DEFINE_int32(L, 10, "maximum lifetime");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "B");
DEFINE_int32(T, 100, "end time");
DEFINE_double(p, 0.5, "decaying rate");
DEFINE_double(eps, 0.2, "epsilon");
DEFINE_bool(save, true, "save results or not");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    auto obj_file = osutils::join(strutils::getBasePath(FLAGS_dat), "obj.gz");
    CoverageObjFun obj(obj_file);
    LifespanGenerator lifegen(FLAGS_L, FLAGS_p);
    HistStreaming hist(FLAGS_n, FLAGS_B, FLAGS_eps, &obj);

    int t = 0;
    std::vector<std::tuple<int, double, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s\n", "time", "value", "#calls", "#algs");

    ioutils::TSVParser ss(FLAGS_dat);
    while (ss.next()) {
        ++t;
        int e = ss.get<int>(0);
        BernoulliSegments segs(lifegen.getTrialLifespanPairs(FLAGS_n));

        hist.feed(e, segs);

        int cost = hist.getCost();
        double val = hist.getResult();
        rst.emplace_back(t, val, cost);

        hist.reduce();
        hist.next();

        printf("\t%-12d%-12.0f%-12d%-12d\r", t, val, cost, hist.size());
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    // save results
    if (FLAGS_save) {
        std::string ofnm = strutils::insertMiddle(
            FLAGS_dat,
            "HistStreaming_K{}T{}e{:g}"_format(
                FLAGS_B, strutils::prettyNumber(FLAGS_T), FLAGS_eps),
            "dat");
        ioutils::saveTripletVec(rst, ofnm, true, "{}\t{:.2f}\t{}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
