#CFLAGS := $(CFLAGS) -g
CFLAGS = -I. -Ilib -Wall -DVERSION=\"5.10\" -Wno-strict-aliasing
CFLAGS  += -Ilib/bluetooth

# include rule.mk
include ./rule.mk

# define objects collection variable
lib = \
	lib/bluetooth.o \
	lib/hci.o \
	lib/sdp.o

hciattach_objs = \
	hciattach.o \
	hciattach_ath3k.o \
	hciattach_bcm43xx.o \
	hciattach_intel.o \
	hciattach_qualcomm.o \
	hciattach_st.o \
	hciattach_ti.o \
	hciattach_tialt.o \
	hciattach_rtk.o \
	hciattach_xr.o \
	hciattach_sprd.o

# add your target(s) to all
.PHONY: all
all: hciattach_opi

# directory
hciattach_opi: $(hciattach_objs) $(lib)
	$(LINK_MSG)
	$(LINKX)

# change objs to your objects collection variable
$(hciattach_objs): %.o: %.c
	$(COMPILE_MSG)
	$(COMPILEX)

# clean temp files
clean:
	-rm -rf $(hciattach_objs) $(hciattach_objs:.o=.d)
	-rm -rf $(lib) $(lib:.o=.d)
	#-rm -rf output
