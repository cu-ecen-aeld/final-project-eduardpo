FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://usercfg.txt"

do_install:append() {
    install -m 0644 ${WORKDIR}/usercfg.txt ${D}${sysconfdir}/raspi-config/
}
