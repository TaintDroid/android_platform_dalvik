# Copyright (c) 2010 The Pennsylvania State University
# Systems and Internet Infrastructure Security Laboratory
#
# Authors: William Enck <enck@cse.psu.edu>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


#
# Android.mk for libattr
#
# This makefile builds libattr to allow XATTR support required for
# file propagation in the TaintDroid project
#

LOCAL_PATH:= $(call my-dir)

#
# Build for the target (device).
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	attr_copy_action.c \
	attr_copy_check.c \
	attr_copy_fd.c \
	attr_copy_file.c \
	libattr.c \
	syscalls.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/attr

LOCAL_MODULE := libattr

include $(BUILD_STATIC_LIBRARY)
