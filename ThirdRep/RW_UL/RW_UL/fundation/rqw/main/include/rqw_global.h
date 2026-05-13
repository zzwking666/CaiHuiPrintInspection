#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(RQW_LIB)
#  define RQW_EXPORT Q_DECL_EXPORT
# else
#  define RQW_EXPORT Q_DECL_IMPORT
# endif
#else
# define RQW_EXPORT
#endif
