/**
 * Copyright (C) by J.Z. 2018-08-25 11:11
 * Distributed under terms of the MIT license.
 */

#include "stdafx.h"
#include "coverage_obj_fun.h"
#include "lifespan_generator.h"
#include "bernoulli_segment.h"
#include "hist_approx.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(lifespans, "", "lifespans file name full path");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_bool(objbin, true, "is objective file binary format?");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "B");
DEFINE_int32(T, 100, "end time");
DEFINE_double(q, .001, "decaying rate");
DEFINE_double(eps, 0.2, "epsilon");
DEFINE_bool(save, true, "save results or not");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("xxx");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    HistApprox hist(FLAGS_n, FLAGS_B, FLAGS_eps, &obj);

    // If lifespan file name is not empty and exists on disk, then read
    // lifespans from file; Otherwise, generate random lifespans.
    LifespanGenerator lifegen(FLAGS_L, FLAGS_q);
    auto pin = ioutils::getIOIn(FLAGS_lifespans);
    if (pin)
        printf("will read lifespans from file.\n");
    else
        printf("will generate random lifespans.\n");

    int t = 0;
    std::vector<int> lifespans;
    std::vector<std::tuple<int, double, int, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s\n", "time", "value", "#calls", "#algs");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (ss.next()) {
        ++t;
        int e = ss.get<int>(0);
        lifespans.clear();
        if (pin)
            pin->load(lifespans);
        else
            lifegen.getLifespans(FLAGS_n, lifespans);
        BernoulliSegments segs(lifespans);

        hist.feed(e, segs);

        int cost = hist.getCost();
        double val = hist.getResult();
        int num_algs = hist.size();
        rst.emplace_back(t, val, cost, num_algs);

        hist.reduce();
        hist.next();

        printf("\t%-12d%-12.0f%-12d%-12d\r", t, val, cost, num_algs);
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    // save results
    if (FLAGS_save) {
        std::string ofnm = osutils::join(
            FLAGS_dir,
            "HistApprox_K{}q{:g}e{:g}T{}.dat"_format(
                FLAGS_B, FLAGS_q, FLAGS_eps, strutils::prettyNumber(FLAGS_T)));
        ioutils::saveTupleVec(rst, ofnm, "{}\t{:.2f}\t{}\t{}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
