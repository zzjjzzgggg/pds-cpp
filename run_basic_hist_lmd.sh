#!/usr/bin/env bash
# created on 2021-02-02 17:53


cmd=./build/exam_basic
for lmd in 0.01 0.02 0.03 0.04 0.05
do
    echo "lmd = $lmd"
    $cmd -dir ../tweet -B 10 -n 50 -L 100 -T 1000 -eps 0.2 -lmd $lmd
done

cmd=./build/exam_hist
for lmd in 0.01 0.02 0.03 0.04 0.05
do
    echo "lmd = $lmd"
    $cmd -dir ../tweet -B 10 -n 50 -L 100 -T 1000 -eps 0.2 -lmd $lmd
done
