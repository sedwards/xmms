/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

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

/* #undef HAVE_SCHED_SETSCHEDULER */
#define HAVE_NANOSLEEP 1


#define PACKAGE "libxmms"
#define VERSION "3.0.2"

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

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

/* Define to 1 if you have the <sys/sched.h> header file. */
/* #undef HAVE_SYS_SCHED_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Name of package */
#define PACKAGE "libxmms"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "your-email@example.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libxmms"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libxmms 3.0.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libxmms"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.0.2"

/* Define to 1 if all of the C89 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. This
   macro is obsolete. */
/* #undef TIME_WITH_SYS_TIME */

/* Version number of package */
#define VERSION "3.0.2"
