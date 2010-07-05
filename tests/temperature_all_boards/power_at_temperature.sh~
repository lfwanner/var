#/bin/bash

APP_DIR=/home/lucas/var/sam3/apps/power_consumption_atmel/
PM="/usr/bin/python /home/lucas/lib/mclib/SCPI/power_measurement_suite.py "
SAM_PROG="/opt/sam-ba//sam3u-samba-program.py -b "
BOARDS=10
OUT_DIR="./output"

cd $APP_DIR
make 
cd -
APP_BIN=`ls $APP_DIR/bin/*flash.bin`



#-1-backup -2-wait  -3-sleep500hz  -5-sleep125khz -6-sleep1mhz -7-sleep48mhz -8-active )

echo "Please enter the board number"
read b
echo "Please connect board $b and follow the programming instructions"
$SAM_PROG $APP_BIN
echo "Please set board $b to sleep mode (32 KHz) and hit return"
read nothing
while true
do
  echo "When ready to measure, hit return"
  read nothing
  echo "Enter temperature"
  read temperature
  $PM > $OUT_DIR/b$b$temperature
  echo 
done

