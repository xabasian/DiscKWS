
CURDIR := $(shell basename `pwd` )
SUBDIRS := learning_tools phoneme_frame_based forced_alignment keyword_spotting
USE_ATLAS = yes
export USE_ATLAS

ifndef _ARCH
  _ARCH := $(shell uname -s)
  export _ARCH
endif

ifndef CONFIGURATION
  CONFIGURATION = Release
  export _CONFIGURATION
endif
OBJDIR := $(CURDIR)/_$(_ARCH)_$(CONFIGURATION)
export OBJDIR


all:
	@echo ">>> Compiling all <<<"
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} ATLAS=$(USE_ATLAS) $@) || exit 10 ; done
	@echo ">> !!! Ok !!! <<<"
	@mkdir -p ../bin
	@cp learning_tools/htk_utils/_$(_ARCH)_$(CONFIGURATION)/htk2db ../bin
	@cp learning_tools/htk_utils/_$(_ARCH)_$(CONFIGURATION)/htk2txt ../bin
	@cp learning_tools/htk_utils/_$(_ARCH)_$(CONFIGURATION)/htk_ceps_dist ../bin
	@cp learning_tools/htk_utils/_$(_ARCH)_$(CONFIGURATION)/htk_shuffle ../bin
	@cp learning_tools/htk_utils/_$(_ARCH)_$(CONFIGURATION)/htk_split ../bin
	@cp learning_tools/htk_utils/_$(_ARCH)_$(CONFIGURATION)/htk_stats ../bin
	@cp learning_tools/htk_utils/_$(_ARCH)_$(CONFIGURATION)/txt2htk ../bin
	@cp phoneme_frame_based/_$(_ARCH)_$(CONFIGURATION)/PhonemeFrameBasedTrain ../bin
	@cp phoneme_frame_based/_$(_ARCH)_$(CONFIGURATION)/PhonemeFrameBasedDecode ../bin
	@cp forced_alignment/_$(_ARCH)_$(CONFIGURATION)/ForcedAlignmentTrain ../bin
	@cp forced_alignment/_$(_ARCH)_$(CONFIGURATION)/ForcedAlignmentDecode ../bin
	@cp keyword_spotting/_$(_ARCH)_$(CONFIGURATION)/KeywordSpotterTrain ../bin
	@cp keyword_spotting/_$(_ARCH)_$(CONFIGURATION)/KeywordSpotterDecode ../bin
	
clean:
	@echo ">>> Cleaning all <<<"
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} clean $@ ) || exit 10 ; done
	@echo ">> !!! Ok !!! <<<"
	rm -fr ../bin/*

