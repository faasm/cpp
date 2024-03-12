# Project directories
LLVM_PROJ_DIR=${FAASM_LLVM_DIR}

# Install dirs
FAASM_LOCAL_DIR=${FAASM_LOCAL_DIR_ENV}
FAASM_SYSROOT=$(FAASM_LOCAL_DIR)/llvm-sysroot
FAASM_TOOLCHAIN_DIR=$(FAASM_LOCAL_DIR)/toolchain
FAASM_TOOLCHAIN_FILE=$(FAASM_TOOLCHAIN_DIR)/tools/WasiToolchain.cmake

CLANG_VERSION=${FAASM_LLVM_VERSION}
CLANG_VERSION_MAJOR := $(shell echo $(CLANG_VERSION) | cut -f1 -d.)

BUILD_DIR=${FAASM_LLVM_BUILD_DIR}
LLVM_CONFIG=$(BUILD_DIR)/llvm/bin/llvm-config
AR=$(BUILD_DIR)/llvm/bin/llvm-ar

WASI_LIBC_DIR=${FAASM_WASI_LIBC_DIR}

# -------------------------------------------
# This is adapted from the wasi-sdk Makefile found here:
# https://github.com/WebAssembly/wasi-sdk/blob/main/Makefile
# -------------------------------------------

default: build

.PHONY: clean-libc
clean-libc:
	rm -rf $(BUILD_DIR)/wasi-libc.BUILT $(WASI_LIBC_DIR)/build

.PHONY: very-clean-libc
very-clean-libc: clean-libc
	# WARNING: this is going to remove _everything_ in FAASM_SYSROOT
	cd $(WASI_LIBC_DIR) && SYSROOT=$(FAASM_SYSROOT) make clean

.PHONY: clean-libs
clean-libs: clean-libc
	# compiler-rt clean
	rm -rf $(BUILD_DIR)/compiler-rt $(BUILD_DIR)/compiler-rt.BUILT
	# libcxx clean
	rm -rf $(BUILD_DIR)/libcxx $(BUILD_DIR)/libcxx.BUILT
	rm -rf $(FAASM_SYSROOT)/include/wasm32-wasi/c++
	rm -rf $(FAASM_SYSROOT)/include/wasm32-wasi-threads/c++

.PHONY: clean-all
clean-all:
	rm -rf $(BUILD_DIR) $(FAASM_TOOLCHAIN_DIR)/bin $(FAASM_TOOLCHAIN_DIR)/libs $(FAASM_SYSROOT)

$(BUILD_DIR)/llvm.BUILT:
	mkdir -p $(BUILD_DIR)/llvm
	cd $(BUILD_DIR)/llvm; cmake -G Ninja \
		-DCMAKE_BUILD_TYPE=MinSizeRel \
		-DLLVM_ENABLE_TERMINFO=OFF \
		-DLLVM_ENABLE_ZLIB=OFF \
		-DLLVM_ENABLE_ZSTD=OFF \
		-DLLVM_STATIC_LINK_CXX_STDLIB=ON \
		-DLLVM_HAVE_LIBXAR=OFF \
		-DCMAKE_INSTALL_PREFIX=${FAASM_TOOLCHAIN_DIR} \
		-DLLVM_INCLUDE_TESTS=OFF \
		-DLLVM_INCLUDE_UTILS=OFF \
		-DLLVM_INCLUDE_BENCHMARKS=OFF \
		-DLLVM_INCLUDE_EXAMPLES=OFF \
		-DLLVM_TARGETS_TO_BUILD=WebAssembly \
		-DLLVM_DEFAULT_TARGET_TRIPLE=wasm32-wasi \
		-DLLVM_ENABLE_PROJECTS="lld;clang;openmp" \
		-DDEFAULT_SYSROOT=$(FAASM_SYSROOT) \
		-DLLVM_INSTALL_BINUTILS_SYMLINKS=TRUE \
		-DLLVM_ENABLE_LIBXML2=OFF \
		$(LLVM_PROJ_DIR)/llvm
	ninja -v -C $(BUILD_DIR)/llvm \
		install-clang \
		install-clang-resource-headers \
		install-lld \
		install-llvm-mc \
		install-llvm-ranlib \
		install-llvm-strip \
		install-llvm-dwarfdump \
		install-llvm-symbolizer \
		install-clang-resource-headers \
		install-ar \
		install-ranlib \
		install-strip \
		install-nm \
		install-size \
		install-strings \
		install-objdump \
		install-objcopy \
		install-c++filt \
		llvm-config
	touch $(BUILD_DIR)/llvm.BUILT

# WASI libc
$(BUILD_DIR)/wasi-libc.BUILT: $(BUILD_DIR)/compiler-rt.BUILT
	mkdir -p $(WASI_LIBC_DIR)/build/wasm32-wasi
	$(MAKE) -C ${WASI_LIBC_DIR} \
		CC=$(FAASM_TOOLCHAIN_DIR)/bin/clang \
		AR=$(FAASM_TOOLCHAIN_DIR)/bin/llvm-ar \
		NM=$(FAASM_TOOLCHAIN_DIR)/bin/llvm-nm \
		SYSROOT=$(FAASM_SYSROOT) \
		default libc_so
	$(MAKE) -C ${WASI_LIBC_DIR} \
		CC=$(FAASM_TOOLCHAIN_DIR)/bin/clang \
		AR=$(FAASM_TOOLCHAIN_DIR)/bin/llvm-ar \
		NM=$(FAASM_TOOLCHAIN_DIR)/bin/llvm-nm \
		SYSROOT=$(FAASM_SYSROOT) \
		THREAD_MODEL=posix
	touch $(BUILD_DIR)/wasi-libc.BUILT

$(BUILD_DIR)/compiler-rt.BUILT: $(BUILD_DIR)/llvm.BUILT
	mkdir -p $(BUILD_DIR)/compiler-rt
	cd $(BUILD_DIR)/compiler-rt; cmake -G Ninja \
		-DCMAKE_SYSROOT=${FAASM_SYSROOT} \
		-DCMAKE_C_COMPILER_WORKS=ON \
		-DCMAKE_C_COMPILER_TARGET=wasm32-wasi \
		-DCMAKE_CXX_COMPILER_WORKS=ON \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DCMAKE_TOOLCHAIN_FILE=${FAASM_TOOLCHAIN_FILE} \
		-DCOMPILER_RT_BAREMETAL_BUILD=ON \
		-DCOMPILER_RT_BUILD_XRAY=OFF \
		-DCOMPILER_RT_INCLUDE_TESTS=OFF \
		-DCOMPILER_RT_HAS_FPIC_FLAG=OFF \
		-DCOMPILER_RT_ENABLE_IOS=OFF \
		-DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
		-DLLVM_CONFIG_PATH=$(LLVM_CONFIG) \
		-DCMAKE_INSTALL_PREFIX=$(FAASM_TOOLCHAIN_DIR)/lib/clang/$(CLANG_VERSION)/ \
		-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
		-DCOMPILER_RT_OS_DIR=wasi \
		$(LLVM_PROJ_DIR)/compiler-rt/lib/builtins
	ninja -v -C $(BUILD_DIR)/compiler-rt install
	# Install clang-provided headers
	cp -R $(BUILD_DIR)/llvm/lib/clang $(FAASM_TOOLCHAIN_DIR)/lib/
	# Install libclang_rt.builtins-wasm32.a
	mkdir -p $(FAASM_TOOLCHAIN_DIR)/lib/clang/$(CLANG_VERSION_MAJOR)/lib/wasi
	cp \
		$(BUILD_DIR)/compiler-rt/lib/wasi/libclang_rt.builtins-wasm32.a \
		$(FAASM_TOOLCHAIN_DIR)/lib/clang/$(CLANG_VERSION_MAJOR)/lib/wasi/libclang_rt.builtins-wasm32.a
	touch $(BUILD_DIR)/compiler-rt.BUILT

# Flags for libcxx and libcxxabi.
# $(1): pthreads ON or OFF
# $(2): shared libraries ON or OFF
LIBCXX_CMAKE_FLAGS = \
	-DCMAKE_C_COMPILER_WORKS=ON \
	-DCMAKE_CXX_COMPILER_WORKS=ON \
	-DCMAKE_AR=$(FAASM_TOOLCHAIN_DIR)/bin/ar \
	-DCMAKE_TOOLCHAIN_FILE=$(FAASM_TOOLCHAIN_FILE) \
	-DCMAKE_STAGING_PREFIX=$(FAASM_SYSROOT) \
	-DCMAKE_POSITION_INDEPENDENT_CODE=$(2) \
	-DLLVM_CONFIG_PATH=$(LLVM_CONFIG) \
	-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
	-DCXX_SUPPORTS_CXX11=ON \
	-DLIBCXX_ENABLE_THREADS:BOOL=$(1) \
	-DLIBCXX_HAS_PTHREAD_API:BOOL=$(1) \
	-DLIBCXX_HAS_EXTERNAL_THREAD_API:BOOL=OFF \
	-DLIBCXX_BUILD_EXTERNAL_THREAD_LIBRARY:BOOL=OFF \
	-DLIBCXX_HAS_WIN32_THREAD_API:BOOL=OFF \
	-DLLVM_COMPILER_CHECKED=ON \
	-DCMAKE_BUILD_TYPE=RelWithDebugInfo \
	-DLIBCXX_ENABLE_SHARED:BOOL=$(2) \
	-DLIBCXX_ENABLE_EXPERIMENTAL_LIBRARY:BOOL=OFF \
	-DLIBCXX_ENABLE_EXCEPTIONS:BOOL=OFF \
	-DLIBCXX_ENABLE_FILESYSTEM:BOOL=ON \
	-DLIBCXX_ENABLE_ABI_LINKER_SCRIPT:BOOL=OFF \
	-DLIBCXX_USE_COMPILER_RT=ON \
	-DLIBCXX_CXX_ABI=libcxxabi \
	-DLIBCXX_CXX_ABI_INCLUDE_PATHS=$(LLVM_PROJ_DIR)/libcxxabi/include \
	-DLIBCXX_HAS_MUSL_LIBC:BOOL=ON \
	-DLIBCXX_ABI_VERSION=2 \
	-DLIBCXXABI_ENABLE_EXCEPTIONS:BOOL=OFF \
	-DLIBCXXABI_ENABLE_SHARED:BOOL=$(2) \
	-DLIBCXXABI_SILENT_TERMINATE:BOOL=ON \
	-DLIBCXXABI_ENABLE_THREADS:BOOL=$(1) \
	-DLIBCXXABI_HAS_PTHREAD_API:BOOL=$(1) \
	-DLIBCXXABI_HAS_EXTERNAL_THREAD_API:BOOL=OFF \
	-DLIBCXXABI_BUILD_EXTERNAL_THREAD_LIBRARY:BOOL=OFF \
	-DLIBCXXABI_HAS_WIN32_THREAD_API:BOOL=OFF \
	-DLIBCXXABI_USE_LLVM_UNWINDER=OFF \
	-DLIBCXX_ENABLE_PIC:BOOL=$(2) \
	-DUNIX:BOOL=ON \
	--debug-trycompile

$(BUILD_DIR)/libcxx.BUILT: $(BUILD_DIR)/llvm.BUILT ${BUILD_DIR}/wasi-libc.BUILT
	# Build different libcxx targets
	mkdir -p $(BUILD_DIR)/libcxx
	cd $(BUILD_DIR)/libcxx && cmake -G Ninja $(call LIBCXX_CMAKE_FLAGS,OFF,ON) \
		-DCMAKE_SYSROOT=${FAASM_SYSROOT} \
		-DCMAKE_C_FLAGS="--target=wasm32-wasi" \
		-DCMAKE_CXX_FLAGS="--target=wasm32-wasi" \
		-DLIBCXX_LIBDIR_SUFFIX=/wasm32-wasi \
		-DLIBCXXABI_LIBDIR_SUFFIX=/wasm32-wasi \
		-DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi" \
		$(LLVM_PROJ_DIR)/runtimes
	mkdir -p $(BUILD_DIR)/libcxx-threads
	cd $(BUILD_DIR)/libcxx-threads && cmake -G Ninja $(call LIBCXX_CMAKE_FLAGS,ON,OFF) \
		-DCMAKE_SYSROOT=${FAASM_SYSROOT} \
		-DCMAKE_C_FLAGS="-pthread --target=wasm32-wasi-threads" \
		-DCMAKE_CXX_FLAGS="-pthread --target=wasm32-wasi-threads" \
		-DLIBCXX_LIBDIR_SUFFIX=/wasm32-wasi-threads \
		-DLIBCXXABI_LIBDIR_SUFFIX=/wasm32-wasi-threads \
		-DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi" \
		$(LLVM_PROJ_DIR)/runtimes
	# Do the install
	ninja -v -C $(BUILD_DIR)/libcxx install
	mv $(FAASM_WASM_SYSROOT)/include/c++ $(FAASM_WASM_SYSROOT)/include/wasm32-wasi/
	ninja -v -C $(BUILD_DIR)/libcxx-threads install
	mv $(FAASM_WASM_SYSROOT)/include/c++ $(FAASM_WASM_SYSROOT)/include/wasm32-wasi-threads/
	# As of this writing, `clang++` will ignore the above include dirs unless this one also exists:
	mkdir -p $(FAASM_WASM_SYSROOT)/include/c++/v1
	touch $(BUILD_DIR)/libcxx.BUILT


.PHONY: extras
extras: $(BUILD_DIR)/libcxx.BUILT
	cp $(FAASM_CPP_PROJ_ROOT)/sysroot_extras/* $(FAASM_SYSROOT)/lib/wasm32-wasi/
	cp $(FAASM_CPP_PROJ_ROOT)/sysroot_extras/* $(FAASM_SYSROOT)/lib/wasm32-wasi-threads/

llvm: $(BUILD_DIR)/llvm.BUILT

libc: $(BUILD_DIR)/wasi-libc.BUILT

libcxx: $(BUILD_DIR)/libcxx.BUILT

compiler-rt: $(BUILD_DIR)/compiler-rt.BUILT

build: $(BUILD_DIR)/llvm.BUILT $(BUILD_DIR)/wasi-libc.BUILT $(BUILD_DIR)/compiler-rt.BUILT $(BUILD_DIR)/libcxx.BUILT extras
