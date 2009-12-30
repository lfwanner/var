#/bin/bash

APP_DIR=/home/lucas/var/sam3/apps/power_consumption/
PM="/usr/bin/python /home/lucas/lib/mclib/SCPI/power_measurement_suite.py "
SAM_PROG="/opt/sam-ba//sam3u-samba-program.py -b "
BOARDS=5
OUT_DIR="/home/lucas/var/tests/sleep_modes/output_no_dbgu"

cd $APP_DIR
make 
cd -
APP_BIN=`ls $APP_DIR/bin/*flash.bin`

MODES=(  -1-backup -2-wait  -3-sleep500hz -4-sleep32khz  -5-sleep125khz -6-sleep1mhz -7-sleep48mhz -8-active )


for b in  $(seq 1  $BOARDS )    
do
    echo "Please connect board $b and follow the programming instructions"
    $SAM_PROG $APP_BIN
    for m in "${MODES[@]}"
    do
        echo "Please set board $b to $m mode and hit return"
        read nothing
        $PM > $OUT_DIR/b$b$m
        echo 
    done
done
