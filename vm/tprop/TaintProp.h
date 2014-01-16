/* Note, this file should be included near the end of Dalvik.h */

#ifndef _DALVIK_TPROP_TAINT_PROP
#define _DALVIK_TPROP_TAINT_PROP

/* Called from dvmJniStartup() */
void dvmTaintPropJniStartup();

/* Called from dvmJniShutdown() */
void dvmTaintPropJniShutdown();

/* Main propagation */
void dvmTaintPropJniMethod(const u4* args, JValue* pResult, const Method* method);

#endif
