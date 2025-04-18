# MAKEFILE for linux GCC
#
# Tom St Denis
# Modified by Clay Culver
#
#  (GNU make only)

ifeq ($V,0)
silent_echo= > /dev/null
else
silent_echo=
endif
ifeq ($V,1)
silent=
silent_stdout=
silent_stderr=
else
silent=@
silent_stdout= > /dev/null
silent_stderr= 2> /dev/null
endif

PLATFORM := $(shell uname | sed -e 's/_.*//')

# ranlib tools
ifndef RANLIB
RANLIB:=$(CROSS_COMPILE)ranlib
endif
INSTALL_CMD = install
UNINSTALL_CMD = rm

#Output filenames for various targets.
ifndef LIBNAME
   LIBNAME=libtomcrypt.a
endif


include makefile_include.mk

ifeq ($(COVERAGE),1)
all_test: LIB_PRE = -Wl,--whole-archive
all_test: LIB_POST = -Wl,--no-whole-archive
LTC_CFLAGS += -fprofile-arcs -ftest-coverage
LTC_EXTRALIBS += -lgcov
endif

LTC_EXTRALIBS += $(EXTRALIBS)

#AES comes in two flavours... enc+dec and enc
src/ciphers/aes/aes_enc.o: src/ciphers/aes/aes.c src/ciphers/aes/aes_tab.c
ifneq ($V,1)
	@echo "   * ${CC} $@" ${silent_echo}
endif
	${silent} ${CC} ${LTC_CFLAGS} -DENCRYPT_ONLY -c $< -o $@
src/ciphers/aes/aes_enc_desc.o: src/ciphers/aes/aes_desc.c
ifneq ($V,1)
	@echo "   * ${CC} $@" ${silent_echo}
endif
	${silent} ${CC} ${LTC_CFLAGS} -DENCRYPT_ONLY -c $< -o $@

.c.o:
ifneq ($V,1)
	@echo "   * ${CC} $@" ${silent_echo}
endif
	${silent} ${CC} ${LTC_CFLAGS} -c $< -o $@

$(LIBNAME): $(OBJECTS)
ifneq ($V,1)
	@echo "   * ${AR} $@" ${silent_echo}
endif
	${silent} $(AR) $(ARFLAGS) $@ $(OBJECTS)
ifneq ($V,1)
	@echo "   * ${RANLIB} $@" ${silent_echo}
endif
	${silent} $(RANLIB) $@

test: $(call print-help,test,Builds the library and the 'test' application to run all self-tests) $(LIBNAME) $(TOBJECTS)
ifneq ($V,1)
	@echo "   * ${CC} $@" ${silent_echo}
endif
	${silent} $(CC) $(LTC_LDFLAGS) $(TOBJECTS) $(LIB_PRE) $(LIBNAME) $(LIB_POST) $(LTC_EXTRALIBS) -o $(TEST)

# build the demos from a template
define DEMO_template
$(1): $(call print-help,$(1),Builds the library and the '$(1)' demo) demos/$(1).o $$(LIBNAME)
ifneq ($V,1)
	@echo "   * $${CC} $$@" ${silent_echo}
endif
	$${silent} $$(CC) $$(LTC_LDFLAGS) $$< $$(LIB_PRE) $$(LIBNAME) $$(LIB_POST) $$(LTC_EXTRALIBS) -o $$@
endef

$(foreach demo, $(strip $(DEMOS)), $(eval $(call DEMO_template,$(demo))))


#This rule installs the library and the header files. This must be run
#as root in order to have a high enough permission to write to the correct
#directories and to set the owner and group to root.
install: $(call print-help,install,Installs the library and headers) .common_install

install_bins: $(call print-help,install_bins,Installs the useful demos ($(USEFUL_DEMOS))) .common_install_bins

uninstall: $(call print-help,uninstall,Uninstalls the library and headers) .common_uninstall

profile:
	LTC_CFLAGS="$(LTC_CFLAGS) -fprofile-generate" $(MAKE) timing EXTRALIBS="$(LTC_EXTRALIBS) -lgcov"
	./timing
	rm -f timing `find . -type f | grep [.][ao] | xargs`
	LTC_CFLAGS="$(LTC_CFLAGS) -fprofile-use" $(MAKE) timing EXTRALIBS="$(LTC_EXTRALIBS) -lgcov"

# target that pre-processes all coverage data
lcov-single-create:
	lcov --capture --no-external --directory src -q --output-file coverage_std.info

# target that removes all coverage output
cleancov-clean:
	rm -f `find . -type f -name "*.info" | xargs`
	rm -rf coverage/

# merges all coverage_*.info files into coverage.info
coverage.info:
	lcov `find -name 'coverage_*.info' -exec echo -n " -a {}" \;` -o coverage.info

# generates html output from all coverage_*.info files
lcov-html: coverage.info
	genhtml coverage.info --output-directory coverage -q

# combines all necessary steps to create the coverage from a single testrun with e.g.
# CFLAGS="-DUSE_LTM -DLTM_DESC -I../libtommath" EXTRALIBS="../libtommath/libtommath.a" make coverage -j9
lcov-single:
	$(MAKE) cleancov-clean
	$(MAKE) lcov-single-create
	$(MAKE) coverage.info


#make the code coverage of the library
coverage: LTC_CFLAGS += -fprofile-arcs -ftest-coverage
coverage: LTC_EXTRALIBS += -lgcov
coverage: LIB_PRE = -Wl,--whole-archive
coverage: LIB_POST = -Wl,--no-whole-archive

coverage: $(call print-help,coverage,Create code-coverage of the library - but better use coverage.sh) test
	./test

# cleans everything - coverage output and standard 'clean'
cleancov: cleancov-clean clean
ifndef AMALGAM
AMALGAM_FILTER_OUT = src/ciphers/aes/aes_enc.c src/ciphers/aes/aes_enc_desc.c
TAB_SOURCES = src/ciphers/aes/aes_tab.c src/ciphers/safer/safer_tab.c src/hashes/whirl/whirltab.c src/stream/sober128/sober128tab.c
SOURCES = $(filter-out $(AMALGAM_FILTER_OUT),$(OBJECTS:.o=.c))
pre_gen/tomcrypt_amalgam.c: $(TAB_SOURCES) $(SOURCES)
	mkdir -p pre_gen
	printf "/*\n * This file has been auto-generated, do not edit!\n */\n\n" > $@
	printf "#define LTC_AES_TAB_C\n" >> $@
	printf "#define LTC_SAFER_TAB_C\n" >> $@
	printf "#define LTC_SOBER128TAB_C\n" >> $@
	printf "#define LTC_WHIRLTAB_C\n\n" >> $@
	printf "#include \"tomcrypt_private.h\"\n\n" >> $@
	cat $^ >> $@

pre_gen: pre_gen/tomcrypt_amalgam.c

.PHONY: pre_gen/tomcrypt_amalgam.c
endif

