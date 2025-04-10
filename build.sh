#!/bin/bash
# Script to build image for qemu.
# Author: Siddhant Jajoo.

git submodule init
git submodule sync
git submodule update

# local.conf won't exist until this step on first execution
source poky/oe-init-build-env

CONFLINE="MACHINE = \"raspberrypi4-64\""

cat conf/local.conf | grep "${CONFLINE}" > /dev/null
local_conf_info=$?

if [ $local_conf_info -ne 0 ];then
	echo "Append ${CONFLINE} in the local.conf file"
	echo ${CONFLINE} >> conf/local.conf
	
else
	echo "${CONFLINE} already exists in the local.conf file"
fi


bitbake-layers show-layers | grep "meta-aesd" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding meta-aesd layer"
	bitbake-layers add-layer ../meta-aesd
else
	echo "meta-aesd layer already exists"
fi

bitbake-layers show-layers | grep "poky/meta" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding poky/meta layer"
	bitbake-layers add-layer ../poky/meta
else
	echo "poky/meta layer already exists"
fi

bitbake-layers show-layers | grep "poky/meta-poky" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding poky/meta-poky layer"
	bitbake-layers add-layer ../poky/meta-poky
else
	echo "poky/meta-poky layer already exists"
fi

bitbake-layers show-layers | grep "poky/meta-yocto-bsp" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding poky/meta-yocto-bsp layer"
	bitbake-layers add-layer ../poky/meta-yocto-bsp
else
	echo "poky/meta-yocto-bsp layer already exists"
fi

bitbake-layers show-layers | grep "meta-openembedded/meta-oe" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding meta-openembedded/meta-oe layer"
	bitbake-layers add-layer ../meta-openembedded/meta-oe
else
	echo "meta-openembedded/meta-oe layer already exists"
fi

bitbake-layers show-layers | grep "meta-openembedded/meta-python" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding meta-openembedded/meta-python layer"
	bitbake-layers add-layer ../meta-openembedded/meta-python
else
	echo "meta-openembedded/meta-python layer already exists"
fi

bitbake-layers show-layers | grep "meta-openembedded/meta-multimedia" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding meta-openembedded/meta-multimedia layer"
	bitbake-layers add-layer ../meta-openembedded/meta-multimedia
else
	echo "meta-openembedded/meta-multimedia layer already exists"
fi

bitbake-layers show-layers | grep "meta-openembedded/meta-networking" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding meta-openembedded/meta-networking layer"
	bitbake-layers add-layer ../meta-openembedded/meta-networking
else
	echo "meta-openembedded/meta-networking layer already exists"
fi

bitbake-layers show-layers | grep "meta-raspberrypi" > /dev/null
layer_info=$?

if [ $layer_info -ne 0 ];then
	echo "Adding meta-raspberrypi layer"
	bitbake-layers add-layer ../meta-raspberrypi
else
	echo "meta-raspberrypi layer already exists"
fi

set -e
bitbake core-image-aesd
