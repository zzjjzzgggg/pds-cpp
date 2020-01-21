/**
 * Copyright (C) by J.Z. 2018-08-25 10:20
 * Distributed under terms of the MIT license.
 */

#include "candidate.h"
#include "coverage_obj_fun.h"
#include "bernoulli_segment.h"
#include "eval_stream.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(lifespans, "../lifespans/lmd{:g}n{}L{}.gz", "lifespans template");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(n, 50, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 2000, "end time");
DEFINE_int32(R, 20, "repeat");
DEFINE_double(lmd, .01, "decaying rate");
DEFINE_bool(objbin, true, "is objective file in binary format");
int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    EvalStream eval(FLAGS_L);
    Candidate chosen(FLAGS_n);
    rngutils::default_rng rng;

    std::string lifespan_fnm = osutils::join(
        FLAGS_dir, fmt::format(FLAGS_lifespans, FLAGS_lmd, FLAGS_n,
                               strutils::prettyNumber(FLAGS_L)));
    auto pin = ioutils::getIOIn(lifespan_fnm);

    int t = 0;
    std::vector<int> lifespans;
    std::vector<std::pair<int, double>> rst;

    printf("\t%-12s%-12s%-12s\n", "time", "value", "|V|");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (t++ < FLAGS_T && ss.next()) {
        int e = ss.get<int>(0);
        lifespans.clear();
        pin->load(lifespans);
        BernoulliSegments segs(lifespans);

        eval.add(e, segs);
        auto pop = eval.getPop();
        eval.next();

        double avg_val = 0;
        for (int rpt = 0; rpt < FLAGS_R; ++rpt) {
            chosen.clear();

            // randomly choose B elements from population
            if (pop.size() > FLAGS_B) {
                std::vector<int> elements;
                for (auto& pr : pop) elements.push_back(pr.first);
                auto samples = rngutils::choose(elements, FLAGS_B, rng);
                for (int s : samples) chosen.insert(s, pop[s]);
            } else {
                for (auto& pr : pop) chosen.insert(pr.first, pr.second);
            }

            avg_val += chosen.value(&obj);
        }
        avg_val /= FLAGS_R;

        rst.emplace_back(t, avg_val);

        printf("\t%-12d%-12.2f%-12lu\r", t, avg_val, pop.size());
        fflush(stdout);
    }
    printf("\n");

    // save results
    std::string ofnm =
        osutils::join(FLAGS_dir, "random_lmd{:g}n{}K{}R{}T{}.dat"_format(
                                     FLAGS_lmd, FLAGS_n, FLAGS_B, FLAGS_R,
                                     strutils::prettyNumber(FLAGS_T)));
    ioutils::savePrVec(rst, ofnm, "{}\t{:.4f}\n");

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
