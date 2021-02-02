/**
 * Copyright (C) by J.Z. 2018-08-25 10:20
 * Distributed under terms of the MIT license.
 */

// #include "coverage_obj_fun.h"
#include "tweet_obj_fun.h"
#include "bernoulli_segment.h"

#include "eval_stream.h"
#include "greedy_alg.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(lifespans, "../lifespans/lmd{:g}n{}L{}.gz", "lifespans template");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(n, 50, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 2000, "end time");
DEFINE_double(lmd, .01, "decaying rate");
DEFINE_bool(objbin, true, "is objective file in binary format");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    std::string lifespan_fnm =
        osutils::join(FLAGS_dir, fmt::format(FLAGS_lifespans, FLAGS_lmd, FLAGS_n,
                                             strutils::prettyNumber(FLAGS_L)));
    auto pin = ioutils::getIOIn(lifespan_fnm);

    // CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    TweetObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    GreedyAlg greedy(FLAGS_n, FLAGS_B, &obj);
    EvalStream eval(FLAGS_L);

    int t = 0;
    std::vector<int> lifespans;
    std::vector<std::tuple<int, double, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s\n", "time", "value", "#cost", "|V|");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (t++ < FLAGS_T && ss.next()) {
        int e = ss.get<int>(0);
        lifespans.clear();
        pin->load(lifespans);
        BernoulliSegments segs(lifespans);

        eval.add(e, segs);

        auto pop = eval.getPop();
        double val = greedy.run(pop);
        int cost = greedy.getCost();
        rst.emplace_back(t, val, cost);

        eval.next();

        printf("\t%-12d%-12.2f%-12d%-12lu\r", t, val, cost, pop.size());
        fflush(stdout);
    }
    printf("\n");

    // save results
    std::string ofnm = osutils::join(
        FLAGS_dir,
        "greedy_lmd{:g}n{}K{}T{}.dat"_format(FLAGS_lmd, FLAGS_n, FLAGS_B,
                                             strutils::prettyNumber(FLAGS_T)));
    ioutils::saveTupleVec(rst, ofnm, "{}\t{:.4f}\t{}\n");

    // greedy.saveSolution(osutils::join(FLAGS_dir, "greedy.dat"));

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
