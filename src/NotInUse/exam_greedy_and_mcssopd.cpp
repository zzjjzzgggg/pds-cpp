/**
 * Copyright (C) by J.Z. 2018-08-25 10:20
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"
#include "lifespan_generator.h"
#include "bernoulli_segment.h"
#include "mc_ssopd.h"
#include "eval_stream.h"
#include "greedy_alg.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_int32(L, 10, "maximum lifetime");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 100, "end time");
DEFINE_double(p, 0.5, "decaying rate");
DEFINE_double(eps, 0.2, "epsilon");
DEFINE_bool(save, true, "save results or not");
DEFINE_bool(objbin, true, "is objective file in binary format");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("xxx");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    LifespanGenerator lifegen(FLAGS_L, FLAGS_p);
    MCSSOPD ssopd(FLAGS_L, FLAGS_n, FLAGS_B, FLAGS_eps, &obj);
    EvalStream eval(FLAGS_L);
    GreedyAlg greedy(FLAGS_n, FLAGS_B, &obj);

    int t = 0;
    std::unordered_map<int, BernoulliSet> pop;
    std::vector<std::tuple<int, double, int>> mc_rst, greedy_rst;

    printf("\t%-12s%-12s%-12s%-12s%-12s\n", "time", "val_ssopd", "val_greedy",
           "cost_ssopd", "cost_greedy");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (ss.next()) {
        ++t;
        int e = ss.get<int>(0);
        BernoulliSegments segs(lifegen.getTrialLifespanPairs(FLAGS_n));

        // MC-SSOPD
        ssopd.feed(e, segs);
        int mc_cost = ssopd.getCost();
        double mc_val = ssopd.getResult().second;

        // Greedy
        eval.add(e, segs);
        int greedy_cost = greedy.getCost();
        double greedy_val = greedy.run(eval.getPop());

        ssopd.next();
        eval.next();

        mc_rst.emplace_back(t, mc_val, mc_cost);
        greedy_rst.emplace_back(t, greedy_val, greedy_cost);

        printf("\t%-12d%-12.2f%-12.2f%-12d%-12d\r", t, mc_val, greedy_val,
               mc_cost, greedy_cost);
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    if (FLAGS_save) {
        auto sT = strutils::prettyNumber(FLAGS_T);
        // MC-SSOPD
        std::string ofnm = osutils::join(
            FLAGS_dir,
            "MC-SSOPD_k{}e{:g}T{}.dat"_format(FLAGS_B, FLAGS_eps, sT));
        ioutils::saveTupleVec(mc_rst, ofnm, true, "{}\t{:.2f}\t{}\n");

        // Greedy
        ofnm = osutils::join(FLAGS_dir,
                             "greedy_pds_k{}T{}.dat"_format(FLAGS_B, sT));
        ioutils::saveTupleVec(greedy_rst, ofnm, true, "{}\t{:.2f}\t{}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
