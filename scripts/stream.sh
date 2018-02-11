#/bin/bash
rmmod uvcvideo
modprobe uvcvideo quirks=128 nodrop=1 timeout=6000

mjpg_streamer -i "/usr/lib/mjpg-streamer/input_uvc.so -d /dev/video0 -n -f 30 -r 640x360" -o "/usr/lib/mjpg-streamer/output_http.so -p 8090 -n -w ./www"

# http://192.168.88.16:8090/?action=stream