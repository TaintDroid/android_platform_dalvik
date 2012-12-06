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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "attr/xattr.h"

#define TAINT_XATTR_NAME "user.taint"

#define USAGE "Usage: %s [g|s|a] <file> [<hex>]\n"
#define TAINT_CLEAR 0x0

typedef unsigned int u4;

static u4 getTaintXattr(const char *path)
{
    int ret;
    u4 buf;
    u4 tag = TAINT_CLEAR;

    ret = getxattr(path, TAINT_XATTR_NAME, &buf, sizeof(buf)); 
    if (ret > 0) {
        tag = buf;
    } else {
        if (errno == ENOATTR) {
            fprintf(stdout, "getxattr(%s): no taint tag\n", path);
        } else if (errno == ERANGE) {
            fprintf(stderr, "Error: getxattr(%s) contents to large\n", path);
        } else if (errno == ENOTSUP) {
            fprintf(stderr, "Error: getxattr(%s) not supported\n", path);
        } else {
            fprintf(stderr, "Errro: getxattr(%s): unknown error code %d\n", path, errno);
        }
    }

    return tag;
}

static void setTaintXattr(const char *path, u4 tag)
{
    int ret;

    ret = setxattr(path, TAINT_XATTR_NAME, &tag, sizeof(tag), 0);

    if (ret < 0) {
        if (errno == ENOSPC || errno == EDQUOT) {
            fprintf(stderr, "Error: setxattr(%s): not enough room to set xattr\n", path);
        } else if (errno == ENOTSUP) {
            fprintf(stderr, "Error: setxattr(%s) not supported\n", path);
        } else {
            fprintf(stderr, "Errro: setxattr(%s): unknown error code %d\n", path, errno);
        }
    }
}

void usage(const char *prog)
{
    fprintf(stderr, USAGE, prog);
    exit(1);
}

int main(int argc, char *argv[])
{
    u4 tag;

    if (argc != 3 && argc != 4) {
        usage(argv[0]);
    }
    
    if (strlen(argv[1]) != 1) {
        usage(argv[0]);
    }

    // Get the taint
    if (argc == 3) {
        if (argv[1][0] == 'g') {
            tag = getTaintXattr(argv[2]);
            fprintf(stdout, "0x%08x\n", tag);
            return 0;
        } else {
            usage(argv[0]);
        }
    }

    // Set the taint
    tag = strtol(argv[3], NULL, 16);
    if (tag == 0 && errno == ERANGE) {
        usage(argv[0]);
    }

    if (argv[1][0] == 's') {
        setTaintXattr(argv[2], tag);
    } else if (argv[1][0] == 'a') {
        u4 old = getTaintXattr(argv[2]);
        setTaintXattr(argv[2], tag | old);
    } else {
        usage(argv[0]);
    }

    return 0;
}
