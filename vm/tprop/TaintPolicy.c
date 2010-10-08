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

#include "tprop/TaintPolicyPriv.h"

/* Constext free grammar for profile entries:
 *
 * <entry> ::= <etype> "." <var> | "return"
 * <etype> ::= "class" | "param" <num> | "return"
 * <var> ::= <signature> "%" <vname> | <var> "." <var>
 *
 * Where:
 * <signature> is the java type signature
 * <vname> is a valid java variable name
 *
 * Variable naming conv
 */


/* Policy for com.ibm.icu4jni.charset.NativeConverter **************/

TaintProfileEntry com_ibm_icu4jni_charset_NativeConverter_convertByteToChar_profile[] = {
    {"param1", "param3"}, /* byte[] input -> char[] output */
    {NULL, NULL}
};

TaintProfileEntry com_ibm_icu4jni_charset_NativeConverter_decode_profile[] = {
    {"param1", "param3"}, /* byte[] input -> char[] output */
    {NULL, NULL}
};

TaintProfileEntry com_ibm_icu4jni_charset_NativeConverter_convertCharToByte_profile[] = {
    {"param1", "param3"}, /* char[] input -> byte[] output */
    {NULL, NULL}
};

TaintProfileEntry com_ibm_icu4jni_charset_NativeConverter_encode_profile[] = {
    {"param1", "param3"}, /* char[] input -> byte[] output */
    {NULL, NULL}
};

TaintProfile com_ibm_icu4jni_charset_NativeConverter_methods[] = {
    {"convertByteToChar", com_ibm_icu4jni_charset_NativeConverter_convertByteToChar_profile},
    {"decode", com_ibm_icu4jni_charset_NativeConverter_decode_profile},
    {"convertCharToByte", com_ibm_icu4jni_charset_NativeConverter_convertCharToByte_profile},
    {"encode", com_ibm_icu4jni_charset_NativeConverter_encode_profile},
    {NULL, NULL}
};

/* Policy for foo.bar.name2 ****************************************/

TaintProfileEntry foo_bar_name2_method1_profile[] = {
    {"class.foo", "return"},
    {"param1.bar", "class.bar"},
    {NULL, NULL}
};

TaintProfileEntry foo_bar_name2_method2_profile[] = {
    {"class.foo", "return"},
    {"param1.bar", "class.bar"},
    {NULL, NULL}
};

TaintProfile foo_bar_name2_methods[] = {
    {"method1", foo_bar_name2_method1_profile},
    {"method2", foo_bar_name2_method2_profile},
    {NULL, NULL}
};

/* Class list ******************************************************/

TaintPolicy gDvmJniTaintPolicy[] = {
    {"Lcom/ibm/icu4jni/charset/NativeConverter;", com_ibm_icu4jni_charset_NativeConverter_methods, NULL},
    {"Lfoo/bar/name2;", foo_bar_name2_methods, NULL},
    {NULL, NULL, NULL}
};
