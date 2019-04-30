/**
 * Copyright (C) by J.Z. 2019-04-30 15:32
 * Distributed under terms of the MIT license.
 */
#include "biased_reservoir_sampler.h"
#include <gflags/gflags.h>

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    BiasedReservoir sampler(10);
    for (int i = 0; i < 100; ++i) {
        sampler.process(i);
        sampler.echo();
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
