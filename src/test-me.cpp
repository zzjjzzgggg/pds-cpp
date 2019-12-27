/**
 * Copyright (C) by J.Z. 2018-08-16 10:54
 * Distributed under terms of the MIT license.
 */

#include "stdafx.h"

#include <iostream>

#include "lifespan_generator.h"
#include "bernoulli_segment.h"
#include "candidate.h"

#include <Eigen/Dense>
#include <gflags/gflags.h>

void test_lifespan() {
    LifespanGenerator sampler(1000, 0.3);

    std::map<int, int> hist;
    for (int n = 0; n < 10000; ++n) hist[sampler.get()]++;

    for (auto p : hist) {
        std::cout << p.first << ' ' << std::string(p.second / 100, '*') << '\n';
    }
}

void test_segments() {
    LifespanGenerator lifegen(1000, 0.01);
    auto lifespans = lifegen.getLifespans(10);
    BernoulliSegments segs(lifespans);

    for (const auto& seg : segs.segments_) {
        printf("[%d, %d): ", seg.start_, seg.end_);
        // print_vec(seg.bs_);
    }

    printf("max idx: %d\n", segs.getMxIdx());
}

void test_candidate_copy() {
    Candidate c1(10);
    c1.insert(1, {1, 3, 5});
    c1.insert(2, {0, 2, 3});
    printf("\nc1:\n");
    c1.info();

    Candidate c2 = c1;
    printf("\nc2:\n");
    c2.info();

    c2.insert(3, {0, 1, 2});
    printf("\nc2 inserted:\n");
    c2.info();

    printf("\nc1:\n");
    c1.info();
}

void test() {
    std::list<double> arr = {1, .9, .8, .7, .65, .6, .5, .4, .3, .2, .1};
    auto i = arr.begin();
    while (i != arr.end()) {
        printf("*i=%g, ", *i);
        auto j = i, l = i;
        double bound = (*i) * .8;
        while (++j != arr.end() && (*j) >= bound)
            ;
        if (j == arr.end()) break;
        printf("*j=%g\n", *j);
        if (--j == i || ++l == j) {
            ++i;
            continue;
        }
        arr.erase(++i, j);
        i = j;

        printf("\n");
        for (double x : arr) printf("%g, ", x);
        printf("\n");
    }
    printf("\n");
    for (double x : arr) printf("%g, ", x);
    printf("\n");
}

void test_eigen() {
    Eigen::MatrixXd m(2, 2);
    m(0, 0) = 3;
    m(1, 0) = 2.5;
    m(0, 1) = -1;
    m(1, 1) = m(1, 0) + m(0, 1);
    std::cout << m << std::endl;
}

int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    // test_lifespan();
    // test_segments();
    // test_segments({41, 301, 27, 128, 10});
    // test_candidate_copy();
    // test();
    // test_eigen();
    printf("%d\n", FEATURE_DIM);

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
