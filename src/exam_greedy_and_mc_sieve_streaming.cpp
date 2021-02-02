/**
 * Copyright (C) by J.Z. 2019-03-29 10:59
 * Distributed under terms of the MIT license.
 */

// #include "coverage_obj_fun.h"
#include "tweet_obj_fun.h"
#include "bernoulli_set_generator.h"
#include "mc_sieve_streaming.h"
#include "greedy_alg.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_bool(objbin, true, "is objective file binary format?");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 100, "end time");
DEFINE_double(q, 0.001, "decaying rate");
DEFINE_double(eps, 0.1, "epsilon");
DEFINE_bool(save, true, "save results or not");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("xxxx");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    // CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    TweetObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    BernoulliSetGenerator bsgen(FLAGS_n, FLAGS_q);

    MCSieveStreaming sieve(FLAGS_n, FLAGS_B, FLAGS_eps, &obj);
    GreedyAlg greedy(FLAGS_n, FLAGS_B, &obj);

    int t = 0;
    std::unordered_map<int, BernoulliSet> pop;
    std::vector<std::tuple<int, double, int, double, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s%-12s\n", "time", "val_ss", "val_greedy",
           "cost_ss", "cost_greedy");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
    while (ss.next()) {
        ++t;
        int e = ss.get<int>(0);
        BernoulliSet bs = pop[e] = bsgen.getBernoulliSet();
        // ioutils::printVec(bs);

        // MC-SieveStreaming
        sieve.feed(e, pop[e]);
        int mcss_cost = sieve.getCost();
        double mcss_val = sieve.getResult().second;

        // Greedy
        int greedy_cost = greedy.getCost();
        double greedy_val = greedy.run(pop);

        rst.emplace_back(t, greedy_val, greedy_cost, mcss_val, mcss_cost);

        printf("\t%-12d%-12.2f%-12.2f%-12d%-12d\r", t, mcss_val, greedy_val,
               mcss_cost, greedy_cost);
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    if (FLAGS_save) {
        auto sT = strutils::prettyNumber(FLAGS_T);
        std::string ofnm = osutils::join(FLAGS_dir, "insertion_only.dat");
        ioutils::saveTupleVec(rst, ofnm, "{}\t{:.2f}\t{}\t{:.2f}\t{}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
