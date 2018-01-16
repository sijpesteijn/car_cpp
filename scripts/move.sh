#!/bin/bash

count=0
min=1000000
max=1000000


while [ $count -le 40 ]
do
    duty=`echo "$(od -An -N4 -tu4 /dev/urandom) % $max + $min" | bc`
    (( count++ ))
    echo $duty
    echo $duty > /sys/class/pwm/pwmchip1/pwm$((RANDOM%2))/duty_cycle
    sleep 1
done
