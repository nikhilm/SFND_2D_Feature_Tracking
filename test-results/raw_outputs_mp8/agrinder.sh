#!/bin/bash
# Useful script to run an agrind (https://github.com/rcoh/angle-grinder) pipeline
# on all files in a directory (generally a raw test outputs directory).
# This one gets the median and p90 matched number of keypoints,
# assuming the raw input is on a source/reference basis for all 10 images.
set -euxo pipefail
for filename in `ls *.txt`
do
agrind -f "$filename" '* | parse "Detector * Descriptor * Source * Reference * (kNN) with n=* matches" as detector,descriptor,source,reference,n | p51(n) as median, p90(n) as p90 by detector,descriptor'
done
