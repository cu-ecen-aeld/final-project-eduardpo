DESCRIPTION = "PIR-MQTT Motion alert"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI += "file://pir_mqtt.cpp"

DEPENDS = "mosquitto libgpiod"


do_compile() {
    ${CXX} ${WORKDIR}/pir_mqtt.cpp -o pir_mqtt -lmosquitto -lgpiod -Wl,--hash-style=gnu
}

do_install() {
    install -Dm755 pir_mqtt ${D}${bindir}/pir_mqtt
}

RDEPENDS:${PN} += "mosquitto libgpiod"