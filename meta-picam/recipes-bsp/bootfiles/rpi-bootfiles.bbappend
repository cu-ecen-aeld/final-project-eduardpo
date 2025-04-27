FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://usercfg.txt"

IMAGE_BOOT_FILES:append = " usercfg.txt"
