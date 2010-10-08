/*
 * Copyright (c) 2010 The Pennsylvania State University
 * Systems and Internet Infrastructure Security Laboratory
 *
 * Authors: William Enck <enck@cse.psu.edu>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


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
