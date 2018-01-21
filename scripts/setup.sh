#!/bin/bash

config-pin P9.21 pwm
config-pin P9.22 pwm

cd /sys/class/pwm/pwmchip1/
echo 0 > export
echo 1 > export

cd ./pwm0
echo 20000000 > period
echo 1480000 > duty_cycle
echo 1 > enable

cd ../pwm1
echo 20000000 > period
echo 1480000 > duty_cycle
echo 1 > enable

mjpg_streamer -i "/usr/lib/mjpg-streamer/input_uvc.so -d /dev/video0 -n -f 30 -r 640x360" -o "/usr/lib/mjpg-streamer/output_http.so -p 8090 -n -w ./www"
