/*
 * Copyright (C) 2008 The Android Open Source Project
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

/*
 * Helper functions to access data fields in Objects.
 */
#ifndef DALVIK_OO_OBJECTINLINES_H_
#define DALVIK_OO_OBJECTINLINES_H_

/*
 * Store a single value in the array, and if the value isn't null,
 * note in the write barrier.
 */
INLINE void dvmSetObjectArrayElement(const ArrayObject* obj, int index,
                                     Object* val) {
    ((Object **)(void *)(obj)->contents)[index] = val;
    if (val != NULL) {
        dvmWriteBarrierArray(obj, index, index + 1);
    }
}


/*
 * Field access functions.  Pass in the word offset from Field->byteOffset.
 *
 * We guarantee that long/double field data is 64-bit aligned, so it's safe
 * to access them with ldrd/strd on ARM.
 *
 * The VM treats all fields as 32 or 64 bits, so the field set functions
 * write 32 bits even if the underlying type is smaller.
 *
 * Setting Object types to non-null values includes a call to the
 * write barrier.
 */
#define BYTE_OFFSET(_ptr, _offset)  ((void*) (((u1*)(_ptr)) + (_offset)))

INLINE JValue* dvmFieldPtr(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset));
}

INLINE bool dvmGetFieldBoolean(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->z;
}
INLINE s1 dvmGetFieldByte(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->b;
}
INLINE s2 dvmGetFieldShort(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->s;
}
INLINE u2 dvmGetFieldChar(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->c;
}
INLINE s4 dvmGetFieldInt(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->i;
}
INLINE s8 dvmGetFieldLong(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->j;
}
INLINE float dvmGetFieldFloat(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->f;
}
INLINE double dvmGetFieldDouble(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->d;
}
INLINE Object* dvmGetFieldObject(const Object* obj, int offset) {
    return ((JValue*)BYTE_OFFSET(obj, offset))->l;
}
INLINE bool dvmGetFieldBooleanVolatile(const Object* obj, int offset) {
    s4* ptr = &((JValue*)BYTE_OFFSET(obj, offset))->i;
    return (bool)android_atomic_acquire_load(ptr);
}
INLINE s1 dvmGetFieldByteVolatile(const Object* obj, int offset) {
    s4* ptr = &((JValue*)BYTE_OFFSET(obj, offset))->i;
    return (s1)android_atomic_acquire_load(ptr);
}
INLINE s2 dvmGetFieldShortVolatile(const Object* obj, int offset) {
    s4* ptr = &((JValue*)BYTE_OFFSET(obj, offset))->i;
    return (s2)android_atomic_acquire_load(ptr);
}
INLINE u2 dvmGetFieldCharVolatile(const Object* obj, int offset) {
    s4* ptr = &((JValue*)BYTE_OFFSET(obj, offset))->i;
    return (u2)android_atomic_acquire_load(ptr);
}
INLINE s4 dvmGetFieldIntVolatile(const Object* obj, int offset) {
    s4* ptr = &((JValue*)BYTE_OFFSET(obj, offset))->i;
    return android_atomic_acquire_load(ptr);
}
INLINE float dvmGetFieldFloatVolatile(const Object* obj, int offset) {
    union { s4 ival; float fval; } alias;
    s4* ptr = &((JValue*)BYTE_OFFSET(obj, offset))->i;
    alias.ival = android_atomic_acquire_load(ptr);
    return alias.fval;
}
INLINE s8 dvmGetFieldLongVolatile(const Object* obj, int offset) {
    const s8* addr = (const s8*)BYTE_OFFSET(obj, offset);
    s8 val = dvmQuasiAtomicRead64(addr);
    ANDROID_MEMBAR_FULL();
    return val;
}
INLINE double dvmGetFieldDoubleVolatile(const Object* obj, int offset) {
    union { s8 lval; double dval; } alias;
    const s8* addr = (const s8*)BYTE_OFFSET(obj, offset);
    alias.lval = dvmQuasiAtomicRead64(addr);
    ANDROID_MEMBAR_FULL();
    return alias.dval;
}
INLINE Object* dvmGetFieldObjectVolatile(const Object* obj, int offset) {
    Object** ptr = &((JValue*)BYTE_OFFSET(obj, offset))->l;
    return (Object*)android_atomic_acquire_load((int32_t*)ptr);
}

#ifdef WITH_TAINT_TRACKING
INLINE u4 dvmGetFieldTaint(const Object* obj, int offset) {
    return (*(u4*)BYTE_OFFSET(obj, offset+sizeof(u4)));
}
INLINE u4 dvmGetFieldTaintWide(const Object* obj, int offset) {
    return (*(u4*)BYTE_OFFSET(obj, offset+sizeof(u4)+sizeof(u4)));
}
INLINE u4 dvmGetFieldTaintVolatile(const Object* obj, int offset) {
    s4* ptr = &(*(s4*)(BYTE_OFFSET(obj, offset+sizeof(u4))));
    return (u4)android_atomic_acquire_load(ptr);
}
INLINE u4 dvmGetFieldTaintWideVolatile(const Object* obj, int offset) {
    s4* ptr = &(*(s4*)BYTE_OFFSET(obj, offset+sizeof(u4)+sizeof(u4)));
    return (u4)android_atomic_acquire_load(ptr);
}
#define dvmGetFieldTaintBoolean(_obj, _offset) dvmGetFieldTaint(_obj, _offset)
#define dvmGetFieldTaintByte(_obj, _offset)    dvmGetFieldTaint(_obj, _offset)
#define dvmGetFieldTaintShort(_obj, _offset)   dvmGetFieldTaint(_obj, _offset)
#define dvmGetFieldTaintChar(_obj, _offset)    dvmGetFieldTaint(_obj, _offset)
#define dvmGetFieldTaintInt(_obj, _offset)     dvmGetFieldTaint(_obj, _offset)
#define dvmGetFieldTaintLong(_obj, _offset)    dvmGetFieldTaintWide(_obj, _offset)
#define dvmGetFieldTaintFloat(_obj, _offset)   dvmGetFieldTaint(_obj, _offset)
#define dvmGetFieldTaintDouble(_obj, _offset)  dvmGetFieldTaintWide(_obj, _offset)
#define dvmGetFieldTaintObject(_obj, _offset)  dvmGetFieldTaint(_obj, _offset)

#define dvmGetFieldTaintBooleanVolatile(_obj, _offset) dvmGetFieldTaintVolatile(_obj, _offset)
#define dvmGetFieldTaintByteVolatile(_obj, _offset)    dvmGetFieldTaintVolatile(_obj, _offset)
#define dvmGetFieldTaintShortVolatile(_obj, _offset)   dvmGetFieldTaintVolatile(_obj, _offset)
#define dvmGetFieldTaintCharVolatile(_obj, _offset)    dvmGetFieldTaintVolatile(_obj, _offset)
#define dvmGetFieldTaintIntVolatile(_obj, _offset)     dvmGetFieldTaintVolatile(_obj, _offset)
#define dvmGetFieldTaintLongVolatile(_obj, _offset)    dvmGetFieldTaintWideVolatile(_obj, _offset)
#define dvmGetFieldTaintFloatVolatile(_obj, _offset)   dvmGetFieldTaintVolatile(_obj, _offset)
#define dvmGetFieldTaintDoubleVolatile(_obj, _offset)  dvmGetFieldTaintWideVolatile(_obj, _offset)
#define dvmGetFieldTaintObjectVolatile(_obj, _offset)  dvmGetFieldTaintVolatile(_obj, _offset)
#else
#define dvmGetFieldTaint(_obj, _offset)        ((void)0)
#define dvmGetFieldTaintWide(_obj, _offset)    ((void)0)
#define dvmGetFieldTaintBoolean(_obj, _offset) ((void)0)
#define dvmGetFieldTaintByte(_obj, _offset)    ((void)0)
#define dvmGetFieldTaintShort(_obj, _offset)   ((void)0)
#define dvmGetFieldTaintChar(_obj, _offset)    ((void)0)
#define dvmGetFieldTaintInt(_obj, _offset)     ((void)0)
#define dvmGetFieldTaintLong(_obj, _offset)    ((void)0)
#define dvmGetFieldTaintFloat(_obj, _offset)   ((void)0)
#define dvmGetFieldTaintDouble(_obj, _offset)  ((void)0)
#define dvmGetFieldTaintObject(_obj, _offset)  ((void)0)

#define dvmGetFieldTaintBooleanVolatile(_obj, _offset) ((void)0)
#define dvmGetFieldTaintByteVolatile(_obj, _offset)    ((void)0)
#define dvmGetFieldTaintShortVolatile(_obj, _offset)   ((void)0)
#define dvmGetFieldTaintCharVolatile(_obj, _offset)    ((void)0)
#define dvmGetFieldTaintIntVolatile(_obj, _offset)     ((void)0)
#define dvmGetFieldTaintLongVolatile(_obj, _offset)    ((void)0)
#define dvmGetFieldTaintFloatVolatile(_obj, _offset)   ((void)0)
#define dvmGetFieldTaintDoubleVolatile(_obj, _offset)  ((void)0)
#define dvmGetFieldTaintObjectVolatile(_obj, _offset)  ((void)0)
#endif

INLINE void dvmSetFieldBoolean(Object* obj, int offset, bool val) {
    ((JValue*)BYTE_OFFSET(obj, offset))->i = val;
}
INLINE void dvmSetFieldByte(Object* obj, int offset, s1 val) {
    ((JValue*)BYTE_OFFSET(obj, offset))->i = val;
}
INLINE void dvmSetFieldShort(Object* obj, int offset, s2 val) {
    ((JValue*)BYTE_OFFSET(obj, offset))->i = val;
}
INLINE void dvmSetFieldChar(Object* obj, int offset, u2 val) {
    ((JValue*)BYTE_OFFSET(obj, offset))->i = val;
}
INLINE void dvmSetFieldInt(Object* obj, int offset, s4 val) {
    ((JValue*)BYTE_OFFSET(obj, offset))->i = val;
}
INLINE void dvmSetFieldFloat(Object* obj, int offset, float val) {
    ((JValue*)BYTE_OFFSET(obj, offset))->f = val;
}
INLINE void dvmSetFieldLong(Object* obj, int offset, s8 val) {
    ((JValue*)BYTE_OFFSET(obj, offset))->j = val;
}
INLINE void dvmSetFieldDouble(Object* obj, int offset, double val) {
    ((JValue*)BYTE_OFFSET(obj, offset))->d = val;
}
INLINE void dvmSetFieldObject(Object* obj, int offset, Object* val) {
    JValue* lhs = (JValue*)BYTE_OFFSET(obj, offset);
    lhs->l = val;
    if (val != NULL) {
        dvmWriteBarrierField(obj, &lhs->l);
    }
}
INLINE void dvmSetFieldIntVolatile(Object* obj, int offset, s4 val) {
    s4* ptr = &((JValue*)BYTE_OFFSET(obj, offset))->i;
    /*
     * TODO: add an android_atomic_synchronization_store() function and
     * use it in the 32-bit volatile set handlers.  On some platforms we
     * can use a fast atomic instruction and avoid the barriers.
     */
    ANDROID_MEMBAR_STORE();
    *ptr = val;
    ANDROID_MEMBAR_FULL();
}
INLINE void dvmSetFieldBooleanVolatile(Object* obj, int offset, bool val) {
    dvmSetFieldIntVolatile(obj, offset, val);
}
INLINE void dvmSetFieldByteVolatile(Object* obj, int offset, s1 val) {
    dvmSetFieldIntVolatile(obj, offset, val);
}
INLINE void dvmSetFieldShortVolatile(Object* obj, int offset, s2 val) {
    dvmSetFieldIntVolatile(obj, offset, val);
}
INLINE void dvmSetFieldCharVolatile(Object* obj, int offset, u2 val) {
    dvmSetFieldIntVolatile(obj, offset, val);
}
INLINE void dvmSetFieldFloatVolatile(Object* obj, int offset, float val) {
    union { s4 ival; float fval; } alias;
    alias.fval = val;
    dvmSetFieldIntVolatile(obj, offset, alias.ival);
}
INLINE void dvmSetFieldLongVolatile(Object* obj, int offset, s8 val) {
    s8* addr = (s8*)BYTE_OFFSET(obj, offset);
    dvmQuasiAtomicSwap64Sync(val, addr);
}
INLINE void dvmSetFieldDoubleVolatile(Object* obj, int offset, double val) {
    union { s8 lval; double dval; } alias;
    alias.dval = val;
    dvmSetFieldLongVolatile(obj, offset, alias.lval);
}
INLINE void dvmSetFieldObjectVolatile(Object* obj, int offset, Object* val) {
    Object** ptr = &((JValue*)BYTE_OFFSET(obj, offset))->l;
    ANDROID_MEMBAR_STORE();
    *ptr = val;
    ANDROID_MEMBAR_FULL();
    if (val != NULL) {
        dvmWriteBarrierField(obj, ptr);
    }
}

#ifdef WITH_TAINT_TRACKING
INLINE void dvmSetFieldTaint(Object* obj, int offset, u4 tag) {
    (*(u4*)BYTE_OFFSET(obj, offset+sizeof(u4))) = tag;
}
INLINE void dvmSetFieldTaintWide(Object* obj, int offset, u4 tag) {
    (*(u4*)BYTE_OFFSET(obj, offset+sizeof(u4)+sizeof(u4))) = tag;
}
INLINE void dvmSetFieldTaintVolatile(Object* obj, int offset, u4 tag) {
    s4* ptr = &(*(s4*)BYTE_OFFSET(obj, offset+sizeof(u4)));
    android_atomic_release_store(tag, ptr);
}
INLINE void dvmSetFieldTaintWideVolatile(Object* obj, int offset, u4 tag) {
    s4* ptr = &(*(s4*)BYTE_OFFSET(obj, offset+sizeof(u4)+sizeof(u4)));
    android_atomic_release_store(tag, ptr);
}
#define dvmSetFieldTaintBoolean(_obj, _offset, _tag) dvmSetFieldTaint(_obj, _offset, _tag)
#define dvmSetFieldTaintByte(_obj, _offset, _tag)    dvmSetFieldTaint(_obj, _offset, _tag)
#define dvmSetFieldTaintShort(_obj, _offset, _tag)   dvmSetFieldTaint(_obj, _offset, _tag)
#define dvmSetFieldTaintChar(_obj, _offset, _tag)    dvmSetFieldTaint(_obj, _offset, _tag)
#define dvmSetFieldTaintInt(_obj, _offset, _tag)     dvmSetFieldTaint(_obj, _offset, _tag)
#define dvmSetFieldTaintLong(_obj, _offset, _tag)    dvmSetFieldTaintWide(_obj, _offset, _tag)
#define dvmSetFieldTaintFloat(_obj, _offset, _tag)   dvmSetFieldTaint(_obj, _offset, _tag)
#define dvmSetFieldTaintDouble(_obj, _offset, _tag)  dvmSetFieldTaintWide(_obj, _offset, _tag)
#define dvmSetFieldTaintObject(_obj, _offset, _tag)  dvmSetFieldTaint(_obj, _offset, _tag)

#define dvmSetFieldTaintBooleanVolatile(_obj, _offset, _tag) dvmSetFieldTaintVolatile(_obj, _offset, _tag)
#define dvmSetFieldTaintByteVolatile(_obj, _offset, _tag)    dvmSetFieldTaintVolatile(_obj, _offset, _tag)
#define dvmSetFieldTaintShortVolatile(_obj, _offset, _tag)   dvmSetFieldTaintVolatile(_obj, _offset, _tag)
#define dvmSetFieldTaintCharVolatile(_obj, _offset, _tag)    dvmSetFieldTaintVolatile(_obj, _offset, _tag)
#define dvmSetFieldTaintIntVolatile(_obj, _offset, _tag)     dvmSetFieldTaintVolatile(_obj, _offset, _tag)
#define dvmSetFieldTaintLongVolatile(_obj, _offset, _tag)    dvmSetFieldTaintWideVolatile(_obj, _offset, _tag)
#define dvmSetFieldTaintFloatVolatile(_obj, _offset, _tag)   dvmSetFieldTaintVolatile(_obj, _offset, _tag)
#define dvmSetFieldTaintDoubleVolatile(_obj, _offset, _tag)  dvmSetFieldTaintWideVolatile(_obj, _offset, _tag)
#define dvmSetFieldTaintObjectVolatile(_obj, _offset, _tag)  dvmSetFieldTaintVolatile(_obj, _offset, _tag)
#else
#define dvmSetFieldTaint(_obj, _offset, _tag)        ((void)0)
#define dvmSetFieldTaintWide(_obj, _offset, _tag)    ((void)0)
#define dvmSetFieldTaintBoolean(_obj, _offset, _tag) ((void)0)
#define dvmSetFieldTaintByte(_obj, _offset, _tag)    ((void)0)
#define dvmSetFieldTaintShort(_obj, _offset, _tag)   ((void)0)
#define dvmSetFieldTaintChar(_obj, _offset, _tag)    ((void)0)
#define dvmSetFieldTaintInt(_obj, _offset, _tag)     ((void)0)
#define dvmSetFieldTaintLong(_obj, _offset, _tag)    ((void)0)
#define dvmSetFieldTaintFloat(_obj, _offset, _tag)   ((void)0)
#define dvmSetFieldTaintDouble(_obj, _offset, _tag)  ((void)0)
#define dvmSetFieldTaintObject(_obj, _offset, _tag)  ((void)0)

#define dvmSetFieldTaintBooleanVolatile(_obj, _offset, _tag) ((void)0)
#define dvmSetFieldTaintByteVolatile(_obj, _offset, _tag)    ((void)0)
#define dvmSetFieldTaintShortVolatile(_obj, _offset, _tag)   ((void)0)
#define dvmSetFieldTaintCharVolatile(_obj, _offset, _tag)    ((void)0)
#define dvmSetFieldTaintIntVolatile(_obj, _offset, _tag)     ((void)0)
#define dvmSetFieldTaintLongVolatile(_obj, _offset, _tag)    ((void)0)
#define dvmSetFieldTaintFloatVolatile(_obj, _offset, _tag)   ((void)0)
#define dvmSetFieldTaintDoubleVolatile(_obj, _offset, _tag)  ((void)0)
#define dvmSetFieldTaintObjectVolatile(_obj, _offset, _tag)  ((void)0)
#endif

/*
 * Static field access functions.
 */
INLINE JValue* dvmStaticFieldPtr(const StaticField* sfield) {
    return (JValue*)&sfield->value;
}

INLINE bool dvmGetStaticFieldBoolean(const StaticField* sfield) {
    return sfield->value.z;
}
INLINE s1 dvmGetStaticFieldByte(const StaticField* sfield) {
    return sfield->value.b;
}
INLINE s2 dvmGetStaticFieldShort(const StaticField* sfield) {
    return sfield->value.s;
}
INLINE u2 dvmGetStaticFieldChar(const StaticField* sfield) {
    return sfield->value.c;
}
INLINE s4 dvmGetStaticFieldInt(const StaticField* sfield) {
    return sfield->value.i;
}
INLINE float dvmGetStaticFieldFloat(const StaticField* sfield) {
    return sfield->value.f;
}
INLINE s8 dvmGetStaticFieldLong(const StaticField* sfield) {
    return sfield->value.j;
}
INLINE double dvmGetStaticFieldDouble(const StaticField* sfield) {
    return sfield->value.d;
}
INLINE Object* dvmGetStaticFieldObject(const StaticField* sfield) {
    return sfield->value.l;
}
INLINE bool dvmGetStaticFieldBooleanVolatile(const StaticField* sfield) {
    const s4* ptr = &(sfield->value.i);
    return (bool)android_atomic_acquire_load((s4*)ptr);
}
INLINE s1 dvmGetStaticFieldByteVolatile(const StaticField* sfield) {
    const s4* ptr = &(sfield->value.i);
    return (s1)android_atomic_acquire_load((s4*)ptr);
}
INLINE s2 dvmGetStaticFieldShortVolatile(const StaticField* sfield) {
    const s4* ptr = &(sfield->value.i);
    return (s2)android_atomic_acquire_load((s4*)ptr);
}
INLINE u2 dvmGetStaticFieldCharVolatile(const StaticField* sfield) {
    const s4* ptr = &(sfield->value.i);
    return (u2)android_atomic_acquire_load((s4*)ptr);
}
INLINE s4 dvmGetStaticFieldIntVolatile(const StaticField* sfield) {
    const s4* ptr = &(sfield->value.i);
    return android_atomic_acquire_load((s4*)ptr);
}
INLINE float dvmGetStaticFieldFloatVolatile(const StaticField* sfield) {
    union { s4 ival; float fval; } alias;
    const s4* ptr = &(sfield->value.i);
    alias.ival = android_atomic_acquire_load((s4*)ptr);
    return alias.fval;
}
INLINE s8 dvmGetStaticFieldLongVolatile(const StaticField* sfield) {
    const s8* addr = &sfield->value.j;
    s8 val = dvmQuasiAtomicRead64(addr);
    ANDROID_MEMBAR_FULL();
    return val;
}
INLINE double dvmGetStaticFieldDoubleVolatile(const StaticField* sfield) {
    union { s8 lval; double dval; } alias;
    const s8* addr = &sfield->value.j;
    alias.lval = dvmQuasiAtomicRead64(addr);
    ANDROID_MEMBAR_FULL();
    return alias.dval;
}
INLINE Object* dvmGetStaticFieldObjectVolatile(const StaticField* sfield) {
    Object* const* ptr = &(sfield->value.l);
    return (Object*)android_atomic_acquire_load((int32_t*)ptr);
}

#ifdef WITH_TAINT_TRACKING
INLINE u4 dvmGetStaticFieldTaint(const StaticField* sfield) {
    return sfield->taint.tag;
}
INLINE u4 dvmGetStaticFieldTaintVolatile(const StaticField* sfield) {
    const u4* ptr = &(sfield->taint.tag);
    return (u4)android_atomic_acquire_load((s4*)ptr);
}
#define dvmGetStaticFieldTaintBoolean(_sfield) dvmGetStaticFieldTaint(_sfield)
#define dvmGetStaticFieldTaintByte(_sfield)    dvmGetStaticFieldTaint(_sfield)
#define dvmGetStaticFieldTaintShort(_sfield)   dvmGetStaticFieldTaint(_sfield)
#define dvmGetStaticFieldTaintChar(_sfield)    dvmGetStaticFieldTaint(_sfield)
#define dvmGetStaticFieldTaintInt(_sfield)     dvmGetStaticFieldTaint(_sfield)
#define dvmGetStaticFieldTaintLong(_sfield)    dvmGetStaticFieldTaint(_sfield)
#define dvmGetStaticFieldTaintFloat(_sfield)   dvmGetStaticFieldTaint(_sfield)
#define dvmGetStaticFieldTaintDouble(_sfield)  dvmGetStaticFieldTaint(_sfield)
#define dvmGetStaticFieldTaintObject(_sfield)  dvmGetStaticFieldTaint(_sfield)

#define dvmGetStaticFieldTaintBooleanVolatile(_sfield) dvmGetStaticFieldTaintVolatile(_sfield)
#define dvmGetStaticFieldTaintByteVolatile(_sfield)    dvmGetStaticFieldTaintVolatile(_sfield)
#define dvmGetStaticFieldTaintShortVolatile(_sfield)   dvmGetStaticFieldTaintVolatile(_sfield)
#define dvmGetStaticFieldTaintCharVolatile(_sfield)    dvmGetStaticFieldTaintVolatile(_sfield)
#define dvmGetStaticFieldTaintIntVolatile(_sfield)     dvmGetStaticFieldTaintVolatile(_sfield)
#define dvmGetStaticFieldTaintLongVolatile(_sfield)    dvmGetStaticFieldTaintVolatile(_sfield)
#define dvmGetStaticFieldTaintFloatVolatile(_sfield)   dvmGetStaticFieldTaintVolatile(_sfield)
#define dvmGetStaticFieldTaintDoubleVolatile(_sfield)  dvmGetStaticFieldTaintVolatile(_sfield)
#define dvmGetStaticFieldTaintObjectVolatile(_sfield)  dvmGetStaticFieldTaintVolatile(_sfield)
#else
#define dvmGetStaticFieldTaint(_sfield)        ((void)0)
#define dvmGetStaticFieldTaintBoolean(_sfield) ((void)0)
#define dvmGetStaticFieldTaintByte(_sfield)    ((void)0)
#define dvmGetStaticFieldTaintShort(_sfield)   ((void)0)
#define dvmGetStaticFieldTaintChar(_sfield)    ((void)0)
#define dvmGetStaticFieldTaintInt(_sfield)     ((void)0)
#define dvmGetStaticFieldTaintLong(_sfield)    ((void)0)
#define dvmGetStaticFieldTaintFloat(_sfield)   ((void)0)
#define dvmGetStaticFieldTaintDouble(_sfield)  ((void)0)
#define dvmGetStaticFieldTaintObject(_sfield)  ((void)0)

#define dvmGetStaticFieldTaintBooleanVolatile(_sfield) ((void)0)
#define dvmGetStaticFieldTaintByteVolatile(_sfield)    ((void)0)
#define dvmGetStaticFieldTaintShortVolatile(_sfield)   ((void)0)
#define dvmGetStaticFieldTaintCharVolatile(_sfield)    ((void)0)
#define dvmGetStaticFieldTaintIntVolatile(_sfield)     ((void)0)
#define dvmGetStaticFieldTaintLongVolatile(_sfield)    ((void)0)
#define dvmGetStaticFieldTaintFloatVolatile(_sfield)   ((void)0)
#define dvmGetStaticFieldTaintDoubleVolatile(_sfield)  ((void)0)
#define dvmGetStaticFieldTaintObjectVolatile(_sfield)  ((void)0)
#endif

INLINE void dvmSetStaticFieldBoolean(StaticField* sfield, bool val) {
    sfield->value.i = val;
}
INLINE void dvmSetStaticFieldByte(StaticField* sfield, s1 val) {
    sfield->value.i = val;
}
INLINE void dvmSetStaticFieldShort(StaticField* sfield, s2 val) {
    sfield->value.i = val;
}
INLINE void dvmSetStaticFieldChar(StaticField* sfield, u2 val) {
    sfield->value.i = val;
}
INLINE void dvmSetStaticFieldInt(StaticField* sfield, s4 val) {
    sfield->value.i = val;
}
INLINE void dvmSetStaticFieldFloat(StaticField* sfield, float val) {
    sfield->value.f = val;
}
INLINE void dvmSetStaticFieldLong(StaticField* sfield, s8 val) {
    sfield->value.j = val;
}
INLINE void dvmSetStaticFieldDouble(StaticField* sfield, double val) {
    sfield->value.d = val;
}
INLINE void dvmSetStaticFieldObject(StaticField* sfield, Object* val) {
    sfield->value.l = val;
    if (val != NULL) {
        dvmWriteBarrierField(sfield->clazz, &sfield->value.l);
    }
}
INLINE void dvmSetStaticFieldIntVolatile(StaticField* sfield, s4 val) {
    s4* ptr = &sfield->value.i;
    ANDROID_MEMBAR_STORE();
    *ptr = val;
    ANDROID_MEMBAR_FULL();
}
INLINE void dvmSetStaticFieldBooleanVolatile(StaticField* sfield, bool val) {
    dvmSetStaticFieldIntVolatile(sfield, val);
}
INLINE void dvmSetStaticFieldByteVolatile(StaticField* sfield, s1 val) {
    dvmSetStaticFieldIntVolatile(sfield, val);
}
INLINE void dvmSetStaticFieldShortVolatile(StaticField* sfield, s2 val) {
    dvmSetStaticFieldIntVolatile(sfield, val);
}
INLINE void dvmSetStaticFieldCharVolatile(StaticField* sfield, u2 val) {
    dvmSetStaticFieldIntVolatile(sfield, val);
}
INLINE void dvmSetStaticFieldFloatVolatile(StaticField* sfield, float val) {
    union { s4 ival; float fval; } alias;
    alias.fval = val;
    dvmSetStaticFieldIntVolatile(sfield, alias.ival);
}
INLINE void dvmSetStaticFieldLongVolatile(StaticField* sfield, s8 val) {
    s8* addr = &sfield->value.j;
    dvmQuasiAtomicSwap64Sync(val, addr);
}
INLINE void dvmSetStaticFieldDoubleVolatile(StaticField* sfield, double val) {
    union { s8 lval; double dval; } alias;
    alias.dval = val;
    dvmSetStaticFieldLongVolatile(sfield, alias.lval);
}
INLINE void dvmSetStaticFieldObjectVolatile(StaticField* sfield, Object* val) {
    Object** ptr = &(sfield->value.l);
    ANDROID_MEMBAR_STORE();
    *ptr = val;
    ANDROID_MEMBAR_FULL();
    if (val != NULL) {
        dvmWriteBarrierField(sfield->clazz, &sfield->value.l);
    }
}

#ifdef WITH_TAINT_TRACKING
INLINE void dvmSetStaticFieldTaint(StaticField* sfield, u4 tag) {
    sfield->taint.tag = tag;
}
INLINE void dvmSetStaticFieldTaintVolatile(StaticField* sfield, u4 tag) {
    u4* ptr = &sfield->taint.tag;
    android_atomic_release_store(tag, (s4*)ptr);
}
#define dvmSetStaticFieldTaintBoolean(_sfield, _tag) dvmSetStaticFieldTaint(_sfield, _tag)
#define dvmSetStaticFieldTaintByte(_sfield, _tag)    dvmSetStaticFieldTaint(_sfield, _tag)
#define dvmSetStaticFieldTaintShort(_sfield, _tag)   dvmSetStaticFieldTaint(_sfield, _tag)
#define dvmSetStaticFieldTaintChar(_sfield, _tag)    dvmSetStaticFieldTaint(_sfield, _tag)
#define dvmSetStaticFieldTaintInt(_sfield, _tag)     dvmSetStaticFieldTaint(_sfield, _tag)
#define dvmSetStaticFieldTaintLong(_sfield, _tag)    dvmSetStaticFieldTaint(_sfield, _tag)
#define dvmSetStaticFieldTaintFloat(_sfield, _tag)   dvmSetStaticFieldTaint(_sfield, _tag)
#define dvmSetStaticFieldTaintDouble(_sfield, _tag)  dvmSetStaticFieldTaint(_sfield, _tag)
#define dvmSetStaticFieldTaintObject(_sfield, _tag)  dvmSetStaticFieldTaint(_sfield, _tag)

#define dvmSetStaticFieldTaintBooleanVolatile(_sfield, _tag) dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#define dvmSetStaticFieldTaintByteVolatile(_sfield, _tag)    dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#define dvmSetStaticFieldTaintShortVolatile(_sfield, _tag)   dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#define dvmSetStaticFieldTaintCharVolatile(_sfield, _tag)    dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#define dvmSetStaticFieldTaintIntVolatile(_sfield, _tag)     dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#define dvmSetStaticFieldTaintLongVolatile(_sfield, _tag)    dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#define dvmSetStaticFieldTaintFloatVolatile(_sfield, _tag)   dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#define dvmSetStaticFieldTaintDoubleVolatile(_sfield, _tag)  dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#define dvmSetStaticFieldTaintObjectVolatile(_sfield, _tag)  dvmSetStaticFieldTaintVolatile(_sfield, _tag)
#else
#define dvmSetStaticFieldTaint(_sfield, _tag)        ((void)0)
#define dvmSetStaticFieldTaintBoolean(_sfield, _tag) ((void)0)
#define dvmSetStaticFieldTaintByte(_sfield, _tag)    ((void)0)
#define dvmSetStaticFieldTaintShort(_sfield, _tag)   ((void)0)
#define dvmSetStaticFieldTaintChar(_sfield, _tag)    ((void)0)
#define dvmSetStaticFieldTaintInt(_sfield, _tag)     ((void)0)
#define dvmSetStaticFieldTaintLong(_sfield, _tag)    ((void)0)
#define dvmSetStaticFieldTaintFloat(_sfield, _tag)   ((void)0)
#define dvmSetStaticFieldTaintDouble(_sfield, _tag)  ((void)0)
#define dvmSetStaticFieldTaintObject(_sfield, _tag)  ((void)0)
#endif

#endif  // DALVIK_OO_OBJECTINLINES_H_
