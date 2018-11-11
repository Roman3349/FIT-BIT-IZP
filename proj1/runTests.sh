#!/usr/bin/env bash

for d in test/*/ ; do
    echo "$d starts"
    ./../bin/proj1 ${d}/cmd < ${d}/test.in > ${d}/test.output
    echo "$d outputs $?"
    diff --color ${d}/test.out ${d}/test.output
    rm ${d}/test.output
done