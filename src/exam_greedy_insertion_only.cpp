/**
 * Copyright (C) by J.Z. 2018-08-24 21:52
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"
#include "bernoulli_set_generator.h"
#include "greedy_alg.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input data file");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 100, "end time");
DEFINE_double(p, 0.5, "decaying rate");
DEFINE_bool(save, true, "save results or not");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    auto obj_file = osutils::join(strutils::getBasePath(FLAGS_dat), "obj.gz");
    CoverageObjFun obj(obj_file);
    BernoulliSetGenerator bsgen(FLAGS_n, FLAGS_p);

    GreedyAlg greedy(FLAGS_n, FLAGS_B, &obj);

    int t = 0, naive_ocalls = 0;
    std::unordered_map<int, BernoulliSet> pop;
    std::vector<std::tuple<int, double, int, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s\n", "time", "value", "#calls", "#nv_calls");

    ioutils::TSVParser ss(FLAGS_dat);
    while (ss.next()) {
        ++t;
        int e = ss.get<int>(0);
        pop[e] = bsgen.getBernoulliSet();

        double val = greedy.run(pop);

        int calls = greedy.getCost();
        naive_ocalls += pop.size() * FLAGS_B;

        rst.emplace_back(t, val, calls, naive_ocalls);

        printf("\t%-12d%-12.2f%-12d%-12d\r", t, val, calls, naive_ocalls);
        fflush(stdout);

        if (t == FLAGS_T) break;
    }
    printf("\n");

    if (FLAGS_save) {
        // save results
        std::string ofnm = strutils::insertMiddle(
            FLAGS_dat, "greedy_insert_only_k{}"_format(FLAGS_B), "dat");
        std::string ano = "#graph: {}\nbudget: {}\n#end time: {}\n"_format(
            FLAGS_dat, FLAGS_B, FLAGS_T);
        ioutils::saveTupleVec(rst, ofnm, true, "{}\t{:.1f}\t{}\t{}\n", ano);
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
