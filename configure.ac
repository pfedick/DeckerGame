dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.69)
AC_INIT([deckergame], [1.0.0], [patrick@pfp.de], [deckergame-1.0.0.tar.bz2])
AC_CONFIG_AUX_DIR(autoconf)

AC_CONFIG_SRCDIR([pplib/include/ppl7.h])
AC_CONFIG_SRCDIR([pplib/include/config_ppl7.h.in])

AC_CONFIG_FILES([Makefile pplib/Makefile])
AC_CONFIG_HEADERS([pplib/include/config_ppl7.h])
AC_CONFIG_HEADERS([pplib/include/ppl7-config.h])


###############################################################################
# Includes
###############################################################################
sinclude(pplib/autoconf/ax_cxx_compile_stdcxx.m4)
sinclude(pplib/autoconf/ax_path_lib_pcre.m4)
sinclude(pplib/autoconf/png.m4)
sinclude(pplib/autoconf/jpeg.m4)
sinclude(pplib/autoconf/libjpeg-turbo.m4)
sinclude(pplib/autoconf/ax_pthread.m4)
sinclude(pplib/autoconf/sdl2.m4)
sinclude(pplib/autoconf/nasm.m4)
sinclude(pplib/autoconf/lib-link.m4)
sinclude(pplib/autoconf/iconv.m4)
sinclude(pplib/autoconf/bzip2.m4)
sinclude(pplib/autoconf/ax_check_zlib.m4)
sinclude(pplib/autoconf/checkfuncs.m4)
sinclude(pplib/autoconf/ax_gcc_x86_cpuid.m4)
sinclude(pplib/autoconf/ax_check_compile_flag.m4)
sinclude(pplib/autoconf/ax_check_compiler_flags.m4)
sinclude(pplib/autoconf/ax_gcc_archflag.m4)
sinclude(autoconf/mpg123.m4)


report_have_pthreads="no"
report_have_jpeg="no"
report_have_mpg123="no"
report_have_iconv="no"
report_have_bzip2="no"
report_have_zlib="no"
report_have_pcre="no"
report_have_sdl2="no"
report_have_png="no"
report_have_jpeg="no"
report_have_nasm="no"


AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

OLD_CFLAGS=$CFLAGS
# Checks for programs.
AC_PROG_CC
CFLAGS=$OLD_CFLAGS
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX_11
dnl AX_CXX_COMPILE_STDCXX(11)


dnl AC_LANG(C++)

dnl AX_GCC_ARCHFLAG
#AX_CC_MAXOPT

AX_CHECK_COMPILE_FLAG([-no-pie],
    LDFLAGS="-no-pie $LDFLAGS"
    NOPIE="-no-pie",
    NOPIE=""
)
AC_SUBST(NOPIE)



case "$host_os" in
	freebsd* )
		CFLAGS="-I/usr/local/include $CFLAGS"
		LDFLAGS="-L/usr/local/lib $LDFLAGS"
  		;;
esac

case "$host_os" in
	*mingw32*|msys )
		report_have_pthreads="yes, Windows Threads"
  		;;
  	*mingw64* )
		report_have_pthreads="yes, Windows Threads"
  		;;
	*)
		AX_PTHREAD([report_have_pthreads="yes, pthreads"]
			AC_DEFINE(HAVE_PTHREADS,1, [Define to 1 if we have pthreads])
		)
		;;
esac

AC_PROG_SED

AC_C_BIGENDIAN(
	report_byteorder="BIG ENDIAN"
	AC_DEFINE(__BIG_ENDIAN__),
	report_byteorder="LITTLE ENDIAN"
	AC_DEFINE(__LITTLE_ENDIAN__))

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(wchar_t)
AC_CHECK_SIZEOF(size_t)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(void*)

AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINTPTR_T


if test $ac_cv_sizeof_wchar_t -eq 4; then
	if test "$report_byteorder" = "LITTLE ENDIAN"; then
		AC_DEFINE(ICONV_UNICODE,"UTF-32LE")
	else
		AC_DEFINE(ICONV_UNICODE,"UTF-32BE")
	fi
else
	if test "$report_byteorder" = "LITTLE ENDIAN"; then
		AC_DEFINE(ICONV_UNICODE,"UTF-16LE")
	else
		AC_DEFINE(ICONV_UNICODE,"UTF-16BE")
	fi
fi



AC_CHECK_SIZEOF(fpos_t)


AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

PKG_PROG_PKG_CONFIG()


#--------------------------------------------------------------------
#       Check for various typedefs and provide substitutes if
#       they don't exist.
#--------------------------------------------------------------------

AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_OFF_T
AC_C_CONST
AC_STRUCT_TM
AX_FPOS_ISSTRUCT()
AX_TM_HAS_GMTOFF()
AC_PATH_XTRA



AC_CHECK_HEADERS([stdlib.h stdint.h inttypes.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([stdarg.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([math.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([sys/file.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([netinet/in.h])

AC_CHECK_HEADERS([strings.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([signal.h])
AC_CHECK_HEADERS([dirent.h])
AC_CHECK_HEADERS([fnmatch.h])
AC_CHECK_HEADERS([sys/stat.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([ctype.h])
AC_CHECK_HEADERS([wctype.h])
AC_CHECK_HEADERS([locale.h])
AC_CHECK_HEADERS([wchar.h])
AC_CHECK_HEADERS([widec.h])
AC_CHECK_HEADERS([pthread.h pthread_np.h])
AC_CHECK_HEADERS([sys/mman.h])

AC_FUNC_MKTIME
AC_CHECK_TYPE(size_t, unsigned)
AC_CHECK_TYPE(wchar_t, unsigned)

# Checks for library functions.
AC_FUNC_FSEEKO
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strtolower strtoupper])
AC_CHECK_FUNCS([bzero bcopy gettimeofday memmove memset mkdir socket __mingw_vasprintf vasprintf asprintf])
AC_CHECK_FUNCS([strcasecmp strlen strnlen strdup strerror strncasecmp strndup strstr strcasestr strcmp strncmp])
AC_CHECK_FUNCS([strncpy strlcpy strncat strlcat])
AC_CHECK_FUNCS([strtok strtok_r])
AC_CHECK_FUNCS([htol atoll atoi atol atof strtoll strtoull])
AC_CHECK_FUNCS([flock fcntl truncate ftruncate sync fsync unlink remove])
AC_CHECK_FUNCS([fgets fputs fgetws fputws fputc fgetc fputwc fgetwc mmap munmap sysconf pagesize])
AC_CHECK_FUNCS([usleep nanosleep localtime localtime_r gmtime gmtime_r])
AC_CHECK_FUNCS([signal])
AC_CHECK_FUNCS([strtok_r])
AC_CHECK_FUNCS([getpid getppid getuid geteuid])
AC_CHECK_FUNCS([mkstemp])
AC_CHECK_FUNCS([wcscmp wcsncmp wcscasecmp wcsncasecmp wcstol wcstoll wcslen wcsstr wcstoul wcstoull wcstod wcstof])
AC_CHECK_FUNCS([wcstombs mbstowcs wcsrtombs wcsnrtombs mbsrtowcs mbsnrtowcs])
AC_CHECK_FUNCS([wprintf fwprintf swprintf vwprintf vfwprintf vswprintf])
AC_CHECK_FUNCS([vsnprintf])
AC_CHECK_FUNCS([wstol wstoll watoi watoll wstod watof])
AC_CHECK_FUNCS([opendir closedir readdir readdir_r stat])
AC_CHECK_FUNCS([clock_gettime])

AC_CHECK_LIB(pthread, pthread_set_name_np,
	AC_DEFINE(HAVE_PTHREAD_SET_NAME_NP,1),,)
AC_CHECK_LIB(pthread, pthread_setname_np,
	AC_DEFINE(HAVE_PTHREAD_SETNAME_NP,1),,)


AC_CACHE_CHECK([for sa_len in struct sockaddr], ac_cv_struct_sa_len,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/socket.h>], [struct sockaddr s; s.sa_len;],
ac_cv_struct_sa_len=yes, ac_cv_struct_sa_len=no)])
if test $ac_cv_struct_sa_len = yes; then
  AC_DEFINE(HAVE_SOCKADDR_SA_LEN)
fi


#LDFLAGS="$LDFLAGS -L/usr/local/lib"
#CFLAGS="$CFLAGS -I/usr/local/include"


##################################################################################### PNG
AC_CHECK_LIBPNG([1.2],
            AC_DEFINE(HAVE_PNG, 1, [ Define if you have libpng. ])
            report_have_png="yes"
           :)

##################################################################################### JPEG
jpeg_note=""
AC_CHECK_LIBJPEG([6.0],
            AC_DEFINE(HAVE_JPEG, 1, [ Define if you have libjpeg. ])
            report_have_jpeg="yes"
           :)


##################################################################################### JPEG-TURBO
AC_CHECK_LIBJPEGTURBO([1.3.0],
            AC_DEFINE(HAVE_LIBJPEGTURBO, 1, [ Define if you have libjpeg-turbo. ])
            AC_DEFINE(HAVE_JPEG,1)
            report_have_jpeg="yes"
            jpeg_note="(using libjpeg-turbo)"
			JPEG_LIBS="$JPEG_LIBS $TURBOJPEG_LIBS"
			JPEG_CFLAGS="$JPEG_CFLAGS $TURBOJPEG_CFLAGS"

           :)
AC_SUBST(JPEG_CFLAGS)
AC_SUBST(JPEG_LIBS)


AC_CHECK_LIBMPG123([1.0.0],
            AC_DEFINE(HAVE_MPG123, 1, [ Define if you have mpg123 library. ])
            report_have_mpg123="yes"
			AC_SUBST(MPG123_CFLAGS)
			AC_SUBST(MPG123_LIBS)
            ,
            )


AM_ICONV
if test "$am_cv_func_iconv_works" = "yes"
then
	report_have_iconv="yes"
	ICONV_CFLAGS=$INCICONV
	ICONV_LIBS=$LIBICONV
	AC_SUBST(ICONV_CFLAGS)
	AC_SUBST(ICONV_LIBS)
fi


# Bzip2 Vorhanden?
AM_BZIP2
AX_CHECK_ZLIB(report_have_zlib="yes"
	AC_DEFINE([HAVE_LIBZ], [1], [Define to 1 if you have `z' library (-lz)])
	ZLIB_CFLAGS="-I${ZLIB_HOME}/include"
	ZLIB_LIBS="-L${ZLIB_HOME}/lib -lz"
	AC_SUBST(ZLIB_CFLAGS)
	AC_SUBST(ZLIB_LIBS)
)


dnl do we need to specify -lm explicitly?
AC_CHECK_FUNC(sin,,[AC_CHECK_LIB(m,sin)])

# PCRE vorhanden?
AX_PATH_LIB_PCRE(
            AC_DEFINE(HAVE_PCRE, 1, [ Define if you have pcre. ])
            report_have_pcre="yes"
            ,
            )

dnl Check for SDL
report_have_sdl2="no"
AM_PATH_SDL2([2.0.0],
            AC_DEFINE(HAVE_SDL2, 1, [ Define if you have libsdl2. ])
            report_have_sdl2="yes"
			AC_SUBST(SDL2_CFLAGS)
			AC_SUBST(SDL2_LIBS)
            ,:)


dnl Check for nasm
AC_MSG_NOTICE(checking if we can use assembly)
use_asm="none"
report_have_nasm="no"

case `uname -s` in
	MINGW*)
		;;
	*)
		case $target in
			i?86*|x86*|amd64*)
				AC_CHECK_NASM(2.0.0,
					report_have_nasm="yes"
				)
				;;
		esac
esac

case $target in
	x86_64*|amd64*)
		AC_DEFINE(HAVE_AMD64,1,)
		;;
esac




if test $report_have_nasm = "yes"
then
	use_asm="nasm"
	AC_DEFINE(HAVE_NASM,1,)
	AC_DEFINE(HAVE_X86_ASSEMBLER,1,)
	HAVE_X86_ASSEMBLER="ASM_RELEASE	=	\$(ASM_OBJ_RELEASE)
ASM_DEBUG	=	\$(ASM_OBJ_DEBUG)
ASM_COVERAGE	=	\$(ASM_OBJ_COVERAGE)"
	AC_SUBST(HAVE_X86_ASSEMBLER)
	ASM=${NASM}
	ASMFLAGS=${NASMFLAGS}
	ASMDEBUGFLAGS=${NASMDEBUGFLAGS}
	AC_SUBST(ASM)
	AC_SUBST(ASMFLAGS)
	AC_SUBST(ASMDEBUGFLAGS)
fi

SRCDIR=`pwd`
AC_SUBST(SRCDIR)




# If we support __thread, that can speed up tcmalloc a bit.
# Note, however, that our code tickles a bug in gcc < 4.1.2
# involving TLS and -fPIC (which our libraries will use) on x86:
#   http://gcc.gnu.org/ml/gcc-bugs/2006-09/msg02275.html
AC_MSG_CHECKING([for __thread])
AC_LINK_IFELSE([AC_LANG_PROGRAM([#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)) && ((__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 1) || (__GNUC__ == 4 && __GNUC_MINOR__ == 1 && __GNUC_PATCHLEVEL__ < 2))
#error gcc has this bug: http://gcc.gnu.org/ml/gcc-bugs/2006-09/msg02275.html
#endif], [static __thread int p = 0])],
               [AC_DEFINE(HAVE_TLS, 1,
                      Define to 1 if compiler supports __thread)
                AC_MSG_RESULT([yes])],
               [AC_MSG_RESULT([no])])

# We also need to check if the kernel supports __thread, which requires uname()
AC_CHECK_DECLS(uname,,, [#include <sys/utsname.h>])




WINDOWS_WINSOCK_LIBS=""
case "$host_os" in
  	*mingw64*|msys )
		MINGW32=yes
		MINGW64=yes
		AC_DEFINE(MINGW32, 1, [Define if you compile on Windows MINGW32])
		AC_DEFINE(MINGW64, 1, [Define if you compile on Windows MINGW64])
		AC_DEFINE(HAVE_LOCALTIME_R,1,[on mingw-w64 we always have localtime_r])
		AC_DEFINE(HAVE_GMTIME_R,1,[on mingw-w64 we always have gmtime_r])

		WINDOWS_WINSOCK_LIBS="-lws2_32"
  		;;
	* )
		MINGW32=no
		;;
esac

AC_SUBST(WINDOWS_WINSOCK_LIBS)


AC_SUBST(report_have_zlib)
AC_SUBST(report_have_bzip2)
AC_SUBST(report_have_png)
AC_SUBST(report_have_jpeg)
AC_SUBST(report_have_sdl2)
AC_SUBST(report_have_pcre)
AC_SUBST(report_have_iconv)
AC_SUBST(report_have_nasm)
AC_SUBST(report_have_pthreads)
AC_SUBST(report_have_mpg123)

###
JPEG_CFLAGS=""
OPENSSL_INCLUDES=""
MYSQL_CFLAGS=""
POSTGRESQL_CFLAGS=""
SQLITE3_CFLAGS=""
LAME_CFLAGS=""
LIBMICROHTTPD_CFLAGS=""
LIBCDIO_CFLAGS=""
LIBIDN_CFLAGS=""
LIBCURL_CPPFLAGS=""
LIBLDNS_CFLAGS=""
IMLIB_CFLAGS=""
FT2_CFLAGS=""

AC_SUBST(JPEG_CFLAGS)
AC_SUBST(OPENSSL_INCLUDES)
AC_SUBST(MYSQL_CFLAGS)
AC_SUBST(POSTGRESQL_CFLAGS)
AC_SUBST(SQLITE3_CFLAGS)
AC_SUBST(LAME_CFLAGS)
AC_SUBST(LIBMICROHTTPD_CFLAGS)
AC_SUBST(LIBCDIO_CFLAGS)
AC_SUBST(LIBIDN_CFLAGS)
AC_SUBST(LIBCURL_CPPFLAGS)
AC_SUBST(LIBLDNS_CFLAGS)
AC_SUBST(IMLIB_CFLAGS)
AC_SUBST(FT2_CFLAGS)





###############################################################################
# Output
###############################################################################

AC_OUTPUT


###############################################################################
# Report
###############################################################################

dnl report configuration
AC_MSG_RESULT([
******************************************************************************
** Configuration summary for $PACKAGE_NAME Version $PACKAGE_VERSION
** Target: $target_os        CPU: $target_cpu
**
** Byteorder: $report_byteorder
**   Size of char         $ac_cv_sizeof_char     Size of float        $ac_cv_sizeof_float
**   Size of short        $ac_cv_sizeof_short     Size of double       $ac_cv_sizeof_double
**   Size of int          $ac_cv_sizeof_int     Size of void*        $ac_cv_sizeof_voidp
**   Size of long         $ac_cv_sizeof_long     Size of wchar_t      $ac_cv_sizeof_wchar_t
**   Size of long long    $ac_cv_sizeof_long_long
**   Size of size_t       $ac_cv_sizeof_size_t
******************************************************************************
** Assembler:
**   Nasm (x86 assembler): $report_have_nasm
**
** Threads:                $report_have_pthreads
**
** Libs:
**   ZLIB library:         $report_have_zlib
**   BZIP2 library:        $report_have_bzip2
**   PCRE:                 $report_have_pcre
**   ICONV:                $report_have_iconv
**   PNG:                  $report_have_png
**   JPEG:                 $report_have_jpeg $jpeg_note
**   SDL2 library:         $report_have_sdl2
**   mpg123 library:       $report_have_mpg123
**
******************************************************************************
])

dnl check if we have all required libraries
missing=""


if test "$report_have_zlib" != "yes"
then
	missing="$missing  - zlib not found
"
fi

if test "$report_have_bzip2" != "yes"
then
	missing="$missing  - bzip2 not found
"
fi

if test "$report_have_pcre" != "yes"
then
	missing="$missing  - pcre not found
"
fi

if test "$report_have_sdl2" != "yes"
then
	missing="$missing  - sdl2 not found
"
fi

if test "$report_have_png" != "yes"
then
	missing="$missing  - libpng not found
"
fi

if test "$report_have_jpeg" != "yes"
then
	missing="$missing  - libpng not found
"
fi


if test "$report_have_iconv" != "yes"
then
	missing="$missing  - libiconv not found
"
fi

if test "$report_have_mpg123" != "yes"
then
	missing="$missing  - libmpg123 not found
"
fi





if test -n "$missing"
then
	AC_MSG_ERROR(Cannot build because of missing dependencies:
$missing)
fi
