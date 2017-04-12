#ifndef LIB_META_ATTR_H
#define LIB_META_ATTR_H

/// Function attributes.
///
/// This files contains various macros that may be invoked at the end of
/// function prototypes to indicate special properties. The attributes will
/// only be honored on supported platforms.
///
/// \file

#if __GNUC__ >= 3

#define ATTR_NORETURN() \
    __attribute__((__noreturn__))

#define ATTR_FORMAT_PRINTF(fmt, va_args) \
    __attribute__((format (printf, (fmt), (va_args))))

#else

/// Indicates that a function never returns after being called.
///
/// ## Example
///
/// ```c
/// void panic(const char *message)
///     ATTR_NORETURN();
/// ```
#define ATTR_NORETURN()

/// Indicates that a function takes a format string and format arguments
/// compatible with those accepted by printf().
///
/// ## Example
///
/// ```c
/// void log_printf(int logLevel, const char *format, ...)
///     ATTR_FORMAT_PRINTF(2, 3);
/// ```
///
/// \param fmt     The position of the format argument, starting from 1.
/// \param va_args The position of the variadic argument, starting from 1.
#define ATTR_FORMAT_PRINTF(fmt, va_args)

#endif

#endif
