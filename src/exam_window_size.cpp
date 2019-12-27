/**
 * Copyright (C) by J.Z. 2019-12-27 08:51
 * Distributed under terms of the MIT license.
 */

#include "stdafx.h"
#include "coverage_obj_fun.h"
#include "simple_greedy.h"
#include "bernoulli_segment.h"
#include "eval_stream.h"
#include "candidate.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_string(lifespans, "../lifespans/q{:g}n{}L{}.gz", "lifespans template");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(W, 100, "window size");
DEFINE_int32(step, 100, "window size step");
DEFINE_int32(T, 2000, "end time");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(n, 50, "number of samples");
DEFINE_double(q, .001, "decaying rate");
DEFINE_bool(save, true, "save results or not");
DEFINE_bool(objbin, true, "is objective file in binary format");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    std::string lifespan_fnm =
        osutils::join(FLAGS_dir, fmt::format(FLAGS_lifespans, FLAGS_q, FLAGS_n,
                                             strutils::prettyNumber(FLAGS_L)));
    auto pin = ioutils::getIOIn(lifespan_fnm);

    CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    EvalStream eval(FLAGS_L);

    int t = 0;
    std::vector<int> lifespans, elements;
    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (t++ < FLAGS_T && ss.next()) {
        int e = ss.get<int>(0);
        lifespans.clear();
        pin->load(lifespans);
        BernoulliSegments segs(lifespans);
        eval.next();
        eval.add(e, segs);
        elements.push_back(e);
    }
    auto ele_bs = eval.getPop();

    std::vector<std::pair<int, double>> result;
    for (int w = FLAGS_W; w <= FLAGS_T; w += FLAGS_step) {
        std::vector<int> eles(elements.begin() + FLAGS_T - w, elements.end());

        SimpleGreedy greedy(FLAGS_B, &obj);
        greedy.run(eles);
        Candidate candidate(FLAGS_n);
        for (int e : greedy.chosen_) {
            if (ele_bs.find(e) != ele_bs.end()) {
                candidate.insert(e, ele_bs[e]);
            }
        }
        double val = candidate.value(&obj);
        result.emplace_back(w, val);
        printf("%d\t%.2f\n", w, val);
    }

    if (FLAGS_save) {
        std::string ofnm = osutils::join(
            FLAGS_dir,
            "window_size_q{:g}n{}K{}T{}.dat"_format(
                FLAGS_q, FLAGS_n, FLAGS_B, strutils::prettyNumber(FLAGS_T)));
        ioutils::savePrVec(result, ofnm, "{}\t{:.4f}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
