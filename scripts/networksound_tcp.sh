#!/bin/bash
play -n -V1 -q -c1 synth sawtooth %-4 sin %-3 fade q 1 1 1
tshark -i ap0 -l -f "tcp port 80" -x |  cut -d\  -f 2-18 | ~/WifiZomaticNetwork/scripts/sharkToSound_tcp.sh
