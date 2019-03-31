/**
 * Copyright (C) by J.Z. 2018-08-16 10:54
 * Distributed under terms of the MIT license.
 */

#include "stdafx.h"

#include <iostream>

#include "lifespan_generator.h"
#include "bernoulli_segment.h"
#include "candidate.h"

#include <gflags/gflags.h>

// Merge two sorted vectors into one vector where duplicated elements are saved
// only once.
std::vector<int> merge_sorted_vectors(std::vector<int>& v1,
                                      std::vector<int>& v2) {
    std::vector<int> vec;
    vec.reserve(v1.size() + v2.size());
    int p1 = 0, p2 = 0;
    while (true) {
        // if p1 reaches the end, then copy v2[p2:] to vec
        if (p1 == v1.size()) {
            while (p2 < v2.size()) vec.push_back(v2[p2++]);
            break;
        }
        // else if p2 reaches the end, then copy v1[p1:] to vec
        if (p2 == v2.size()) {
            while (p1 < v1.size()) vec.push_back(v1[p1++]);
            break;
        }
        // otherwise, both p1 and p2 do not point at ends
        if (v1[p1] == v2[p2]) {
            vec.push_back(v1[p1]);
            ++p1;
            ++p2;
        } else if (v1[p1] < v2[p2]) {
            vec.push_back(v1[p1]);
            ++p1;
        } else {
            vec.push_back(v2[p2]);
            ++p2;
        }
    }
    return vec;
}

void print_vec(const std::vector<int>& vec) {
    printf("{");
    for (int v : vec) printf("%d, ", v);
    printf("}\n");
}

void test_merge() {
    std::vector<int> v1 = {0, 3, 17, 130};
    std::vector<int> v2 = {0, 3, 17, 19, 20, 130, 131};

    auto vec = merge_sorted_vectors(v1, v2);
    print_vec(vec);
}

void test_lifespan() {
    LifespanGenerator sampler(1000, 0.3);

    std::map<int, int> hist;
    for (int n = 0; n < 10000; ++n) hist[sampler.get()]++;

    for (auto p : hist) {
        std::cout << p.first << ' ' << std::string(p.second / 100, '*') << '\n';
    }
}

void test_segments(const std::vector<int>& lifespans) {
    // a vector of (trial, lifespan) pairs
    std::vector<std::pair<int, int>> samples;
    samples.reserve(lifespans.size());
    int i = 0;
    for (auto lifespan : lifespans) samples.emplace_back(i++, lifespan);

    BernoulliSegments segs(std::move(samples));

    for (const auto& seg : segs.segments_) {
        printf("[%d, %d): ", seg.start_, seg.end_);
        print_vec(seg.bs_);
    }
}

void test_candidate_copy() {
    Candidate c1;
    c1.init(10);
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
    int i = 1;
    int j = 2, l = i;
    if (i == --j || ++l == j) {
        ++i;
    }
    printf("i=%d, j=%d, l=%d\n", i, j, l);
}

int main(int argc, char* argv[]) {
    gflags::SetUsageMessage("usage:");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    osutils::Timer tm;

    // test_lifespan();
    // test_segments({1, 4, 6, 4});
    // test_segments({41, 301, 27, 128, 10});
    // test_candidate_copy();
    test();

    printf("cost time %s\n", tm.getStr().c_str());
    gflags::ShutDownCommandLineFlags();
    return 0;
}
