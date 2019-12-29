/**
 * Copyright (C) by J.Z. 2018-08-25 10:20
 * Distributed under terms of the MIT license.
 */

#include "stdafx.h"
#include "active_set_obj_fun.h"
#include "bernoulli_segment.h"
#include "candidate.h"
#include "eval_stream.h"

#include "biased_reservoir_sampler.h"
#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(feature, "feature.gz", "feature data file name");
DEFINE_string(lifespans, "../lifespans/q{:g}n{}L{}.gz", "lifespans template");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(n, 20, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 2000, "end time");
DEFINE_double(q, .001, "decaying rate");
DEFINE_double(lambda, .5, "lambda");

int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    std::string lifespan_fnm =
        osutils::join(FLAGS_dir, fmt::format(FLAGS_lifespans, FLAGS_q, FLAGS_n,
                                             strutils::prettyNumber(FLAGS_L)));
    auto pin = ioutils::getIOIn(lifespan_fnm);

    FeatureData dat;
    auto feature_fnm = osutils::join(FLAGS_dir, FLAGS_feature);
    ioutils::TSVParser ss_cache(feature_fnm);
    while (ss_cache.next()) {
        int id = ss_cache.get<int>(0);
        FeatureVector v;
        for (int i = 0; i < FEATURE_DIM; ++i)
            v(i) = ss_cache.get<double>(i + 1);
        dat[id] = v;
        if (ss_cache.getLineNO() > FLAGS_T) break;
    }
    ActiveSetObjFun obj(FLAGS_lambda, &dat);

    EvalStream eval(FLAGS_L);
    BiasedReservoir sampler(10, -std::log(1 - FLAGS_q));
    BiasedReservoir unbiased_sampler(10, 0);

    int t = 0;
    std::vector<int> lifespans;
    Candidate solution(FLAGS_n);
    std::vector<std::tuple<int, double, double>> result;

    printf("\t%-12s%-12s%-12s\n", "time", "unbiased", "biased");

    ioutils::TSVParser ss(feature_fnm);
    while (t++ < FLAGS_T && ss.next()) {
        int e = ss.get<int>(0);
        lifespans.clear();
        pin->load(lifespans);
        BernoulliSegments segs(lifespans);
        eval.add(e, segs);

        sampler.process(e);
        unbiased_sampler.process(e);

        auto ele_bs = eval.getPop();

        solution.clear();
        for (int e : unbiased_sampler.reservoir_) {
            if (ele_bs.find(e) != ele_bs.end()) {
                solution.insert(e, ele_bs[e]);
            }
        }
        double val1 = solution.value(&obj);

        solution.clear();
        for (int e : sampler.reservoir_) {
            if (ele_bs.find(e) != ele_bs.end()) {
                solution.insert(e, ele_bs[e]);
            }
        }
        double val2 = solution.value(&obj);

        result.emplace_back(t, val1, val2);

        eval.next();

        printf("\t%-12d%-12.2f%-12.2f\r", t, val1, val2);
        fflush(stdout);
    }
    printf("\n");

    std::string ofnm = osutils::join(
        FLAGS_dir,
        "reservoir_sampling_q{:g}n{}K{}T{}.dat"_format(
            FLAGS_q, FLAGS_n, FLAGS_B, strutils::prettyNumber(FLAGS_T)));
    ioutils::saveTupleVec(result, ofnm, "{}\t{:.4f}\t{:.4f}\n");

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
