#pragma once

// Normally we don't export class templates (but do complete specializations),
// inline functions, and classes with only inline member functions. Exporting
// classes that inherit from non-exported/imported bases (e.g., std::string)
// will end up badly. The only known workarounds are to not inherit or to not
// export. Also, MinGW GCC doesn't like seeing non-exported functions being
// used before their inline definition. The workaround is to reorder code. In
// the end it's all trial and error.
//
// Exportation of explicit template instantiations is even hairier: MinGW GCC
// requires __declspec(dllexport) on the extern template declaration while VC
// wants it on the definition. Use LIBCANVAS_{DEC,DEF}EXPORT for that.
//

#if defined(LIBCANVAS_STATIC)
#  define LIBCANVAS_SYMEXPORT
#  define LIBCANVAS_DECEXPORT
#elif defined(LIBCANVAS_STATIC_BUILD)
#  define LIBCANVAS_SYMEXPORT
#  define LIBCANVAS_DECEXPORT
#  define LIBCANVAS_DEFEXPORT
#elif defined(LIBCANVAS_SHARED)
#  ifdef _WIN32
#    define LIBCANVAS_SYMEXPORT __declspec(dllimport)
#    define LIBCANVAS_DECEXPORT __declspec(dllimport)
#  else
#    define LIBCANVAS_SYMEXPORT
#    define LIBCANVAS_DECEXPORT
#  endif
#elif defined(LIBCANVAS_SHARED_BUILD)
#  ifdef _WIN32
#    define LIBCANVAS_SYMEXPORT __declspec(dllexport)
#    if defined(_MSC_VER)
#      define LIBCANVAS_DECEXPORT
#      define LIBCANVAS_DEFEXPORT __declspec(dllexport)
#    else
#      define LIBCANVAS_DECEXPORT __declspec(dllexport)
#      define LIBCANVAS_DEFEXPORT
#    endif
#  else
#    define LIBCANVAS_SYMEXPORT
#    define LIBCANVAS_DECEXPORT
#    define LIBCANVAS_DEFEXPORT
#  endif
#else
// If none of the above macros are defined, then we assume we are being used
// by some third-party build system that cannot/doesn't signal the library
// type. Note that this fallback works for both static and shared but in case
// of shared will be sub-optimal compared to having dllimport.
//
#  define LIBCANVAS_SYMEXPORT
#  define LIBCANVAS_DECEXPORT
#  define LIBCANVAS_DEFEXPORT
#endif
