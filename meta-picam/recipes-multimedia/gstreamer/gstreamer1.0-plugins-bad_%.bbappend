# Enable the libcamerasrc plugin from gstreamer-bad
PACKAGECONFIG:append = " libcamera"

DEPENDS += "libcamera"
