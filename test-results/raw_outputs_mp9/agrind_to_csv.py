import json
import csv
import fileinput
import sys

writer = csv.DictWriter(sys.stdout, fieldnames=["detector", "descriptor", "detect_ms", "desc_ms"])
writer.writeheader()
for line in fileinput.input():
    l = json.loads(line)
    if l:
        writer.writerow(l[0])