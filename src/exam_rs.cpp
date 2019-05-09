/**
 * Copyright (C) by J.Z. 2019-04-30 15:32
 * Distributed under terms of the MIT license.
 */
#include "biased_reservoir_sampler.h"
#include <gflags/gflags.h>

DEFINE_string(data, "", "input file full path");
DEFINE_int32(capacity, 100, "reservoir capacity");
DEFINE_int32(T, 100, "end time");
DEFINE_int32(step, 100, "save step");
DEFINE_double(lambda, .001, "lambda = 1 - p");
int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("xxx");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    BiasedReservoir sampler(FLAGS_capacity, FLAGS_lambda);
    for (int e = 0; e < FLAGS_T; ++e) {
        sampler.process(e);
        if ((e + 1) % FLAGS_step == 0) {
            sampler.sort();
            auto fnm = osutils::join(strutils::getBasePath(FLAGS_data),
                                     "samples_lmd{:g}cap{}_{}.dat"_format(
                                         FLAGS_lambda, FLAGS_capacity, e + 1));
            auto ptr_out = ioutils::getIOOut(fnm);
            int cur = 0;
            char line[256];
            auto ptr_in = ioutils::getIOIn(FLAGS_data);
            for (int ln : sampler.reservoir_) {
                for (int i = 0; i < ln - cur; ++i) ptr_in->readLine(line, 256);
                auto sz = ptr_in->readLine(line, 256);
                cur = ln + 1;
                ptr_out->write(line, sz);
            }
            printf("saved to %s\n", fnm.c_str());
            sampler.shuffle();
        }
    }

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
