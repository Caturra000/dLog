#ifndef __DLOG_PREPROCESS_H__
#define __DLOG_PREPROCESS_H__

#define DLOG_STRIPPATH(s) strrchr("/" __FILE__, '/') + 1

// compile time
#define DLOG_FILENAME DLOG_STRIPPATH(__FILE__)

namespace dlog {}

#endif