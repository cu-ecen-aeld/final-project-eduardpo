do_install:append() {
    install -d ${D}/boot

    # Only append if not already present
    if ! grep -q "include usercfg.txt" ${D}/boot/config.txt 2>/dev/null; then
        echo "" >> ${D}/boot/config.txt
        echo "[all]" >> ${D}/boot/config.txt
        echo "include usercfg.txt" >> ${D}/boot/config.txt
    fi
}
