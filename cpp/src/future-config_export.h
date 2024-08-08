
#ifndef FUTURE_CONFIG_EXPORT_H
#define FUTURE_CONFIG_EXPORT_H

#ifdef FUTURE_CONFIG_STATIC_DEFINE
#  define FUTURE_CONFIG_EXPORT
#  define FUTURE_CONFIG_NO_EXPORT
#else
#  ifndef FUTURE_CONFIG_EXPORT
#    ifdef future_config_EXPORTS
        /* We are building this library */
#		if defined(_MSC_VER)
#      		define FUTURE_CONFIG_EXPORT __declspec(dllexport)
#		else
#			define FUTURE_CONFIG_EXPORT __attribute__((visibility("default")))
#		endif
#    else
        /* We are using this library */
#		if defined(_MSC_VER)
#      		define FUTURE_CONFIG_EXPORT __declspec(dllimport)
#		else
#			define FUTURE_CONFIG_EXPORT __attribute__((visibility("default")))
#		endif
#    endif
#  endif

#  ifndef FUTURE_CONFIG_NO_EXPORT
#		if defined(_MSC_VER)
#    		define FUTURE_CONFIG_NO_EXPORT
#		else
#			define FUTURE_CONFIG_NO_EXPORT __attribute__((visibility("hidden")))
#		endif
#  endif
#endif

#ifndef FUTURE_CONFIG_DEPRECATED
#  	if defined(_MSC_VER)
#  		define FUTURE_CONFIG_DEPRECATED __declspec(deprecated)
#	else
#		define FUTURE_CONFIG_DEPRECATED __attribute__ ((__deprecated__))
#	endif
#endif

#ifndef FUTURE_CONFIG_DEPRECATED_EXPORT
#  define FUTURE_CONFIG_DEPRECATED_EXPORT FUTURE_CONFIG_EXPORT FUTURE_CONFIG_DEPRECATED
#endif

#ifndef FUTURE_CONFIG_DEPRECATED_NO_EXPORT
#  define FUTURE_CONFIG_DEPRECATED_NO_EXPORT FUTURE_CONFIG_NO_EXPORT FUTURE_CONFIG_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef FUTURE_CONFIG_NO_DEPRECATED
#    define FUTURE_CONFIG_NO_DEPRECATED
#  endif
#endif

#endif /* FUTURE_CONFIG_EXPORT_H */
