#!/bin/bash
play -n -V1 -q -c1 synth sin %-3 sin %-2 fade q 1 1 1
tshark -i ap0 -l -f "udp port 9003 or udp port 9002" -x |  cut -d\  -f 2-18 | ~/WifiZomaticNetwork/scripts/sharkToSound.sh
