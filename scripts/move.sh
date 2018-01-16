#!/bin/bash

count=0
min=1150000
max=1810000


while [ $count -le 400 ]
do
    duty=$((RANDOM%$max+$min))
    (( count++ ))
    echo $duty > /sys/class/pwm/pwmchip1/pwm$((RANDOM%1))/duty_cycle
    sleep 1
done
