FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://usercfg.txt"

do_install:append() {
    install -d ${D}/boot
    install -m 0644 ${WORKDIR}/usercfg.txt ${D}/boot/usercfg.txt
}

do_deploy:append() {
    install -Dm0644 ${WORKDIR}/usercfg.txt ${DEPLOYDIR}/bootfiles/usercfg.txt
}