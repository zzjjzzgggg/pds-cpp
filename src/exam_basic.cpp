/**
 * Copyright (C) by J.Z. 2018-08-25 10:06
 * Distributed under terms of the MIT license.
 */

// #include "coverage_obj_fun.h"
#include "tweet_obj_fun.h"
#include "bernoulli_segment.h"
#include "mc_ssopd.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(lifespans, "../lifespans/lmd{:g}n{}L{}.gz", "lifespans template");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_bool(objbin, true, "is objective file binary format?");
DEFINE_int32(L, 100, "maximum lifetime");
DEFINE_int32(n, 50, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 1000, "end time");
DEFINE_double(lmd, .01, "decaying rate");
DEFINE_double(eps, 0.2, "epsilon");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("xxxx");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    // CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    TweetObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    MCSSOPD ssopd(FLAGS_L, FLAGS_n, FLAGS_B, FLAGS_eps, &obj);

    std::string lifespan_fnm =
        osutils::join(FLAGS_dir, fmt::format(FLAGS_lifespans, FLAGS_lmd, FLAGS_n,
                                             strutils::prettyNumber(FLAGS_L)));
    printf("%s\n", lifespan_fnm.c_str());
    auto pin = ioutils::getIOIn(lifespan_fnm);

    int t = 0;
    std::vector<int> lifespans;
    std::vector<std::tuple<int, double, int>> rst;

    printf("\t%-12s%-12s%-12s\n", "time", "value", "cost");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (ss.next()) {
        ++t;
        int e = ss.get<int>(0);
        lifespans.clear();
        pin->load(lifespans);
        BernoulliSegments segs(lifespans);

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
    std::string ofnm =
        osutils::join(FLAGS_dir, "basic_lmd{:g}n{}K{}e{:g}T{}.dat"_format(
                                     FLAGS_lmd, FLAGS_n, FLAGS_B, FLAGS_eps,
                                     strutils::prettyNumber(FLAGS_T)));
    ioutils::saveTripletVec(rst, ofnm, "{}\t{:.4f}\t{}\n");

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
