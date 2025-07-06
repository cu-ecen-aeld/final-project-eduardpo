# Enable GStreamer plugin
PACKAGECONFIG:append = " gst"

PACKAGECONFIG[gst] = "-Dgstreamer=enabled,-Dgstreamer=disabled,gstreamer1.0 gstreamer1.0-plugins-base"

# Ensure the plugin is packaged
FILES:${PN}-gst = "${libdir}/gstreamer-1.0"

# GStreamer plugin runtime dependencies (minimal and correct)
RDEPENDS:${PN}-gst += " \
    gstreamer1.0-plugins-base \
"
