#!/bin/bash
saveIFS=$IFS
IFS=,
while read -r -a line
do
    printf "$line\n"
#    printf '%s,%s,%d,%d\n' "${line[*]:0:3}" "${line[3]:0:6}" "0x${line[3]:6:4}" "0x${line[3]:10:4}"
#    printf '%d\n' "0x${line:1:2}"
#    printf '%d\n' "0x33"
    IFS=' '
    read -ra newarr <<< "$line"
    # Print each value of the array by using
    # the loop
    segmentcount=0
    command='play -n -V1 -q -c1 synth '
    for val in "${newarr[@]}";
    do
        if [ "${val}" != "00" ]; then
	    newval=$(printf '%d' "0x$val")
            newval=$(expr $newval % 52 - 26)
            segment=$(printf ' sine %%%d ' "$newval")
            command="${command}${segment}"
            ((segmentcount++))
        fi
    done
    command="${command} fade q 0.9 .1 0.5 &"
    printf '%s\n' "${command}"
#    printf '%s\n' "${segmentcount}"
    if [ "${segmentcount}" != "0" ]; then
	eval $command
    else
        printf "\n"
    fi
    printf "\n\n"
    IFS=,
done
IFS=$saveIFS
