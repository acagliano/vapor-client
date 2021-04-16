# ----------------------------
# Program Options
# ----------------------------

NAME         ?= VAPOR
ICON         ?= icon.png
DESCRIPTION  ?= "VAPOR Networking"
MAIN_ARGS    ?= NO
COMPRESSED   ?= NO
ARCHIVED     ?= NO

L ?= graphx keypadc srldrvce usbdrvce

# ----------------------------
# Compile Options
# ----------------------------

OPT_MODE     ?= -Oz
EXTRA_CFLAGS ?= -Wall -Wextra

# ----------------------------
# Debug Options
# ----------------------------

OUTPUT_MAP   ?= YES

include $(CEDEV)/meta/makefile.mk

LDLIBS = $(CEDEV)/lib/libload/graphx.lib $(CEDEV)/lib/libload/fileioc.lib $(CEDEV)/lib/libload/usbdrvce.lib $(CEDEV)/lib/libload/srldrvce.lib $(CEDEV)/lib/libload/keypadc.lib
