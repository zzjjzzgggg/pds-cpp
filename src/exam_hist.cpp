/**
 * Copyright (C) by J.Z. 2018-08-25 11:11
 * Distributed under terms of the MIT license.
 */

#include "stdafx.h"
#include "coverage_obj_fun.h"
#include "bernoulli_segment.h"
#include "hist_approx.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(lifespans, "../lifespans/lmd{:g}n{}L{}.gz", "lifespans template");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_bool(objbin, true, "is objective file binary format?");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(n, 50, "number of samples");
DEFINE_int32(B, 10, "B");
DEFINE_int32(T, 2000, "end time");
DEFINE_double(lmd, .01, "decaying rate");
DEFINE_double(eps, 0.2, "epsilon");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("xxx");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    HistApprox hist(FLAGS_n, FLAGS_B, FLAGS_eps, &obj);

    std::string lifespan_fnm = osutils::join(
        FLAGS_dir, fmt::format(FLAGS_lifespans, FLAGS_lmd, FLAGS_n,
                               strutils::prettyNumber(FLAGS_L)));
    auto pin = ioutils::getIOIn(lifespan_fnm);

    int t = 0;
    std::vector<int> lifespans;
    std::vector<std::tuple<int, double, int, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s\n", "time", "value", "#calls", "#algs");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (t++ < FLAGS_T && ss.next()) {
        int e = ss.get<int>(0);
        lifespans.clear();
        pin->load(lifespans);
        BernoulliSegments segs(lifespans);

        hist.feed(e, segs);

        int cost = hist.getCost();
        double val = hist.getResult();
        int num_algs = hist.size();
        rst.emplace_back(t, val, cost, num_algs);

        hist.reduce();
        hist.next();

        printf("\t%-12d%-12.2f%-12d%-12d\r", t, val, cost, num_algs);
        fflush(stdout);
    }
    printf("\n");

    // save results
    std::string ofnm =
        osutils::join(FLAGS_dir, "hist_lmd{:g}n{}K{}e{:g}T{}.dat"_format(
                                     FLAGS_lmd, FLAGS_n, FLAGS_B, FLAGS_eps,
                                     strutils::prettyNumber(FLAGS_T)));
    ioutils::saveTupleVec(rst, ofnm, "{}\t{:.4f}\t{}\t{}\n");

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
