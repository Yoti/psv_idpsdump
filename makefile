TITLE_ID = YPID00001
TITLE    = PSV IDPS Dumper v0.9
TARGET   = PSV_IDPS_Dumper_v0.9
OBJS     = main.o graphics.o font.o

LIBS = -lSceLibKernel_stub -lSceVshBridge_stub -lSceDisplay_stub -lSceCtrl_stub

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

%.vpk: eboot.bin
	vita-mksfoex -d PARENTAL_LEVEL=1 -s APP_VER=00.80 -s TITLE_ID=$(TITLE_ID) "$(TITLE)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
					--add change.log=sce_sys/manual/change.log \
					--add thanks.txt=sce_sys/manual/thanks.txt \
					--add LICENSE=sce_sys/manual/license.txt \
					--add livearea/icon0.png=sce_sys/icon0.png \
					--add livearea/pic0.png=sce_sys/pic0.png \
					--add livearea/bg0.png=sce_sys/livearea/contents/bg0.png \
					--add livearea/startup.png=sce_sys/livearea/contents/startup.png \
					--add livearea/github.png=sce_sys/livearea/contents/github.png \
					--add livearea/template.xml=sce_sys/livearea/contents/template.xml \
					$@

eboot.bin: $(TARGET).velf
	vita-make-fself $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS) \
		eboot.bin param.sfo

vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/
	@echo "Sent."

send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."
