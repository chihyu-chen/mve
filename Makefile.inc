# Default values.
CXX ?= g++
CXXWARNINGS ?= -Wall -Wextra -Wundef -pedantic
CXXFEATURES ?= -funsafe-math-optimizations -fno-math-errno -std=c++11
CXXFLAGS ?= ${CXXWARNINGS} ${CXXINTRINSICS} ${CXXFEATURES} -g -O3

# Delete this if OpenMP is not available (e.g., OS X without gcc)
OPENMP ?= -fopenmp

# Always compile programs and libs with multi-threading.
CXXFLAGS += -pthread

# Add include and library paths for PNG
PNG_INCLUDE_PATH = /usr/local/include
PNG_LIB_PATH = /usr/local/lib

CXXFLAGS += -I${PNG_INCLUDE_PATH}
LDFLAGS += -L${PNG_LIB_PATH} -lpng

UNAME = $(shell uname)
ifeq (${UNAME},Darwin)
    OPENMP =
endif

# Determine the target
ifeq ($(BUILD_TARGET),ios_simulator)
	SDK = iphonesimulator
	ARCH = x86_64
	SDK_PATH = $(shell xcrun --sdk $(SDK) --show-sdk-path)
	CXX = xcrun -sdk $(SDK) clang++
	CXXFLAGS += -arch $(ARCH) -isysroot $(SDK_PATH) -mios-simulator-version-min=9.0
	LDFLAGS += -arch $(ARCH) -isysroot $(SDK_PATH) -mios-simulator-version-min=9.0
else ifeq ($(BUILD_TARGET),ios_device)
	SDK = iphoneos
	ARCH = arm64
	SDK_PATH = $(shell xcrun --sdk $(SDK) --show-sdk-path)
	CXX = xcrun -sdk $(SDK) clang++
	CXXFLAGS += -arch $(ARCH) -isysroot $(SDK_PATH) -miphoneos-version-min=9.0
	LDFLAGS += -arch $(ARCH) -isysroot $(SDK_PATH) -miphoneos-version-min=9.0
    CXXFLAGS += -D__ARM_NEON__ -D__LITTLE_ENDIAN__
else
    CXXINTRINSICS ?= -msse2 -msse3 -msse4 -mpopcnt
    CXXFLAGS += ${CXXINTRINSICS}
endif

COMPILE.cc = ${CXX} ${CXXFLAGS} ${CPPFLAGS} -c
LINK.o = ${CXX} ${LDFLAGS}
.DEFAULT_GOAL := ${TARGET}

# How to link example from example.o and other files.
%: %.o
	${LINK.o} -o $@ $^ ${LDLIBS}

# How to compile example.o from example.cc.
%.o: %.cc
	${COMPILE.cc} -o $@ $<

# Library paths.
vpath libmve_dmrecon.a ${MVE_ROOT}/libs/dmrecon/
vpath libmve_math.a ${MVE_ROOT}/libs/math/
vpath libmve.a ${MVE_ROOT}/libs/mve/
vpath libmve_ogl.a ${MVE_ROOT}/libs/ogl/
vpath libmve_util.a ${MVE_ROOT}/libs/util/
vpath libmve_sfm.a ${MVE_ROOT}/libs/sfm/
vpath libmve_fssr.a ${MVE_ROOT}/libs/fssr/

# Autogenerate dependencies.
Makefile.dep: [^_]*.cc
	${COMPILE.cc} -MM $^ >$@

-include Makefile.dep
