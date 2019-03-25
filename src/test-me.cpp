/**
 * Copyright (C) by J.Z. 2018-08-16 10:54
 * Distributed under terms of the MIT license.
 */

#include "stdafx.h"
#include <gflags/gflags.h>

int main(int argc, char *argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
