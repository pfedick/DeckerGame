dnl AC_CHECK_LIBDAV1D([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
AC_DEFUN([AC_CHECK_LIBDAV1D],[
AC_MSG_CHECKING(if we should use libdav1d)
AC_ARG_WITH([dav1d],
	[  --with-dav1d[[=PATH]]     Prefix where libdav1d is installed],
	[dav1d_prefix="$withval"],
	[dav1d_prefix="auto"])
	#echo "dav1d_prefix=$dav1d_prefix"

	if test "$dav1d_prefix" != "no"
	then
		AC_MSG_RESULT(yes)
		if test "$dav1d_prefix" != "yes"
		then
			intdav1d_LIBS="-L $dav1d_prefix/lib -ldav1d"
			intdav1d_CFLAGS="-I $dav1d_prefix/include"
		else
			intdav1d_LIBS="-ldav1d"
			intdav1d_CFLAGS=""
		fi

		am_save_CPPFLAGS="$CPPFLAGS"
		am_save_LIBS="$LIBS"
		am_save_LDFLAGS="$LDFLAGS"
		am_save_CFLAGS="$CFLAGS"

		LIBS="$LIBS $intdav1d_LIBS"
		CFLAGS="$CFLAGS $intdav1d_CFLAGS"
		AC_CHECK_HEADERS([dav1d/dav1d.h],

			AC_CHECK_LIB(dav1d,dav1d_open,
				DAV1D_LIBS=$intdav1d_LIBS
				DAV1D_CFLAGS=$intdav1d_CFLAGS

				ifelse([$2], , :, [$2])
				,
				ifelse([$3], , :, [$3])
			:)
		,
			ifelse([$3], , :, [$3])
		)
		CPPFLAGS="$am_save_CPPFLAGS"
		LIBS="$am_save_LIBS"
		LDFLAGS="$am_save_LDFLAGS"
		CFLAGS="$am_save_CFLAGS"
	else
		AC_MSG_RESULT(no)
		ifelse([$3], , :, [$3])
	fi
])
