# carmageddon

#flash beaglebone

download latest image: https://beagleboard.org/latest-images

unzip using 'The Unarchiver'

sudo dd bs=1m if=bone-debian-9.3-iot-armhf-2018-01-07-4gb.img of=/dev/disk2

insert in beaglebone, once booted edit uEnv.txt and  uncomment cmdline=init=/opt/scripts/tools/eMMC/init-eMMC-flasher-v3.sh

reboot (with flash button pressed) and wait for flashing to finish

#wifi

connmanctl
agent on
services
connect <wifi_id>

#install libraries

follow instructions in setup_bb.sh

#webcam

rmmod uvcvideo
modprobe uvcvideo quirks=128 nodrop=1 timeout=6000

ffmpeg -i /dev/video0 -r 30 -s 640x480 -f v4l2 http://localhost:8090/feed1.ffm

g++ -I/usr/local/include/opencv -I/usr/local/include/restbed -std=c++11 -lrestbed -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -o restbed.o restbed.cpp
