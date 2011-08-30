
#include "Dalvik.h"
#include "tprop/TaintPolicyPriv.h"

/* Wrapper to bundle a Field and an Object instance 
 * - needed when dealing with nested instance field entries
 */
typedef struct {
    Field *field;
    Object *obj;
} FieldRef;

HashTable *gPolicyTable = NULL;

#ifdef TAINT_JNI_LOG
/* JNI logging for debugging purposes 
 * -- used to only print methods once (quites things down a bit)
 */
HashTable *gJniLogSeen = NULL;
bool gJniLog = true;
#endif

/* Code called from dvmJniStartup()
 * Initializes the gPolicyTable for fast lookup of taint policy 
 * profiles for methods.
 */
void dvmTaintPropJniStartup()
{
    TaintPolicy* policy;
    u4 hash;
    
    /* Create the policy table (perfect size) */
    gPolicyTable = dvmHashTableCreate(
	    dvmHashSize(TAINT_POLICY_TABLE_SIZE), 
	    freeTaintPolicy);

    for (policy = gDvmJniTaintPolicy; policy->classDescriptor != NULL; policy++) {
	TaintProfile *profile;
    
	/* Create the method table for this class */
	policy->methodTable = dvmHashTableCreate(
		TAINT_PROFILE_TABLE_SIZE, freeTaintProfile);

	/* Add all of the methods */
	for (profile = &policy->profiles[0]; profile->methodName != NULL; profile++) {
	    hash = dvmComputeUtf8Hash(profile->methodName);
	    dvmHashTableLookup(policy->methodTable, hash, profile,
		    hashcmpTaintProfile, true);
	}

	/* Add this class to gPolicyTable */
	hash = dvmComputeUtf8Hash(policy->classDescriptor);
	dvmHashTableLookup(gPolicyTable, hash, policy, 
		hashcmpTaintPolicy, true);
    }

#ifdef TAINT_JNI_LOG
    /* JNI logging for debugging purposes */
    gJniLogSeen = dvmHashTableCreate(dvmHashSize(50), free);
#endif
}

/* Code called from dvmJniShutdown()
 * deallocates the gPolicyTable
 */
void dvmTaintPropJniShutdown()
{
    dvmHashTableFree(gPolicyTable);
#ifdef TAINT_JNI_LOG
    /* JNI logging for debugging purposes */
    dvmHashTableFree(gJniLogSeen);
#endif
}

/* Returns the taint on an object.
 * - Currently only arrays and java.lang.String is supported
 */
u4 getObjectTaint(Object* obj, const char* descriptor)
{
    ArrayObject *arrObj = NULL;

    if (obj == NULL) {
	return TAINT_CLEAR;
    }

    if (descriptor[0] == '[') {
	/* Get the taint from the array */
	arrObj = (ArrayObject*) obj;
	if (arrObj != NULL) {
	    return arrObj->taint.tag;
	}
    } 
    
    if (strcmp(descriptor, "Ljava/lang/String;") == 0) {
	arrObj = (ArrayObject*) dvmGetFieldObject(obj, 
		gDvm.offJavaLangString_value);
	if (arrObj != NULL) {
	    return arrObj->taint.tag;
	} /* else, empty string? don't worry about it */
    } 

    /* TODO: What about classes derived from String? */

    /* Don't worry about other object types */
    return TAINT_CLEAR;
}

/* Adds taint to a known object.
 * - Currently only arrays and java.lang.String is supported
 */
void addObjectTaint(Object* obj, const char* descriptor, u4 tag)
{
    ArrayObject *arrObj = NULL;

    if (obj == NULL) {
	return;
    }

    if (descriptor[0] == '[') {
	/* Get the taint from the array */
	arrObj = (ArrayObject*) obj;
	if (arrObj != NULL) {
	    arrObj->taint.tag |= tag;
	}
    } 
    
    if (strcmp(descriptor, "Ljava/lang/String;") == 0) {
	arrObj = (ArrayObject*) dvmGetFieldObject(obj, 
		gDvm.offJavaLangString_value);
	if (arrObj != NULL) {
	    arrObj->taint.tag |= tag;
	} /* else, empty string? don't worry about it */
    } 

    /* TODO: What about classes derived from String? */

    /* Don't worry about other object types */
    return;
}

/* Sets the taint on the return value
 * - rtaint points to an address in the args array
 * - descriptor is the return type
 * - for return objects, only arrays and java.lang.String supported
 *   (will taint object reference returned otherwise)
 */
void setReturnTaint(u4 tag, u4* rtaint, JValue* pResult, 
	const char* descriptor)
{
    Object* obj = NULL;
    ArrayObject* arrObj = NULL;

    switch (descriptor[0]) {
	case 'V':
	    /* void, do nothing */
	    break;
	case 'Z':
	case 'B':
	case 'C':
	case 'S':
	case 'I':
	case 'J':
	case 'F':
	case 'D':
	    /* Easy case */
	    *rtaint |= tag;
	    break;
	case '[':
	    /* Best we can do is taint the array, however
	     * this is not right for "[[" or "[L" */
	    arrObj = (ArrayObject*) pResult->l;
	    if (arrObj != NULL) {
		arrObj->taint.tag |= tag;
	    } /* else, method returning null pointer */
	    break;
	case 'L':
	    obj = (Object*) pResult->l;

	    if (obj != NULL) {
		if (strcmp(descriptor, "Ljava/lang/String;") == 0) {
		    arrObj = (ArrayObject*)dvmGetFieldObject(obj, 
			    gDvm.offJavaLangString_value);
		    if (arrObj != NULL) {
			arrObj->taint.tag |= tag;
		    } /* else, empty string?, don't worry about it */
		} else {
		    /* TODO: What about classes derived from String? */
		    /* Best we can do is to taint the object ref */
		    *rtaint |= tag;
		}
	    }
	    break;
    }
}

/* Returns the TaintPolicyProfile associated with this method
 * - returns NULL if not found
 */
TaintProfile* getPolicyProfile(const Method* method)
{
    TaintPolicy* policy = NULL;
    TaintProfile* profile = NULL;
    u4 hash;

    /* temporary variables for the search */
    TaintPolicy tPol = {NULL, NULL, NULL};
    TaintProfile tProf = {NULL, NULL};

    dvmHashTableLock(gPolicyTable);

    /* Find the class */
    hash = dvmComputeUtf8Hash(method->clazz->descriptor);
    tPol.classDescriptor = method->clazz->descriptor;
    policy = (TaintPolicy*) dvmHashTableLookup(gPolicyTable,
	    hash, &tPol, hashcmpTaintPolicy, false);

    if (policy != NULL) {
	dvmHashTableLock(policy->methodTable);

	/* Find the Method */
	hash = dvmComputeUtf8Hash(method->name);
	tProf.methodName = method->name;
	profile = (TaintProfile*) dvmHashTableLookup(policy->methodTable,
		hash, &tProf, hashcmpTaintProfile, false);

	dvmHashTableUnlock(policy->methodTable);
    }

    dvmHashTableUnlock(gPolicyTable);

    return profile;
}


/* utility to determine the type of entry string
 */
TaintProfileEntryType getEntryType(const char* entry)
{
    TaintProfileEntryType type = kTaintProfileUnknown;

    if (strncmp(entry, "class", 5) == 0) {
	type = kTaintProfileClass;
    } else if (strncmp(entry, "param", 5) == 0) {
	type = kTaintProfileParam;
    } else if (strncmp(entry, "return", 6) == 0) {
	type = kTaintProfileReturn;
    }

    return type;
}

/* returns the field structure corresponding to the profile entry
 * variable in the form: <signature> % <name> 
 */
Field* getFieldFromProfileVar(const char* var, const ClassObject* clazz)
{
    char* pos;
    char* sig;
    char* name;
    InstField* ifield;
    StaticField* sfield;
    Field* field = NULL;

    // TODO: Can we avoid the allocation if we replace the '%'?
    // Need to make sure there isn't a locking problem if that route
    // is taken.
    pos = index(var, '%');
    sig = strndup(var, pos-var);
    name = strdup(pos+1);

    /* Try both static and instance fields */
    ifield = dvmFindInstanceFieldHier(clazz, name, sig);
    if (ifield != NULL) {
	field = (Field*) ifield;
    } else {
	sfield = dvmFindStaticFieldHier(clazz, name, sig);
	if (sfield != NULL) {
	    field = (Field*) sfield;
	}
    }

    free(sig);
    free(name);

    return field;
}

/* Returns the taint tag for a field
 * - obj only used if the field is not static
 */
u4 getTaintFromField(Field* field, Object* obj)
{
    u4 tag = TAINT_CLEAR;

    if (dvmIsStaticField(field)) {
	StaticField* sfield = (StaticField*) field;
	tag = dvmGetStaticFieldTaint(sfield);
    } else {
	InstField* ifield = (InstField*) field;
	if (field->signature[0] == 'J' || field->signature[0] == 'D') {
	    tag = dvmGetFieldTaintWide(obj, ifield->byteOffset);
	} else {
	    tag = dvmGetFieldTaint(obj, ifield->byteOffset);
	}
    }

    return tag;
}

/* add tag to a field
 * - obj only used if the field is not static
 */
void addTaintToField(Field* field, Object* obj, u4 tag)
{
    if (dvmIsStaticField(field)) {
	StaticField* sfield = (StaticField*) field;
	tag |= dvmGetStaticFieldTaint(sfield);
	dvmSetStaticFieldTaint(sfield, tag);
    } else {
	InstField* ifield = (InstField*) field;
	if (field->signature[0] == 'J' || field->signature[0] == 'D') {
	    tag |= dvmGetFieldTaintWide(obj, ifield->byteOffset);
	    dvmSetFieldTaintWide(obj, ifield->byteOffset, tag);
	} else {
	    tag |= dvmGetFieldTaint(obj, ifield->byteOffset);
	    dvmSetFieldTaint(obj, ifield->byteOffset, tag);
	}
    }
}

/* Returns the object pointer for a field
 * - obj only used if the field is not static
 * - Note: will not return an array object
 */
Object* getObjectFromField(Field* field, Object* obj)
{
    if (field->signature[0] != 'L') {
	return NULL;
    }

    if (dvmIsStaticField(field)) {
	StaticField* sfield = (StaticField*) field;
	return dvmGetStaticFieldObject(sfield);
    } else {
	InstField* ifield = (InstField*) field;
	return dvmGetFieldObject(obj, ifield->byteOffset);
    }
}

/* Gets the field associated with an entry string that has the
 * "class.", "argX.", or "return." stripped
 * - recursively finds the end field
 * - obj is ignored if field is static
 */
FieldRef getFieldFromEntry(const char* entry, ClassObject* clazz, Object* obj)
{
    FieldRef fRef;
    char* split;

    memset(&fRef, 0, sizeof(fRef));

    split = index(entry, '.');
    if (split == NULL) { /* This is the last part */
	fRef.field = getFieldFromProfileVar(entry, clazz);
	fRef.obj = obj;
	if (fRef.field == NULL) {
	    LOGW("TaintPolicy: variable doesn't exist: %s", entry);
	}

    } else if (entry[0] != 'L') {
	/* recursion is required, but target isn't an Object */
	LOGW("TaintPolicy: expected object variable: %s", entry);

    } else { /* recursion is required */
	char* var = strndup(entry, split-entry);
	fRef.field = getFieldFromProfileVar(var, clazz);
	fRef.obj = obj;
	free(var);

	if (fRef.field == NULL) {
	    LOGW("TaintPolicy: variable doesn't exist: %s", entry);
	} else {
	    Object* obj2 = getObjectFromField(fRef.field, fRef.obj);
	    if (obj2 != NULL) { 
		/* recurse */
		fRef = getFieldFromEntry(split+1, fRef.field->clazz, obj2);
	    } else {
		LOGW("TaintPolicy: error getting object for %s", entry);
	    }
	}
    }

    return fRef;
}

/* Gets the taint tag associated with a field.
 *  - Here, we assume any "class." has been stripped.
 *  - We recursively dereference the field name if multiple levels
 *  - obj is ignored if field is static
 */
u4 getFieldEntryTaint(const char* entry, ClassObject* clazz, Object* obj)
{
    u4 tag = TAINT_CLEAR;
    FieldRef fRef;

    fRef = getFieldFromEntry(entry, clazz, obj);
    if (fRef.field != NULL) {
	tag = getTaintFromField(fRef.field, fRef.obj);
    }

    return tag;
}

/* Returns the index in args[] corresponding to the parameter
 * string entry.
 * - It doesn't matter if the entry has multiple parts, e.g.,
 *   "param1.foo.bar", as long as the variable name after the first
 *   "." is not a number. Since the signature comes next, we can 
 *   safely assume this is the case.
 * - returns -1 on parsing error
 * - If descriptor is not NULL, it will point to a newly allocated 
 *   descriptor that needs to be free()'d (unless there was an error)
 */
int paramToArgIndex(const char* entry, const Method* method, char** descriptor) {
    int pIdx, aIdx, i;
    char* endptr;
    const char* num = entry + 5; /* "param" is the first 5 characters */
    const DexProto* proto = &method->prototype;
    DexParameterIterator pIterator;

    /* Step 1: determine the parameter index (pIdx) */
    pIdx = strtol(num, &endptr, 10);
    if (num == endptr) {
	/* error parsing */
	return -1;
    } 

    /* Step 2: translate parameter index into args array index */
    dexParameterIteratorInit(&pIterator, proto);
    aIdx = (dvmIsStaticMethod(method)?0:1); /* index where params start */
    for (i=0; i<=pIdx ; i++) {
	const char* desc = dexParameterIteratorNextDescriptor(&pIterator);

	if (desc == NULL) {
	    /* This index doesn't exist, error */
	    return -1;
	} 

	if (i == pIdx) {
	    /* This is the index */
	    if (descriptor != NULL) {
		*descriptor = strdup(desc);
	    }
	    break;
	}

	/* increment the args array index */
	aIdx++;

	if (desc[0] == 'J' || desc[0] == 'D') {
	    /* wide argument, increment index one more */
	    aIdx++;
	}
    }

    return aIdx;
}

u4 getParamEntryTaint(const char* entry, const u4* args, const Method* method)
{
    u4 tag = TAINT_CLEAR;
    int aIdx;
    char* pos;
    char* pDesc = NULL; /* parameter descriptor */

    /* Determine corresponding args[] index */
    aIdx = paramToArgIndex(entry, method, &pDesc);
    if (aIdx == -1) {
	LOGW("TaintPolicy: error parsing %s", entry);
	return tag;
    }

    /* Determine if entry requres field search */
    pos = index(entry, '.');
    if (pos == NULL ) { /* just need parameter taint */
	switch (pDesc[0]) {
	    case 'Z':
	    case 'B':
	    case 'C':
	    case 'S':
	    case 'I':
	    case 'J':
	    case 'F':
	    case 'D':
		/* assume args array length (insSize) = 3
		 * and aIdx = 1 (second index)
		 * 0 1 2 [3] 4 5 6
		 *	       ^-- the taint value we want
		 */
		tag = args[aIdx+method->insSize+1];
		break;
	    case '[':
	    case 'L':
		/* use both the object reference taint and Object taint */
		tag = args[aIdx+method->insSize+1];
		tag |= getObjectTaint((Object*)args[aIdx], pDesc);
		break;
	    default:
		LOGW("TaintPolicy: unknown parameter type for %s", entry);
	}

    } else { /* need to get the parameter object for field search */
	if (pDesc[0] != 'L') {
	    LOGW("TaintPolicy: param not object in %s", entry);
	} else {
	    Object* obj = (Object*)args[aIdx];
	    tag = getFieldEntryTaint(pos+1, obj->clazz, obj);
	}
    }

    if (pDesc) {
	free(pDesc);
    }
    
    return tag;
}

u4 getEntryTaint(const char* entry, const u4* args, const Method* method)
{
    u4 tag = TAINT_CLEAR;
    char *pos;

    /* Determine split point if any */
    pos = index(entry, '.');

    switch (getEntryType(entry)) {
	case kTaintProfileClass:
	    if (dvmIsStaticMethod(method)) {
		tag = getFieldEntryTaint(pos+1, method->clazz, NULL);
	    } else {
		tag = getFieldEntryTaint(pos+1, method->clazz, (Object*)args[0]);
	    }
	    break;

	case kTaintProfileParam:
	    tag = getParamEntryTaint(entry, args, method);
	    break;

	default:
	    LOGW("TaintPolicy: Invalid from type: [%s]", entry);
    }
    
    return tag;
}

/* adds the taint tag associated with a field.
 *  - Here, we assume any "class." has been stripped.
 *  - We recursively dereference the field name if multiple levels
 *  - obj is ignored if field is static
 */
void addFieldEntryTaint(u4 tag, const char* entry, ClassObject* clazz, Object* obj)
{
    FieldRef field;

    field = getFieldFromEntry(entry, clazz, obj);
    if (field.field != NULL) {
	addTaintToField(field.field, field.obj, tag);
    }
}

void addParamEntryTaint(u4 tag, const char* entry, const u4* args, const Method* method)
{
    int aIdx;
    char* pos;
    char* pDesc = NULL; /* parameter descriptor */
    
    pos = index(entry, '.');

    /* Determine corresponding args[] index */
    aIdx = paramToArgIndex(entry, method, &pDesc);
    if (aIdx == -1) {
	LOGW("TaintPolicy: error parsing %s", entry);
	return;
    }

    if (pos == NULL && (pDesc[0] == '[' || pDesc[0] == 'L')) {
	/* target is a parameter that we can taint directly */
	Object* obj = (Object*)args[aIdx];
	addObjectTaint(obj, pDesc, tag);
    } else if (pDesc[0] == 'L') {
	Object* obj = (Object*)args[aIdx];
	addFieldEntryTaint(tag, pos+1, obj->clazz, obj);
    } else {
	LOGW("TaintPolicy: param not object or array in %s (%s)", 
		entry, pDesc);
    }

    if (pDesc) {
	free(pDesc);
    }
}

u4 addEntryTaint(u4 tag, const char* entry, const u4* args, const Method* method)
{
    u4 rtaint = TAINT_CLEAR;

    switch (getEntryType(entry)) {
	case kTaintProfileClass:
	    if (dvmIsStaticMethod(method)) {
		addFieldEntryTaint(tag, entry, method->clazz, NULL);
	    } else {
		addFieldEntryTaint(tag, entry, method->clazz, (Object*)args[0]);
	    }
	    break;

	case kTaintProfileParam:
	    addParamEntryTaint(tag, entry, args, method);
	    break;

	case kTaintProfileReturn:
	    if (entry[7] == '\0') { /* taint the return itself */
		rtaint = tag;
	    } else {
		// TODO: implement return field tainting (need pResult)
		LOGW("TaintPolicy: tainting return fields not supported");
	    }
	    break;

	default:
	    LOGW("TaintPolicy: Invalid from type: [%s]", entry);
    }

    return rtaint;
}

/* Returns a taint if the profile policy indicates propagation
 * to the return
 */
u4 propMethodProfile(const u4* args, const Method* method)
{
    u4 rtaint = TAINT_CLEAR;
    TaintProfile* profile = NULL;
    TaintProfileEntry* entry = NULL;

    profile = getPolicyProfile(method);
    if (profile == NULL) {
	return rtaint;
    }

    //LOGD("TaintPolicy: applying policy for %s.%s",
    //	    method->clazz->descriptor, method->name);

    /* Cycle through the profile entries */
    for (entry = &profile->entries[0]; entry->from != NULL; entry++) {
	u4 tag = TAINT_CLEAR;

	tag = getEntryTaint(entry->from, args, method);
	if (tag) {
	    //LOGD("TaintPolicy: tag = %d %s -> %s",
	    //	    tag, entry->from, entry->to);
	    rtaint |= addEntryTaint(tag, entry->to, args, method);
	}

    }

    return rtaint;
}

/* Used to propagate taint for JNI methods
 * Two types of propagation:
 *  1) simple conservative propagation based on parameters
 *  2) propagation based on function profile policies
 */
void dvmTaintPropJniMethod(const u4* args, JValue* pResult, const Method* method)
{
    const DexProto* proto = &method->prototype;
    DexParameterIterator pIterator;
    int nParams = dexProtoGetParameterCount(proto);
    int pStart = (dvmIsStaticMethod(method)?0:1); /* index where params start */

    /* Consider 3 arguments. [x] indicates return taint index
     * 0 1 2 [3] 4 5 6
     */
    int nArgs = method->insSize;
    u4* rtaint = (u4*) &args[nArgs]; /* The return taint value */
    int tStart = nArgs+1; /* index of args[] where taint values start */
    int tEnd   = nArgs*2; /* index of args[] where taint values end */
    u4	tag = TAINT_CLEAR;
    int i;

#if 0
    {
	char *desc = dexProtoCopyMethodDescriptor(proto);
	LOGW("Jni: %s.%s%s, descriptor: %s", 
		method->clazz->descriptor, method->name, 
		(dvmIsStaticMethod(method)?"[static]":"", desc)
		);
	free(desc);
    }
#endif

#ifdef TAINT_JNI_LOG
    /* JNI logging for debugging purposes */
    if (gJniLog) {
	u4 hash;
	int len;
	char *inStr, *outStr;

	len = strlen(method->clazz->descriptor) + 1 + strlen(method->name);
	inStr = malloc(len+1);
	strcpy(inStr, method->clazz->descriptor);
	strcat(inStr, ".");
	strcat(inStr, method->name);
	hash = dvmComputeUtf8Hash(inStr);

	dvmHashTableLock(gJniLogSeen);

	outStr = dvmHashTableLookup(gJniLogSeen, hash, inStr, 
		(HashCompareFunc) strcmp, false);

	if (outStr == NULL) {
	    /* New method! */
	    LOGD("JNI METHOD INVOCATION: %s\n", inStr);
	    /* add it */
	    dvmHashTableLookup(gJniLogSeen, hash, inStr, 
		(HashCompareFunc) strcmp, true);
	} else {
	    free(inStr); /* don't need this anymore */
	}

	dvmHashTableUnlock(gJniLogSeen);
    }
#endif

    /* Union the taint tags, this includes object ref tags 
     * - we don't need to worry about static vs. not static, because getting
     *	 the taint tag on the "this" object reference is a good
     * - we don't need to worry about wide registers, because the stack
     *	 interleaving of taint tags makes it transparent
     */
    for (i = tStart; i <= tEnd; i++) {
	tag |= args[i];
    }

    /* If not static, pull any taint from the "this" object */
    if (!dvmIsStaticMethod(method)) {
	tag |= getObjectTaint((Object*)args[0], method->clazz->descriptor);
    }

    /* Union taint from Objects we care about */
    dexParameterIteratorInit(&pIterator, proto);
    for (i=pStart; ; i++) {
	const char* desc = dexParameterIteratorNextDescriptor(&pIterator);

	if (desc == NULL) {
	    break;
	} 
	
	if (desc[0] == '[' || desc[0] == 'L') {
	    tag |= getObjectTaint((Object*) args[i], desc);
	}

	if (desc[0] == 'J' || desc[0] == 'D') {
	    /* wide argument, increment index one more */
	    i++;
	}
    }

    /* Look at the taint policy profiles (may have return taint) */
    tag |= propMethodProfile(args, method);

    /* Update return taint according to the return type */
    if (tag) {
	const char* desc = dexProtoGetReturnType(proto);
	setReturnTaint(tag, rtaint, pResult, desc);
    }
}

