/*------------------------------------------------------------------------------------
 * RSTL_VERSION
 * INTERNAL_STRINGIZE
 * INTERNAL_PRIMITIVE_STRINGIZE
 * COMPILER_EDG
 * COMPILER_EDG_VC_MODE
 * COMPILER_EDG_GCC_MODE
 * COMPILER_CPP11_ENABLED
 * COMPILER_RVCT
 * COMPILER_ARM
 * COMPILER_VERSION
 * COMPILER_NAME
 * COMPILER_CLANG
 * COMPILER_STRING
 * COMPILER_GNUC
 * COMPILER_BORLANDC
 * COMPILER_NO_MEMBER_TEMPLATES
 * COMPILER_NO_MEMBER_TEMPLATE_FRIENDS
 * COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION
 * COMPILER_NO_TEMPLATE_SPECIALIZATION
 * COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
 * COMPILER_NO_VOID_RETURNS
 * COMPILER_NO_EXCEPTION_STD_NAMESPACE
 * COMPILER_NO_DEDUCED_TYPENAME
 * COMPILER_NO_STATIC_CONSTANTS
 * COMPILER_NO_COVARIANT_RETURN_TYPE
 * COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP
 * COMPILER_NO_TEMPLATE_TEMPLATES
 * COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
 * COMPILER_NO_NOEXCEPT
 * COMPILER_MSVC
 * COMPILER_MICROSOFT
 * COMPILER_MSVCOLD
 * COMPILER_MSVC6
 * COMPILER_MSVC7
 * COMPILER_MSVC7_2003
 * COMPILER_MSVC7_1
 * COMPILER_MSVC8_2005
 * COMPILER_MSVC8_0
 * COMPILER_MSVC9_2008
 * COMPILER_MSVC9_0
 * COMPILER_MSVC_2010
 * COMPILER_MSVC10_0
 * COMPILER_MSVC_2011
 * COMPILER_MSVC11_0
 * COMPILER_MSVC_2012
 * COMPILER_MSVC12_0
 * COMPILER_MSVC_2013
 * COMPILER_MSVC13_0
 * COMPILER_MSVC_2015
 * COMPILER_MSVC14_0
 * COMPILER_MSVC_2017
 * COMPILER_MSVC15_0
 *------------------------------------------------------------------------------------*/
#ifndef RSTL_CONFIG_H
#define RSTL_CONFIG_H

#ifndef RSTL_VERSION
#   define RSTL_VERSION "1.0.0"
#   define RSTL_VERSION_N 100
#endif

#ifndef INTERNAL_STRINGIZE
#   define INTERNAL_STRINGIZE(x) INTERNAL_PRIMITIVE_STRINGIZE(x)
#endif
#ifndef INTERNAL_PRIMITIVE_STRINGIZE
#   define INTERNAL_PRIMITIVE_STRINGIZE(x) #x
#endif

#if defined(__EDG_VERSION__)
#   define COMPILER_EDG 1
#   if defined(_MSC_VER)
#       define COMPILER_EDG_VC_MODE 1
#   endif
#   if defined(__GNUC__)
#       define COMPILER_EDG_GCC_MODE 1
#   endif
#endif

#if !defined(COMPILER_CPP11_ENABLED) && defined(__cplusplus)
#   if (__cplusplus >= 201103L)     // Clang and GCC defines this like so in C++11 mode.
#       define COMPILER_CPP11_ENABLED 1
#   elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#       define COMPILER_CPP11_ENABLED 1
#   elif defined(_MSC_VER) && _MSC_VER >= 1600      // Microsoft unilaterally enables its C++11 support; there is no way to disable it.
#       define COMPILER_CPP11_ENABLED 1
#   endif
#endif

#if defined(__ARMCC_VERSION)
#   define COMPILER_RVCT 1
#   define COMPILER_ARM 1
#   define COMPILER_VERSION __ARMCC_VERSION
#   define COMPILER_NAME "RVCT"

#elif defined(__clang__) && !defined(_MSC_VER)      // Clang's GCC-compatible driver.
#   define COMPILER_CLANG 1
#   define COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
#   define COMPILER_NAME "clang"
#   define COMPILER_STRING COMPILER_NAME __clang_version__

#elif defined(__GNUC__)     // GCC compilers exist for many platforms.
#   define COMPILER_GNUC 1
#   define COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#   define COMPILER_NAME "GCC"
#   define COMPILER_STRING COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( __GNUC__ ) "." INTERNAL_STRINGIZE( __GNUC_MINOR__ )
#   if (__GNUC__ == 2) && (__GNUC_MINOR__ < 95)     // If GCC < 2.95
#       define COMPILER_NO_MEMBER_TEMPLATES 1
#   endif
#   if (__GNUC__ == 2) && (__GNUC_MINOR__ <= 97)     // If GCC <= 2.97
#       define COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1
#   endif
#   if (__GNUC__ == 3) && ((__GNUC_MINOR__ == 1) || (__GNUC_MINOR__ == 2))      // If GCC 3.1 or 3.2
#       define COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS 1
#   endif

#elif defined(__BORLANDC__)
#   define COMPILER_BORLANDC 1
#   define COMPILER_VERSION __BORLANDC__
#   define COMPILER_NAME "Borland C"
#   if (__BORLANDC__ <= 0x0500)
#       define COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1
#   endif
#   if (__BORLANDC__ >= 0x561) && (__BORLANDC__ < 0x600)
#       define COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION 1
#   endif

#elif defined(__ICL) || defined(__ICC)
#   define COMPILER_INTEL 1
#   if define (_MSC_VER)
#       define COMPILER_MSVC 1
#       define COMPILER_MICROSOFT 1
#   endif
#   if defined(__GNUC__)
#       define COMPILER_GNUC 1
#   endif
#   if defined(__ICL)
#       define COMPILER_VERSION __ICL
#   elif defined(__ICC)
#       define COMPILER_VERSION __ICC
#   endif
#   define COMPILER_NAME "Intel C++"
#   if defined(_MSC_VER)
#       define COMPILE_STRING COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ ) ", VC++ version " INTERNAL_STRINGIZE( _MSC_VER )
#   elif define(__GNUC__)
#       define COMPILE_STRING COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ ) ", GCC version " INTERNAL_STRINGIZE( __GNUC__ )
#   else
#       define COMPILE_STRING COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ )
#   endif

#elif defined(_MSC_VER)
#   define COMPILER_MSVC 1
#   define COMPILER_MICROSOFT 1
#   define COMPILER_VERSION _MSC_VER
#   define COMPILER_NAME "Microsoft Visual C++"
#   if defined(__clang__)
#       define COMPILER_CLANG_CL 1      // Clang's MSVC-compatible driver
#   endif
#   define STANDARD_LIBRARY_MSVC 1
#   define STANDARD_LIBRARY_MICROSOFT 1
#   if (_MSC_VER <= 1200)       // If Vc6.x and earlier
#       if (_MSC_VER < 1200)
#           define COMPILER_MSVCOLD 1
#           define COMPILER_NO_TEMPLATE_SPECIALIZATION 1
#       else
#           define COMPILER_MSVC6 1
#       endif
#       define COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS 1     // The compiler compiles this OK, but executes it wrong. Fixed in VC7.0
#       define COMPILER_NO_VOID_RETURNS 1                             // The compiler fails to compile such cases. Fixed in VC7.0
#       define COMPILER_NO_EXCEPTION_STD_NAMESPACE 1                  // The compiler fails to compile such cases. Fixed in VC7.0
#       define COMPILER_NO_DEDUCED_TYPENAME 1                         // The compiler fails to compile such cases. Fixed in VC7.0
#       define COMPILER_NO_STATIC_CONSTANTS 1                         // The compiler fails to compile such cases. Fixed in VC7.0
#       define COMPILER_NO_COVARIANT_RETURN_TYPE 1                    // The compiler fails to compile such cases. Fixed in VC7.1
#       define COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP 1                // The compiler compiles this OK, but executes it wrong. Fixed in VC7.1
#       define COMPILER_NO_TEMPLATE_TEMPLATES 1                       // The compiler fails to compile such cases. Fixed in VC7.1
#       define COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION 1          // The compiler fails to compile such cases. Fixed in VC7.1
#       define COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1                  // The compiler fails to compile such cases. Fixed in VC7.1
#   elif (_MSC_VER <= 1300)
#       define COMPILER_MSVC7 1
#       define COMPILER_NO_COVARIANT_RETURN_TYPE 1                    // The compiler fails to compile such cases. Fixed in VC7.1
#       define COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP 1                // The compiler compiles this OK, but executes it wrong. Fixed in VC7.1
#       define COMPILER_NO_TEMPLATE_TEMPLATES 1                       // The compiler fails to compile such cases. Fixed in VC7.1
#       define COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION 1          // The compiler fails to compile such cases. Fixed in VC7.1
#       define COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1                  // The compiler fails to compile such cases. Fixed in VC7.1
#       define COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION 1           // This is the case only for VC7.0 and not VC6 or VC7.1+. Fixed in VC7.1
#   elif (_MSC_VER < 1400)      // VS2003 _MSC_VER of 1300 means VC7 (VS2003)
#       define COMPILER_MSVC7_2003 1
#       define COMPILER_MSVC7_1    1
#   elif (_MSC_VER < 1500)      // VS2005 _MSC_VER of 1400 means VC8 (VS2005)
#       define COMPILER_MSVC8_2005 1
#       define COMPILER_MSVC8_0    1
#   elif (_MSC_VER < 1600)      // VS2008 _MSC_VER of 1500 means VC9 (VS2008)
#       define COMPILER_MSVC9_2008 1
#       define COMPILER_MSVC9_0    1
#   elif (_MSC_VER < 1700)      // VS2010 _MSC_VER of 1600 means VC10 (VS2010)
#       define COMPILER_MSVC_2010 1
#       define COMPILER_MSVC10_0  1
#   elif (_MSC_VER < 1800)      // VS2012 _MSC_VER of 1700 means VS2011/VS2012
#       define COMPILER_MSVC_2011 1     // Microsoft changed the name to VS2012 before shipping, despite referring to it as VS2011 up to just a few weeks before shipping.
#       define COMPILER_MSVC11_0  1
#       define COMPILER_MSVC_2012 1
#       define COMPILER_MSVC12_0  1
#   elif (_MSC_VER < 1900)      // VS2013 _MSC_VER of 1800 means VS2013
#       define COMPILER_MSVC_2013 1
#       define COMPILER_MSVC13_0  1
#   elif (_MSC_VER < 1910)      // VS2015 _MSC_VER of 1900 means VS2015
#       define COMPILER_MSVC_2015 1
#       define COMPILER_MSVC14_0  1
#   elif (_MSC_VER < 1911)      // VS2017 _MSC_VER of 1910 means VS2017
#       define COMPILER_MSVC_2017 1
#       define COMPILER_MSVC15_0  1
#endif

#ifndef COMPILER_STRING
#   define COMPILER_STRING COMPILER_NAME " compiler, version " INTERNAL_PRIMITIVE_STRINGIZE(COMPILER_VERSION)
#endif

#endif

#if !defined(COMPILER_NO_NOEXCEPT)
#   if defined(COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (COMPILER_VERSION >= 1900)

#   elif defined(COMPILER_CPP11_ENABLED) && defined(__clang__) && (COMPILER_VERSION >= 401) && defined(__apple_build_version__)

#   elif defined(COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)

#   elif defined(COMPILER_CPP11_ENABLED) && defined(__clang__) && (COMPILER_VERSION >= 300) && !defined(__apple_build_version__)

#   elif defined(COMPILER_CPP11_ENABLED) &&defined(__GNUC__) && (COMPILER_VERSION >= 4006)

#   else
#       define COMPILER_NO_NOEXCEPT 1
#   endif
#endif

#endif // RSTL_CONFIG_H
