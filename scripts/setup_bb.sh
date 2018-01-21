#!/bin/bash

echo Update ubuntu
apt-get update
apt-get -y upgrade

# Format sd card

(maybe: apt-get install parted)

parted /dev/mmcblk0 -s "mklabel GPT"
parted /dev/mmcblk0 -s "mkpart primary 0% 100%"
mkfs.ext2 /dev/mmcblk0p1

echo Mounting sd card
mkdir /media/card
sudo mount -t ext4 -o rw /dev/mmcblk0p1 /media/card

echo Moving stuff to sdcard
rm -rf /home
rm -rf /root
rm -rf /var/cache
mkdir /media/card/notouch
mkdir /media/card/notouch/home
mkdir /media/card/notouch/root
mkdir /media/card/notouch/var
mkdir /media/card/notouch/var/cache
ln -s /media/card/notouch/home /home
ln -s /media/card/notouch/root /root
ln -s /media/card/notouch/var/cache /var/cache
mount --bind /media/card/notouch/home /home
mount --bind /media/card/notouch/root /root
mount --bind /media/card/notouch/var/cache /var/cache

blkid
ls -l /dev/disk/by-uuid
vi /etc/fstab
UUID=614e8846-4aa6-4e7e-8b97-48f0ebe4e816 /media/card ext2 defaults,rw,auto,user,exec 0 0

echo Installing opencv

git clone https://github.com/opencv/opencv.git
apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
cd opencv
mkdir build
cd build
cmake -DWITH_OPENCL=NO -DWITH_OPENCL_SVM=NO -DWITH_OPENCLAMDFFT=NO -DWITH_OPENCLAMDBLAS=NO -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make
make install
ldconfig

echo Installing restbed

git clone https://github.com/Corvusoft/restbed.git
apt-get -y install libssl-dev
git submodule init
git submodule update
mkdir build
cd build
cmake -DBUILD_SHARED=YES -DBUILD_EXAMPLES=NO -DBUILD_TESTS=NO ..
make install

mkdir /usr/local/include/restbed
cp -R ../distribution/include/* /usr/local/include/restbed
cp ../distribution/library/librestbed.so.4.6.0 /usr/local/lib
ln -s /usr/local/lib/librest.so.4.6.0 /usr/local/lib/librestbed.so.4
ln -s /usr/local/lib/librest.so.4 /usr/local/lib/librestbed.so
ldconfig

echo Installing jansson
apt-get -y install libjansson-dev

echo Installing wifi dongle TL-WN823N

apt-get install linux-headers-$(uname -r)
git clone https://github.com/Mange/rtl8192eu-linux-driver.git
cd rtl8192eu-linux-driver
make ARCH=arm
make install

echo Finished