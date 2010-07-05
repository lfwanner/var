#/bin/bash


PM="/usr/bin/python /home/lucas/lib/mclib/SCPI/power_measurement_suite.py "
OCTAVE="/usr/bin/octave"
OUT_DIR="./output"
rm $OUT_DIR/*

COUNTER=0
while [  $COUNTER -lt 10 ]; do
	TIME=`date +%s%N`
	TIME=${TIME:1:13}
	$PM > $OUT_DIR/tmp
	`$OCTAVE avg.m > /dev/null`
	AV=`cat output/ac output/av | tr '\n' ' '`
	echo $TIME $AV > output/tav
	cat output/tav >> time_series
#	let COUNTER=COUNTER+1 
done



