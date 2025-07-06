DESCRIPTION = "Custom image for RPi4 with libcamera, GStreamer, OpenCV, Wi-Fi, SSH"
LICENSE = "MIT"

inherit core-image

IMAGE_FEATURES += "ssh-server-openssh"

IMAGE_INSTALL += " \
    libcamera \
    v4l-utils \
    gstreamer1.0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    openssh \
    iw \
    wpa-supplicant \
    opencv \
    mosquitto \
    libgpiod \
    pir-mqtt \
    rpicam-apps \
    i2c-tools \
"
# x264
IMAGE_INSTALL += "x264 libcamera-gst"

IMAGE_BOOT_FILES += "bootfiles/*"
