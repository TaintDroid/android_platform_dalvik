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

LOCAL_PATH := $(my-dir)

ifeq ($(WITH_TAINT_TRACKING),true)
    # Add ftaint only for "eng"
    include $(CLEAR_VARS)
    LOCAL_MODULE := ftaint
    #LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_TAGS := eng
    LOCAL_SRC_FILES := ftaint.c
    LOCAL_STATIC_LIBRARIES := libattr
    LOCAL_C_INCLUDES := dalvik/libattr
    include $(BUILD_EXECUTABLE)
endif

