

ifeq (,$(filter _%,$(notdir $(CURDIR))))
  include target.mk
else
#----- End Boilerplate

# This section is for the architecture-specific target directory
VPATH =  $(SRCDIR)

# Additional headers and libraries directories
INFRA_PATH = ../../learning_tools/infra2
LEARNING_PATH = ../../learning_tools
ATLAS_PATH = /usr/local/atlas3.10/lib

CC = g++
CXXFLAGS = -Wall -I$(INFRA_PATH) -I$(LEARNING_PATH)
LDLIBS = -L$(INFRA_PATH) -L$(LEARNING_PATH)/cmdline

# Check if the configuration is Release or Debug
ifeq ($(CONFIGURATION),Debug)
	CXXFLAGS += -g
	LDLIBS += -g -linfra_debug -lcmdline
else
	CXXFLAGS += -O3 -DNDEBUG
	LDLIBS += -O3 -linfra -L$(ATLAS_PATH) -lcblas -latlas -lcmdline
endif

ifeq ($(ATLAS),yes)
  	LDLIBS += -L$(ATLAS_PATH) -lcblas -latlas
endif

# If the machine is PowerPC swap endians (shell variable)
ifeq ($(shell uname -p),powerpc) 
		CXXFLAGS += -D_ENDIAN_SWAP_
endif

# Targets
all: KeywordSpotterTrain KeywordSpotterDecode
KeywordSpotterTrain: KeywordSpotterTrain.o Classifier.o Dataset.o
KeywordSpotterDecode: KeywordSpotterDecode.o Classifier.o Dataset.o

#----- Begin Boilerplate
endif
