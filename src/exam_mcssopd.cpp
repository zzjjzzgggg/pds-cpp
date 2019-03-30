/**
 * Copyright (C) by J.Z. 2018-08-25 10:06
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"
#include "lifespan_generator.h"
#include "bernoulli_segment.h"
#include "mc_ssopd.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input data");
DEFINE_int32(L, 10, "maximum lifetime");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "budget");
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
    MCSSOPD ssopd(FLAGS_L, FLAGS_n, FLAGS_B, FLAGS_eps, &obj);

    std::vector<std::tuple<int, double, int>> rst;

    printf("\t%-12s%-12s%-12s\n", "time", "value", "cost");

    int t = 0;
    ioutils::TSVParser ss(FLAGS_dat);
    while (ss.next()) {
        ++t;
        int e = ss.get<int>(0);
        BernoulliSegments segs(lifegen.getTrialLifespanPairs(FLAGS_n));

        ssopd.feed(e, segs);
        int cost = ssopd.getCost();
        double val = ssopd.getResult().second;

        ssopd.next();

        rst.emplace_back(t, val, cost);

        printf("\t%-12d%-12.0f%-12d\r", t, val, cost);
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    // save results
    if (FLAGS_save) {
        std::string ofnm = strutils::insertMiddle(
            FLAGS_dat,
            "MC-SSOPD_K{}T{}e{:g}"_format(
                FLAGS_B, strutils::prettyNumber(FLAGS_T), FLAGS_eps),
            "dat");
        ioutils::saveTripletVec(rst, ofnm, true, "{}\t{:.2f}\t{}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
