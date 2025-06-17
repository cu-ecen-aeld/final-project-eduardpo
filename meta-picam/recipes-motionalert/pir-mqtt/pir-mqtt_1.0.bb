ESCRIPTION = "PIR-MQTT Motion alert"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI += "file://pir_mqtt.cpp file://pir-mqtt.service file://config.ini"

DEPENDS = "mosquitto libgpiod"


do_compile() {
    ${CXX} ${WORKDIR}/pir_mqtt.cpp -o pir_mqtt -lmosquitto -lgpiod -Wl,--hash-style=gnu
}

do_install() {
    install -Dm755 pir_mqtt ${D}${bindir}/pir_mqtt
    install -Dm644 ${WORKDIR}/pir-mqtt.service ${D}${systemd_system_unitdir}/pir-mqtt.service
    install -Dm644 ${WORKDIR}/config.ini ${D}${sysconfdir}/pir_mqtt/config.ini
}


inherit systemd

# SYSTEMD_SERVICE:${PN} = "pir_mqtt.service"
# SYSTEMD_AUTO_ENABLE:${PN} = "enable"


# Specify which package contains the systemd service.
# ${PN} expands to the package name, e.g., 'pir-mqtt-alert'.
SYSTEMD_PACKAGES = "${PN}"

# List the systemd service file(s) that should be enabled by default.
# This ensures that 'systemctl enable pir-mqtt-alert.service' is run during image creation.
SYSTEMD_SERVICE:${PN} = "pir-mqtt.service"

# Optional: Define RRECOMMENDS if there are other packages that are highly recommended
# but not strictly required for the application to function.
# For example, if you want to ensure the base system includes tools for debugging MQTT.
# RRECOMMENDS:${PN} += "mosquitto-clients"

# Optional: Specify dependencies for the runtime package.
# This ensures that libmosquitto and libgpiod runtime libraries are included in the image
# when this application is added.
RDEPENDS:${PN} += "mosquitto libgpiod"