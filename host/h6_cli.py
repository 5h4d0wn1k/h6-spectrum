#!/usr/bin/env python3
"""H6 - Spectrum Analyzer host helper: offline channel-energy analysis.
Receive-only fixture analysis. Educational/authorized own-lab use only.
"""
import argparse
import os
import sys

MOD = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, MOD)
from hw_common import DEMO_TAG, read_target


def analyze(text):
    rows = []
    for line in text.splitlines():
        line = line.strip()
        if line.startswith("Ch") and "[" in line and "]" in line:
            try:
                ch = int(line[2:].split("[")[0].strip())
                val = float(line.split("[")[1].split("]")[0])
                rows.append((ch, val))
            except (ValueError, IndexError):
                continue
    return rows


def busiest(rows):
    if not rows:
        return None
    return max(rows, key=lambda r: r[1])


def run_demo():
    print("=== H6 WiFi channel energy (offline) ===")
    rows = analyze(read_target(
        "fixtures/energy.log",
        "Ch1 [-81.0]\nCh6 [-61.0]\nCh11 [-74.0]\n"))
    for ch, v in rows:
        bar = "#" * max(0, int((v + 100) / 80 * 20))
        print("  Ch%2d %6.1f dBm %s" % (ch, v, bar))
    b = busiest(rows)
    if b:
        print("  Busiest: Ch%d (%.1f dBm)" % b)
    print(DEMO_TAG)
    return 0


def main(argv=None):
    p = argparse.ArgumentParser(
        description="H6 Spectrum Analyzer - offline energy analysis")
    p.add_argument("--demo", action="store_true", help="offline demo (exit 0)")
    p.add_argument("--file", help="energy log path")
    args = p.parse_args(argv)
    text = read_target("fixtures/energy.log")
    if args.file:
        text = open(args.file).read()
    if args.demo or not args.file:
        return run_demo()
    rows = analyze(text)
    for row in rows:
        print(row)
    return 0


if __name__ == "__main__":
    sys.exit(main())
