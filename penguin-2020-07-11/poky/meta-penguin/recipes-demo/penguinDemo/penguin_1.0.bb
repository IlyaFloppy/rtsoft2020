SUMMARY = "kernel module"
LICENSE = "BEERWARE"
LIC_FILES_CHKSUM = "file://COPYING;md5=19d79d4dcc60d97be787c4c31878bfa4"

inherit module

SRC_URI = "file://Makefile \
           file://penguin.c \
           file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.

RPROVIDES_${PN} += "kernel-module-hello"
