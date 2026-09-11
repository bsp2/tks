
ifeq ($(DEMO_3),y)
EXTRAFLAGS+= -DDEMO_3
endif

ifeq ($(DEMO_SMP),y)
MIN_FEATURES:=y
WAVETABLES  :=n
LFO         :=n
MODSEQ      :=n
MUTEX       :=n
RANGE       :=n
GLIDE       :=n
TIMEDLOOP   :=n
FILTERS     :=n
PLUGINS     :=n
PROCEDURAL  :=n
AUDIO       :=y
else
ifeq ($(DEMO_ALL),y)
MIN_FEATURES:=n
WAVETABLES  :=y
LFO         :=y
MODSEQ      :=y
MUTEX       :=y
RANGE       :=y
GLIDE       :=y
TIMEDLOOP   :=y
FILTERS     :=y
PLUGINS     :=y
PROCEDURAL  :=y
AUDIO       :=y
SR_TRACK_SENDS:=y
SR_TRACK_FX   :=y
SR_VOICE_FX   :=y
else
ifeq ($(DEMO_3),y)
include music/autogen_demo_3_fm-a.mk
else
include music/autogen_demo_2-r-sr.mk
endif # DEMO_3
endif # DEMO_ALL
endif # DEMO_SMP
