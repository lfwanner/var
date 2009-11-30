#/bin/bash

APP_DIR=/home/lucas/variability/sam3/apps/power_consumption_atmel/
PM="/usr/bin/python /home/lucas//mclib/SCPI/power_measurement_suite.py "
SAM_PROG="/opt/sam-ba//sam3u-samba-program.py -b "
BOARDS=5
OUT_DIR="/home/lucas/variability/tests/sleep_modes/output"

cd $APP_DIR
make 
cd -
APP_BIN=`ls $APP_DIR/bin/*flash.bin`

MODES=( sleep500hz sleep125khz sleep1mhz sleep48mhz wait active backup  )

for b in  $(seq 2  $BOARDS )
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