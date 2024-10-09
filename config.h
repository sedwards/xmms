/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

#if defined(USE_DMALLOC)
# include <glib.h>

/* 
 * G_malloc() and friends are redefined by glib.h when using DMALLOC.
 */
# define ALLOC(type, count)	    (type *) malloc (sizeof (type) * (count))
# define CALLOC(type, count)	    (type *) calloc ((count), sizeof (type))
# define REALLOC(mem, type, count)  (type *) realloc ((mem), sizeof (type) * (count))
# define MALLOC(size)		    (gpointer) malloc (size)
# define FREE(mem)		    (void) free (mem)


/*
 * Wrapper macro which invokes the given function and allocates the
 * returned string through the dmalloc library.
 */
# define g_wrap_it(function, args...) 					\
({									\
	gchar *__s1 = function (args);					\
	gchar *__s2 = strcpy (malloc (strlen (__s1) + 1), __s1);	\
	g_free_orig (__s1);						\
	__s2;								\
})


/* 
 * The follwing functions return strings which must be freed with
 * g_free().  We must therefore wrap them so that the strings are
 * allocated via the dmalloc library.
 */
# define g_strdup(str)                 g_wrap_it (g_strdup, str)
# define g_strdup_printf(fmt, args...) g_wrap_it (g_strdup_printf, fmt ,##args)
# define g_strdup_vprintf(fmt, argv)   g_wrap_it (g_strdup_vprintf, fmt, argv)
# define g_strndup(str, n)             g_wrap_it (g_strndup, str, n)
# define g_strnfill(len, fill_char)    g_wrap_it (g_strnfill, len, fill_char)
# define g_strconcat(str1, args...)    g_wrap_it (g_strconcat, str1 ,##args)
# define g_strjoin(sep, args...)       g_wrap_it (g_strjoin, sep ,##args)
# define g_strescape(str)              g_wrap_it (g_strescape, str)
# define g_strjoinv(sep, str_array)    g_wrap_it (g_strjoinv, sep, str_array)


void g_free_func (gpointer mem);
void g_free_orig (gpointer mem);

#else
# define g_free_func g_free
#endif

/* Define if you have the sched_setscheduler function.  */
/* #undef HAVE_SCHED_SETSCHEDULER */


/* #undef HAVE_SOUND */
#define PACKAGE "xmms"
#define VERSION "0.0.1"
/* #undef USE_3DNOW */
/* #undef WITH_SYMBOL_UNDERSCORE */
/* #undef WITH_SM */
/* #undef ENABLE_NLS */
/* #undef HAVE_CATGETS */
/* #undef HAVE_GETTEXT */
/* #undef HAVE_LC_MESSAGES */
/* #undef HAVE_STPCPY */
/* #undef DISABLE_STRICT_FPE */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Path to default cdaudio device. */
/* #undef CDDA_DEVICE */

/* Path to default cdaudio directory. */
/* #undef CDDA_DIRECTORY */

/* Path to OSS DSP, really just a data pipe, default is /dev/dsp. */
#define DEV_DSP "/dev/dsp"

/* Path to OSS mixer, default is /dev/mixer. */
#define DEV_MIXER "/dev/mixer"

/*  Define to disable per user plugin directory */
/* #undef DISABLE_USER_PLUGIN_DIR */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef ENABLE_NLS */

/* Define to 1 if you have the Mac OS X function
   CFLocaleCopyPreferredLanguages in the CoreFoundation framework. */
#define HAVE_CFLOCALECOPYPREFERREDLANGUAGES 1

/* Define to 1 if you have the Mac OS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
#define HAVE_CFPREFERENCESCOPYAPPVALUE 1

/* Define if nl_langinfo(CODESET) is available. */
#define HAVE_CODESET /**/

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
/* #undef HAVE_DCGETTEXT */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if libesd has the esd_get_latency function */
/* #undef HAVE_ESD_GET_LATENCY */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <fnmatch.h> header file. */
#define HAVE_FNMATCH_H 1

/* Define to 1 if you have the <fts.h> header file. */
#define HAVE_FTS_H 1

/* Define to 1 if you have the 'getmntinfo' function. */
/* #undef HAVE_GETMNTINFO */

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

/* Define to 1 if you have the <GL/glx.h> header file. */
/* #undef HAVE_GL_GLX_H */

/* Define to 1 if you have the <GL/gl.h> header file. */
/* #undef HAVE_GL_GL_H */

/* Define if you have the iconv() function and it works. */
#define HAVE_ICONV 1

/* Define to 1 if you have the 'iconv_open' function. */
/* #undef HAVE_ICONV_OPEN */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if zlib is available */
#define HAVE_LIBZ /**/

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <linux/cdrom.h> header file. */
/* #undef HAVE_LINUX_CDROM_H */

/* Define to 1 if you have the <linux/joystick.h> header file. */
/* #undef HAVE_LINUX_JOYSTICK_H */

/* Define to 1 if you have the <machine/soundcard.h> header file. */
/* #undef HAVE_MACHINE_SOUNDCARD_H */

/* Define to 1 if you have the 'mkdtemp' function. */
/* #undef HAVE_MKDTEMP */

/* Define if you have the FreeBSD newpcm driver */
/* #undef HAVE_NEWPCM */

/* Define if you have the Orbit libraries */
/* #undef HAVE_ORBIT */

/* Define to 1 if you have the 'sched_get_priority_max' function. */
/* #undef HAVE_SCHED_GET_PRIORITY_MAX */

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define to 1 if you have the 'srandomdev' function. */
/* #undef HAVE_SRANDOMDEV */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/cdio.h> header file. */
/* #undef HAVE_SYS_CDIO_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/sched.h> header file. */
/* #undef HAVE_SYS_SCHED_H */

/* Define to 1 if you have the <sys/soundcard.h> header file. */
/* #undef HAVE_SYS_SOUNDCARD_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Name of package */
#define PACKAGE "xmms"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* The size of 'long', as computed by sizeof. */
#define SIZEOF_LONG 0

/* Define to 1 if all of the C89 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Define to symbol prefix, if any */
#define SYMBOL_PREFIX "_"

/* Version number of package */
#define VERSION "0.0.1"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* XF86VidMode extension is present */
/* #undef XF86VIDMODE */

/* Define to empty if 'const' does not conform to ANSI C. */
/* #undef const */

/* Define to '__inline__' or '__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to int if the socklen_t type is missing */
/* #undef socklen_t */
