/**
 * Copyright (C) by J.Z. 2018-08-25 10:20
 * Distributed under terms of the MIT license.
 */

#include "active_set_obj_fun.h"
#include "lifespan_generator.h"
#include "bernoulli_segment.h"

#include "eval_stream.h"
#include "greedy_alg.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(feature, "feature.gz", "feature data file name");
DEFINE_string(lifespans, "", "lifespans file name full path");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 100, "end time");
DEFINE_double(q, .001, "decaying rate");
DEFINE_double(lambda, .5, "lambda");
DEFINE_bool(save, true, "save results or not");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    FeatureData dat;
    auto feature_fnm = osutils::join(FLAGS_dir, FLAGS_feature);
    ioutils::TSVParser ss(feature_fnm);
    while (ss.next()) {
        int id = ss.get<int>(0);
        FeatureVector v;
        for (int i = 0; i < FEATURE_DIM; ++i) v(i) = ss.get<double>(i + 1);
        dat[id] = v;
        if (ss.getLineNO() > FLAGS_T) break;
    }

    ActiveSetObjFun obj(FLAGS_lambda, &dat);

    GreedyAlg greedy(FLAGS_n, FLAGS_B, &obj);
    EvalStream eval(FLAGS_L);

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
    std::vector<std::tuple<int, double, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s\n", "time", "value", "#cost", "|V|");

    ioutils::TSVParser ss2(feature_fnm);
    while (t++ < FLAGS_T && ss2.next()) {
        int e = ss2.get<int>(0);
        lifespans.clear();
        if (pin)
            pin->load(lifespans);
        else
            lifegen.getLifespans(FLAGS_n, lifespans);
        BernoulliSegments segs(lifespans);

        eval.add(e, segs);

        auto pop = eval.getPop();
        double val = greedy.run(pop);
        int cost = greedy.getCost();

        eval.next();

        rst.emplace_back(t, val, cost);

        printf("\t%-12d%-12.2f%-12d%-12lu\r", t, val, cost, pop.size());
        fflush(stdout);
    }
    printf("\n");

    if (FLAGS_save) {
        // save results
        std::string ofnm = osutils::join(
            FLAGS_dir,
            "greedy_pds_n{}K{}q{:g}T{}.dat"_format(
                FLAGS_n, FLAGS_B, FLAGS_q, strutils::prettyNumber(FLAGS_T)));
        ioutils::saveTupleVec(rst, ofnm, "{}\t{:.4f}\t{}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
