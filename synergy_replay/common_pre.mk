
ifeq ($(DEMO_3),y)
EXTRAFLAGS+= -DDEMO_3
endif

ifeq ($(DEMO_SIMPLE),y)
EXTRAFLAGS+= -DDEMO_SIMPLE
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
#LOG         :=n
#PROFILE     :=n
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
LOG         :=y
PROFILE     :=y
else
ifeq ($(DEMO_SIMPLE),y)
include music/simple.mk
else
ifeq ($(DEMO_3),y)
include music/demo_3_fm-a.mk
else
include music/demo_2-s-sr.mk
endif # DEMO_3
endif # DEMO_SIMPLE
endif # DEMO_ALL
endif # DEMO_SMP
