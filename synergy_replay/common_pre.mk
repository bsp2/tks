
ifeq ($(DEMO_3),y)
EXTRAFLAGS+= -DDEMO_3
endif

ifeq ($(DEMO_SMP),y)
MIN_FEATURES:=y
WAVETABLES  :=n
LFO         :=n
MODSEQ      :=n
FILTERS     :=n
PLUGINS     :=n
PROCEDURAL  :=n
AUDIO       :=y
else
ifeq ($(DEMO_3),y)
include music/autogen_demo_3_fm-a.mk
else
include music/autogen_demo_2-r-sr.mk
endif # DEMO_3
endif # DEMO_SMP
