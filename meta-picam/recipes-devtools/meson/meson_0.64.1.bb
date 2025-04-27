SUMMARY = "Meson build system"
HOMEPAGE = "https://mesonbuild.com/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://COPYING;md5=3b83ef96387f14655fc854ddc3c6bd57"

SRC_URI = "https://github.com/mesonbuild/meson/releases/download/${PV}/meson-${PV}.tar.gz"
SRC_URI[sha256sum] = "3a8e030c2334f782085f81627062cc6d4a6771edf31e055ffe374f9e6b089ab9"

inherit setuptools3

# Prevent race conditions from parallel pyc compilation
export PYTHONDONTWRITEBYTECODE = "1"

S = "${WORKDIR}/meson-${PV}"

DEPENDS += "ninja-native python3 python3-setuptools-native"

RDEPENDS:${PN} += "ninja python3 python3-setuptools"

BBCLASSEXTEND = "native nativesdk"
