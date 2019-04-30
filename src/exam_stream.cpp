/**
 * Copyright (C) by J.Z. 2019-03-28 08:38
 * Distributed under terms of the MIT license.
 */

#include "lifespan_generator.h"
#include "bernoulli_segment.h"

#include <gflags/gflags.h>

DEFINE_string(dir, "", "working directory");
DEFINE_int32(n, 10, "number of samples");
DEFINE_int32(L, 5000, "maximum lifetime");
DEFINE_int32(T, 200, "stream end time");
DEFINE_double(q, .001, "decaying rate");
DEFINE_bool(echo, false, "echo");

int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    LifespanGenerator lifegen(FLAGS_L, FLAGS_q);

    auto filename = osutils::join(
        FLAGS_dir, "q{:g}n{}L{}T{}_bin.gz"_format(
                       FLAGS_q, FLAGS_n, strutils::prettyNumber(FLAGS_L),
                       strutils::prettyNumber(FLAGS_T)));
    auto pout = ioutils::getIOOut(filename);
    int t = 0;
    while (t++ < FLAGS_T) {
        auto lifespans = lifegen.getLifespans(FLAGS_n);
        pout->save(lifespans);
        if (FLAGS_echo) ioutils::printVec(lifespans);
    }
    printf("Saved to %s.\n", filename.c_str());

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
