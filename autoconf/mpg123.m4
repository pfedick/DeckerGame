dnl AC_CHECK_LIBJPEG([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
AC_DEFUN([AC_CHECK_LIBMPG123],[
AC_MSG_CHECKING(if we should use libmpg123)
AC_ARG_WITH([mpg123],
	[  --with-mpg123[[=PATH]]     Prefix where libmpg123 is installed (optional)],
	[mpg123_prefix="$withval"],
	[mpg123_prefix="auto"])
	#echo "mpg123_prefix=$mpg123_prefix"

	if test "$mpg123_prefix" != "no"
	then
		AC_MSG_RESULT(yes)
		if test "$mpg123_prefix" != "yes"
		then
			intMPG123_LIBS="-L $mpg123_prefix/lib -lmpg123"
			intMPG123_CFLAGS="-I $mpg123_prefix/include"
		else
			intMPG123_LIBS="-lmpg123"
			intMPG123_CFLAGS=""
		fi

		am_save_CPPFLAGS="$CPPFLAGS"
		am_save_LIBS="$LIBS"
		am_save_LDFLAGS="$LDFLAGS"
		am_save_CFLAGS="$CFLAGS"

		LIBS="$LIBS $intMPG123_LIBS"
		CFLAGS="$CFLAGS $intMPG123_CFLAGS"
		AC_CHECK_HEADERS([mpg123.h],

			AC_CHECK_LIB(mpg123,mpg123_init,
				MPG123_LIBS=$intMPG123_LIBS
				MPG123_CFLAGS=$intMPG123_CFLAGS

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
