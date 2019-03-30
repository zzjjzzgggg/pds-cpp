/**
 * Copyright (C) by J.Z. 2019-03-29 10:59
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"
#include "bernoulli_set_generator.h"

#include "mc_sieve_streaming.h"
#include "greedy_alg.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input data file");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 100, "end time");
DEFINE_double(p, 0.5, "decaying rate");
DEFINE_double(eps, 0.2, "epsilon");
DEFINE_bool(save, true, "save results or not");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    auto obj_file = osutils::join(strutils::getBasePath(FLAGS_dat), "obj.gz");
    CoverageObjFun obj(obj_file);
    BernoulliSetGenerator bsgen(FLAGS_n, FLAGS_p);

    MCSieveStreaming sieve(FLAGS_n, FLAGS_B, FLAGS_eps, &obj);
    GreedyAlg greedy(FLAGS_n, FLAGS_B, &obj);

    int t = 0;
    std::unordered_map<int, BernoulliSet> pop;
    std::vector<std::tuple<int, double, int>> mcss_rst, greedy_rst;

    printf("\t%-12s%-12s%-12s%-12s%-12s\n", "time", "val_ss", "val_greedy",
           "cost_ss", "cost_greedy");

    ioutils::TSVParser ss(FLAGS_dat);
    while (ss.next()) {
        ++t;
        int e = ss.get<int>(0);
        BernoulliSet bs = pop[e] = bsgen.getBernoulliSet();

        // MC-SieveStreaming
        sieve.feed(e, pop[e]);
        int mcss_cost = sieve.getCost();
        double mcss_val = sieve.getResult().second;
        mcss_rst.emplace_back(t, mcss_val, mcss_cost);

        // Greedy
        int greedy_cost = greedy.getCost();
        double greedy_val = greedy.run(pop);
        greedy_rst.emplace_back(t, greedy_val, greedy_cost);

        printf("\t%-12d%-12.2f%-12.2f%-12d%-12d\r", t, mcss_val, greedy_val,
               mcss_cost, greedy_cost);
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    if (FLAGS_save) {
        auto sT = strutils::prettyNumber(FLAGS_T);
        // MC-SieveStreaming
        std::string ofnm = strutils::insertMiddle(
            FLAGS_dat,
            "MC-SieveStreaming_k{}e{:g}T{}"_format(FLAGS_B, FLAGS_eps, sT),
            "dat");
        std::string ano = fmt::format(
            "#graph: {}\n#budget: {}\n#end time: {}\n#epsilon: {:.2f}\n",
            FLAGS_dat, FLAGS_B, FLAGS_T, FLAGS_eps);
        ioutils::saveTupleVec(mcss_rst, ofnm, true, "{}\t{:.2f}\t{}\n", ano);

        // Greedy
        ofnm = strutils::insertMiddle(
            FLAGS_dat, "greedy_insert_only_k{}T{}"_format(FLAGS_B, sT), "dat");
        ano = "#graph: {}\nbudget: {}\n#end time: {}\n"_format(
            FLAGS_dat, FLAGS_B, FLAGS_T);
        ioutils::saveTupleVec(greedy_rst, ofnm, true, "{}\t{:.2f}\t{}\n", ano);
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
