#/bin/bash

APP_DIR=/home/lucas/var/sam3/apps/power_consumption_fastrc/
PM="/usr/bin/python /home/lucas/lib/mclib/SCPI/power_measurement_suite.py "
SAM_PROG="/opt/sam-ba//sam3u-samba-program.py -b "
BOARDS=5
OUT_DIR="/home/lucas/var/tests/clock_fastrc/output"

cd $APP_DIR
make 
cd -
APP_BIN=`ls $APP_DIR/bin/*flash.bin`

MODES=( 00125 00250 00500 01000 02000 04000 08000 12000 )


for b in  $(seq 1  $BOARDS )    
do
    echo "Please connect board $b and follow the programming instructions"
    $SAM_PROG $APP_BIN
    for m in "${MODES[@]}"
    do
        echo "Please set board $b to $m mode and hit return"
        read nothing
        $PM > $OUT_DIR/b$b-$m
        echo 
    done
done
