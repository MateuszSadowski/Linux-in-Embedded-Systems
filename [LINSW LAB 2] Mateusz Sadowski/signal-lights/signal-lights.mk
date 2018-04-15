#####################################################################
# Signal lights
#####################################################################
SIGNAL-LIGHTS_VERSION = 1.0
SIGNAL-LIGHTS_DEPENDENCIES = wiringpi
SIGNAL-LIGHTS_SITE = /home/samba/sadowskim/Documents/LINSW/lab2\ -\ 05.04.18/lab2\ v2
SIGNAL-LIGHTS_METHOD = local
define SIGNAL-LIGHTS_BUILD_CMDS
 $(MAKE) $(TARGET_CONFIGURE_OPTS) signal-lights -C $(@D)
endef
define WORMS_INSTALL_TARGET_CMDS
 $(INSTALL) -D -m 0755 $(@D)/signal-lights $(TARGET_DIR)/usr/bin
endef
$(eval $(generic-package))
