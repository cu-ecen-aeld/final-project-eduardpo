FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://usercfg.txt"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://usercfg.txt"

do_install:append() {
    install -d ${D}/boot

    # Install usercfg.txt
    install -m 0644 ${WORKDIR}/usercfg.txt ${D}/boot/usercfg.txt

    # Inject the include directive if not already present
    if ! grep -q "include usercfg.txt" ${D}/boot/config.txt 2>/dev/null; then
        echo "" >> ${D}/boot/config.txt
        echo "[all]" >> ${D}/boot/config.txt
        echo "include usercfg.txt" >> ${D}/boot/config.txt
    fi
}


do_deploy:append() {
    install -d ${DEPLOYDIR}/boot
    echo "" >> ${DEPLOYDIR}/boot/config.txt
    echo "[all]" >> ${DEPLOYDIR}/boot/config.txt
    echo "include usercfg.txt" >> ${DEPLOYDIR}/boot/config.txt
    install -Dm0644 ${WORKDIR}/usercfg.txt ${DEPLOYDIR}/bootfiles/usercfg.txt
}
