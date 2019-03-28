/**
 * Copyright (C) by J.Z. 2019-03-28 08:38
 * Distributed under terms of the MIT license.
 */

#include "lifespan_generator.h"
#include "bernoulli_segment.h"

#include <gflags/gflags.h>

DEFINE_string(dat, "", "input data");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(L, 1000, "maximum lifetime");
DEFINE_double(p, 0.5, "decaying rate");

void print_vec(const std::vector<int>& vec) {
    printf("{");
    for (int v : vec) printf("%d, ", v);
    printf("}\n");
}

int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    LifespanGenerator lifegen(FLAGS_L, FLAGS_p);

    ioutils::TSVParser ss(FLAGS_dat);
    while (ss.next() && ss.getLineNO() < 10) {
        int e = ss.get<int>(0);
        printf("\n%d:\n", e);
        BernoulliSegments bsegs(e, lifegen.getTrialLifespanPairs(FLAGS_n));
        for (const auto& seg : bsegs.getBernoulliSegments()) {
            printf("[%d, %d]: ", seg.start_, seg.end_);
            print_vec(seg.bs_);
        }
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
