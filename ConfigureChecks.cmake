include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckPrototypeExists)
include(CheckTypeSize)

# The FindKDE4.cmake module sets _KDE4_PLATFORM_DEFINITIONS with
# definitions like _GNU_SOURCE that are needed on each platform.
set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})

#check for libz using the cmake supplied FindZLIB.cmake
if(ZLIB_FOUND)
   set(HAVE_LIBZ 1)
else(ZLIB_FOUND)
   set(HAVE_LIBZ 0)
endif(ZLIB_FOUND)

if(JPEG_FOUND)
   set(HAVE_LIBJPEG 1)
else(JPEG_FOUND)
   set(HAVE_LIBJPEG 0)
endif(JPEG_FOUND)

if(OPENSSL_FOUND)
   set(HAVE_SSL 1)
else(OPENSSL_FOUND)
   set(HAVE_SSL 0)
endif(OPENSSL_FOUND)

#check for png
if(PNG_FOUND)
   set(HAVE_LIBPNG 1)
else(PNG_FOUND)
   set(HAVE_LIBPNG 0)
endif(PNG_FOUND)

if(BZIP2_FOUND)
   set(HAVE_BZIP2_SUPPORT 1)
   if(BZIP2_NEED_PREFIX)
      set(NEED_BZ2_PREFIX 1)
   endif(BZIP2_NEED_PREFIX)
else(BZIP2_FOUND)
   set(HAVE_BZIP2_SUPPORT 0)
endif(BZIP2_FOUND)

if(LIBART_FOUND)
   set(HAVE_LIBART 1)
else(LIBART_FOUND)
   set(HAVE_LIBART 0)
endif(LIBART_FOUND)

if(CARBON_FOUND)
	set(HAVE_CARBON 1)
else(CARBON_FOUND)
	set(HAVE_CARBON 0)
endif(CARBON_FOUND)

if(KDE4_KDEPIM_NEW_DISTRLISTS)
	set(KDEPIM_NEW_DISTRLISTS 1)
else(KDE4_KDEPIM_NEW_DISTRLISTS)
	set(KDEPIM_NEW_DISTRLISTS 0)
endif(KDE4_KDEPIM_NEW_DISTRLISTS)

if(SASL2_FOUND)
	set(HAVE_LIBSASL2 1)
else(SASL2_FOUND)
	set(HAVE_LIBSASL2 0)
endif(SASL2_FOUND)

#now check for dlfcn.h using the cmake supplied CHECK_include_FILE() macro
# If definitions like -D_GNU_SOURCE are needed for these checks they
# should be added to _KDE4_PLATFORM_DEFINITIONS when it is originally
# defined outside this file.  Here we include these definitions in
# CMAKE_REQUIRED_DEFINITIONS so they will be included in the build of
# checks below.
set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})
if (WIN32)
   set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN32_LIBRARIES} )
   set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN32_INCLUDES} )
endif (WIN32)
check_include_files(baudboy.h HAVE_BAUDBOY_H)
check_include_files(bluetooth/bluetooth.h HAVE_BLUETOOTH_BLUETOOTH_H)
check_include_files(byteswap.h HAVE_BYTESWAP_H)
check_include_files(crt_externs.h HAVE_CRT_EXTERNS_H)
check_include_files(dlfcn.h HAVE_DLFCN_H)
check_include_files(err.h HAVE_ERR_H)
check_include_files(fcntl.h HAVE_FCNTL_H)
check_include_files(gnokii.h HAVE_GNOKII_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(libgen.h HAVE_LIBGEN_H)
check_include_files(lockdev.h HAVE_LOCKDEV_H)
check_include_files(malloc.h HAVE_MALLOC_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(ndir.h HAVE_NDIR_H)
check_include_files(paths.h HAVE_PATHS_H)
check_include_files(sasl/sasl.h HAVE_SASL_SASL_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(sysent.h HAVE_SYSENT_H)
check_include_files(sys/bitypes.h HAVE_SYS_BITYPES_H)
check_include_files(sys/cdefs.h HAVE_SYS_CDEFS_H)
check_include_files(sys/dir.h HAVE_SYS_DIR_H)
check_include_files(sys/file.h HAVE_SYS_FILE_H)
check_include_files(sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_files(sys/limits.h HAVE_SYS_LIMITS_H)
check_include_files(sys/ndir.h HAVE_SYS_NDIR_H)
check_include_files(sys/param.h HAVE_SYS_PARAM_H)
check_include_files(sys/select.h HAVE_SYS_SELECT_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/sysctl.h HAVE_SYS_SYSCTL_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(time.h HAVE_TIME_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(values.h HAVE_VALUES_H)
check_include_files(sys/time.h TM_IN_SYS_TIME)
check_include_files("sys/time.h;time.h" TIME_WITH_SYS_TIME)
check_include_files(assert.h HAVE_ASSERT_H)
check_include_files(dirent.h HAVE_DIRENT_H)

check_function_exists(_NSGetEnviron   HAVE_NSGETENVIRON)
check_function_exists(setenv     HAVE_SETENV)
check_function_exists(socket          HAVE_SOCKET)
check_function_exists(strlcat    HAVE_STRLCAT)
check_function_exists(strlcpy    HAVE_STRLCPY)
check_function_exists(unsetenv   HAVE_UNSETENV)
check_function_exists(usleep     HAVE_USLEEP)

check_symbol_exists(gethostname     "unistd.h"                 HAVE_GETHOSTNAME)
check_symbol_exists(getdomainname     "unistd.h"                 HAVE_GETDOMAINNAME)
check_symbol_exists(res_init        "sys/types.h;netinet/in.h;arpa/nameser.h;resolv.h" HAVE_RES_INIT)
check_symbol_exists(snprintf        "stdio.h"                  HAVE_SNPRINTF)
check_symbol_exists(vsnprintf       "stdio.h"                  HAVE_VSNPRINTF)

check_prototype_exists(setenv stdlib.h HAVE_SETENV_PROTO)

check_prototype_exists(gethostname "stdlib.h;unistd.h" HAVE_GETHOSTNAME_PROTO)
check_prototype_exists(res_init "sys/types.h;netinet/in.h;arpa/nameser.h;resolv.h" HAVE_RES_INIT_PROTO)
check_prototype_exists(strlcat string.h HAVE_STRLCAT_PROTO)
check_prototype_exists(strlcpy string.h HAVE_STRLCPY_PROTO)
check_prototype_exists(unsetenv stdlib.h HAVE_UNSETENV_PROTO)
check_prototype_exists(usleep unistd.h HAVE_USLEEP_PROTO)
check_prototype_exists(getdomainname "stdlib.h;unistd.h;netdb.h" HAVE_GETDOMAINNAME_PROTO)

check_type_size("int" SIZEOF_INT)
check_type_size("char *"  SIZEOF_CHAR_P)
check_type_size("long" SIZEOF_LONG)
check_type_size("short" SIZEOF_SHORT)
check_type_size("size_t" SIZEOF_SIZE_T)
check_type_size("unsigned long" SIZEOF_UNSIGNED_LONG)
check_type_size("uint64_t" SIZEOF_UINT64_T)
check_type_size("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)
