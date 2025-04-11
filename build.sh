#!/bin/bash
# Script to build image for qemu.
# Author: Siddhant Jajoo.

git submodule init
git submodule sync
git submodule update

# local.conf won't exist until this step on first execution
source poky/oe-init-build-env

CONFLINE="MACHINE = \"qemuarm64\""
#CONFLINE="MACHINE = \"raspberrypi4-64\""

cat conf/local.conf | grep "${CONFLINE}" > /dev/null
local_conf_info=$?

if [ $local_conf_info -ne 0 ];then
	echo "Append ${CONFLINE} in the local.conf file"
	echo ${CONFLINE} >> conf/local.conf
	
else
	echo "${CONFLINE} already exists in the local.conf file"
fi

# bitbake-layers show-layers | grep "meta-aesd" > /dev/null
# layer_info=$?

# if [ $layer_info -ne 0 ];then
# 	echo "Adding meta-aesd layer"
# 	bitbake-layers add-layer ../meta-aesd
# else
# 	echo "meta-aesd layer already exists"
# fi

declare -a layer_specs=(
  "meta-aesd|../meta-aesd"
  "poky/meta|../poky/meta"
  "poky/meta-poky|../poky/meta-poky"
  "poky/meta-yocto-bsp|../poky/meta-yocto-bsp"
  "meta-openembedded/meta-oe|../meta-openembedded/meta-oe"
  "meta-openembedded/meta-python|../meta-openembedded/meta-python"
  "meta-openembedded/meta-multimedia|../meta-openembedded/meta-multimedia"
  "meta-openembedded/meta-networking|../meta-openembedded/meta-networking"
  "meta-raspberrypi|../meta-raspberrypi"
)

for spec in "${layer_specs[@]}"; do
  IFS='|' read -r layer_name layer_path <<< "$spec"
  if ! (bitbake-layers show-layers 2>/dev/null | grep -q "$layer_name"); then
    echo "Adding $layer_name layer"
    bitbake-layers add-layer "$layer_path"
  else
    echo "$layer_name layer already exists"
  fi
done

set -e
bitbake core-image-minimal
#bitbake core-image-base
#bitbake core-image-aesd
