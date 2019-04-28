/**
 * Copyright (C) by J.Z. 2018-08-24 21:52
 * Distributed under terms of the MIT license.
 */

#include "coverage_obj_fun.h"
#include "bernoulli_set_generator.h"
#include "greedy_alg.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_string(stream, "stream.gz", "input streaming data file name");
DEFINE_string(obj, "obj_bin.gz", "objective file name");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(B, 10, "budget");
DEFINE_int32(T, 100, "end time");
DEFINE_double(p, 0.5, "decaying rate");
DEFINE_bool(save, true, "save results or not");
DEFINE_bool(objbin, true, "is objective file binary format?");

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    CoverageObjFun obj(osutils::join(FLAGS_dir, FLAGS_obj), FLAGS_objbin);
    BernoulliSetGenerator bsgen(FLAGS_n, FLAGS_p);

    GreedyAlg greedy(FLAGS_n, FLAGS_B, &obj);

    int t = 0, naive_ocalls = 0;
    std::unordered_map<int, BernoulliSet> pop;
    std::vector<std::tuple<int, double, int, int>> rst;

    printf("\t%-12s%-12s%-12s%-12s\n", "time", "value", "#calls", "#nv_calls");

    ioutils::TSVParser ss(osutils::join(FLAGS_dir, FLAGS_stream));
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
        std::string ofnm = osutils::join(
            FLAGS_dir, "greedy_insert_only_K{}p{:g}T{}.dat"_format(
                           FLAGS_B, FLAGS_p, strutils::prettyNumber(FLAGS_T)));
        ioutils::saveTupleVec(rst, ofnm, true, "{}\t{:.1f}\t{}\t{}\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
