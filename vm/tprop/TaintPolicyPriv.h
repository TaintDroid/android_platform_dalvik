
#ifndef _DALVIK_TPROP_TAINT_PROP_PRIV
#define _DALVIK_TPROP_TAINT_PROP_PRIV

#include "Dalvik.h"

typedef enum {
    kTaintProfileUnknown = 0,
    kTaintProfileClass,
    kTaintProfileParam,
    kTaintProfileReturn
} TaintProfileEntryType;

typedef struct {
    const char* from;
    const char* to;
} TaintProfileEntry;

#define TAINT_PROFILE_TABLE_SIZE 8 /* per class */
#define TAINT_POLICY_TABLE_SIZE 32 /* number of classes */

typedef struct {
    const char* methodName;
    const TaintProfileEntry* entries;
} TaintProfile;

typedef struct {
    const char* classDescriptor;
    const TaintProfile* profiles;
    HashTable* methodTable; /* created on startup */
} TaintPolicy;

extern TaintPolicy gDvmJniTaintPolicy[];

/* function of type HashCompareFunc */
static int hashcmpTaintPolicy(const void* ptr1, const void* ptr2)
{
    TaintPolicy* p1 = (TaintPolicy*) ptr1;
    TaintPolicy* p2 = (TaintPolicy*) ptr2;

    return strcmp(p1->classDescriptor, p2->classDescriptor);
}

/* function of type HashCompareFunc */
static int hashcmpTaintProfile(const void* ptr1, const void* ptr2)
{
    TaintProfile* m1 = (TaintProfile*) ptr1;
    TaintProfile* m2 = (TaintProfile*) ptr2;

    return strcmp(m1->methodName, m2->methodName);
}

/* function of type HashFreeFunc */
static void freeTaintPolicy(void* p)
{
    TaintPolicy* pol = (TaintPolicy*) p;
    if (pol != NULL) {
	dvmHashTableFree(pol->methodTable);
    }
}

/* function of type HashFreeFunc */
static void freeTaintProfile(void* p)
{
    /* nothing to free */
    return;
}

#endif
