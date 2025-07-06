# d@DellEdi:~/workspace/build_rpi$ cat /home/ed/workspace/build_rpi/tmp/work/cortexa72-poky-linux/gstreamer1.0-plugins-ugly/1.20.7-r0/temp/log.do_configure | grep x264
# NOTE: Executing meson -Dnls=enabled -Dgpl=enabled -Ddoc=disabled -Dsidplay=disabled -Da52dec=enabled -Damrnb=disabled -Damrwbdec=disabled -Dcdio=disabled -Ddvdread=disabled -Dmpeg2dec=enabled -Dorc=enabled -Dx264=disabled...
# Dependency x264 skipped: feature x264 disabled
#     x264                  : disabled
# ed@DellEdi:~/workspace/build_rpi
#
# Force the x264 plugin to be compiled in:
PACKAGECONFIG:append = " x264"