

ifeq (,$(filter _%,$(notdir $(CURDIR))))
  include target.mk
else
#----- End Boilerplate

# This section is for the architecture-specific target directory
VPATH =  $(SRCDIR)

# Additional headers and libraries directories
INFRA_PATH = ../../../learning_tools/infra2
LEARNING_PATH = ../../../learning_tools
ATLAS_PATH = /root/atlas_install/ATLAS3.8.0/Linux_C2D64SSE3/lib

CC = g++
CXXFLAGS = -Wall -I$(INFRA_PATH) -I$(LEARNING_PATH)
LDLIBS = -L$(INFRA_PATH) -L$(LEARNING_PATH)/cmdline

# Check if the configuration is Release or Debug
ifeq ($(CONFIGURATION),Debug)
	CXXFLAGS += -g
	LDLIBS += -g -linfra_debug -lcmdline
else
	CXXFLAGS += -O3 -DNDEBUG
	LDLIBS += -O3 -linfra -lcmdline
endif
ifeq ($(ATLAS),yes)
  	LDLIBS += -L$(ATLAS_PATH) -lcblas -latlas
endif

# If the machine is PowerPC swap endians (shell variable)
ifeq ($(shell uname -p),powerpc) 
		CXXFLAGS += -D_ENDIAN_SWAP_
endif

# Targets
all: htk_shuffle htk_stats htk2db htk2txt txt2htk htk_ceps_dist htk_split
htk_shuffle: htk_shuffle.o HtkFile.o
htk_stats: htk_stats.o HtkFile.o
htk_split: htk_split.o HtkFile.o
htk2db: htk2db.o HtkFile.o
htk2txt: htk2txt.o HtkFile.o
txt2htk: txt2htk.o HtkFile.o
htk_ceps_dist: htk_ceps_dist.o HtkFile.o

#----- Begin Boilerplate
endif
