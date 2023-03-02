#!/bin/bash
# Useful script to run an agrind (https://github.com/rcoh/angle-grinder) pipeline
# on all files in a directory (generally a raw test outputs directory).
# This one gets the median and p90 matched number of keypoints,
# assuming the raw input is on a source/reference basis for all 10 images.
set -euo pipefail
# agrind -f test_results_AKAZE_AKAZE.txt '* | parse "* detection * keypoints in * ms * descriptor extraction in * ms" as detector,ignore,detect_ms,descriptor,desc_ms | fields except ignore | p50(detect_ms), p50(desc_ms) by detector,descriptor'
for filename in `ls *.txt`
do
agrind --output json -f "$filename" '* | parse "* detection * keypoints in * ms * descriptor extraction in * ms" as detector,ignore,detect_ms,descriptor,desc_ms | fields except ignore | p50(detect_ms) as detect_ms, p50(desc_ms) as desc_ms by detector,descriptor'
done
