/**
 * Copyright (C) by J.Z. 2018-08-25 10:20
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"
#include "lifespan_generator.h"
#include "bernoulli_segment.h"
#include "candidate.h"
#include "eval_stream.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(lifespans, "", "lifespans file name full path");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 100, "end time");
DEFINE_int32(R, 10, "repeat");
DEFINE_double(q, .001, "decaying rate");
DEFINE_bool(save, true, "save results or not");
DEFINE_bool(objbin, true, "is objective file in binary format");

int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    EvalStream eval(FLAGS_L);
    Candidate chosen;
    rngutils::default_rng rng;

    // If lifespan file name is not empty and the file exists, then read
    // lifespans from file; Otherwise, generate random lifespans.
    LifespanGenerator lifegen(FLAGS_L, FLAGS_q);
    auto pin = ioutils::getIOIn(FLAGS_lifespans);
    if (pin)
        printf("will read lifespans from file.\n");
    else
        printf("will generate random lifespans.\n");

    int t = 0;
    std::vector<int> lifespans;
    std::vector<std::pair<int, double>> rst;

    printf("\t%-12s%-12s%-12s\n", "time", "value", "|V|");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (t++ < FLAGS_T && ss.next()) {
        int e = ss.get<int>(0);
        lifespans.clear();
        if (pin)
            pin->load(lifespans);
        else
            lifegen.getLifespans(FLAGS_n, lifespans);
        BernoulliSegments segs(lifespans);

        eval.add(e, segs);
        auto pop = eval.getPop();
        eval.next();

        double avg_val = 0;
        for (int rpt = 0; rpt < FLAGS_R; ++rpt) {
            chosen.clear();
            chosen.init(FLAGS_n);

            // randomly choose B elements from population
            if (pop.size() > FLAGS_B) {
                std::vector<int> elements;
                for (auto& pr : pop) elements.push_back(pr.first);
                auto samples = rngutils::choose(elements, FLAGS_B, rng);
                for (int s : samples) chosen.insert(s, pop[s]);
            } else {
                for (auto& pr : pop) chosen.insert(pr.first, pr.second);
            }

            avg_val += chosen.value(obj);
        }
        avg_val /= FLAGS_R;

        rst.emplace_back(t, avg_val);

        printf("\t%-12d%-12.2f%-12lu\r", t, avg_val, pop.size());
        fflush(stdout);
    }
    printf("\n");

    if (FLAGS_save) {
        // save results
        std::string ofnm =
            osutils::join(FLAGS_dir, "random_n{}K{}R{}q{:g}T{}.dat"_format(
                                         FLAGS_n, FLAGS_B, FLAGS_R, FLAGS_q,
                                         strutils::prettyNumber(FLAGS_T)));
        ioutils::savePrVec(rst, ofnm, "{}\t{:.4f}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
