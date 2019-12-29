/**
 * Copyright (C) by J.Z. 2019-04-30 15:32
 * Distributed under terms of the MIT license.
 */
#include "biased_reservoir_sampler.h"
#include <gflags/gflags.h>

DEFINE_double(q, 0.01, "q in [0,1), q=0 becomes unbiased reservoir sampling");

int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    double lambda = -std::log(1 - FLAGS_q);

    int R = 1000, N = 100;
    std::map<int, int> hist;
    for (int k = 0; k < R; ++k) {
        BiasedReservoir sampler(10, lambda);
        for (int e = 0; e < N; ++e) sampler.process(e);
        for (int s : sampler.reservoir_) ++hist[s / (N / 10)];
    }

    // visulaize hist
    double mx = 0;
    for (auto& pr : hist)
        if (pr.second > mx) mx = pr.second;
    for (int i = 0; i < 10; ++i) {
        printf("%d: ", i);
        for (int j = 0; j < hist[i] * 20 / mx; ++j) printf("*");
        printf("\n");
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
