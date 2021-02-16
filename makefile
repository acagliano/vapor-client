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

OUTPUT_MAP   ?= NO

include $(CEDEV)/meta/makefile.mk

