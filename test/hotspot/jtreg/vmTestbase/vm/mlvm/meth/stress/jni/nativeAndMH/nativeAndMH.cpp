/*
 * Copyright (c) 2011, 2018, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jni.h"
#include "jni_tools.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARGS_COUNT 6

JNIEXPORT jobject JNICALL
Java_vm_mlvm_meth_stress_jni_nativeAndMH_Test_native01(
        JNIEnv * pEnv, jclass clazz,
        jstring a1,
        jobject a2,
        jobject a3,
        jobject a4,
        jobject a5,
        jobject a6,
        jobject mhToCall)
{
    jclass mhClass;
    jmethodID mid;
    jclass objectClass;
    jobjectArray arguments;
    jobject result;

    if ( ! NSK_JNI_VERIFY(pEnv, (mhClass = NSK_CPP_STUB2(GetObjectClass, pEnv, mhToCall)) != NULL) )
        return NULL;

    if ( ! NSK_JNI_VERIFY(pEnv, NULL != (mid = NSK_CPP_STUB4(GetMethodID, pEnv, mhClass,
            "invokeWithArguments",
            "([Ljava/lang/Object;)Ljava/lang/Object;"))) )
        return NULL;

    NSK_JNI_VERIFY(pEnv, NULL != (objectClass = NSK_CPP_STUB2(FindClass, pEnv, "java/lang/Object")));

    NSK_JNI_VERIFY(pEnv, NULL != (arguments = NSK_CPP_STUB4(NewObjectArray, pEnv, ARGS_COUNT, objectClass, NULL)));

    NSK_JNI_VERIFY_VOID(pEnv, NSK_CPP_STUB4(SetObjectArrayElement, pEnv, arguments, 0, a1));
    NSK_JNI_VERIFY_VOID(pEnv, NSK_CPP_STUB4(SetObjectArrayElement, pEnv, arguments, 1, a2));
    NSK_JNI_VERIFY_VOID(pEnv, NSK_CPP_STUB4(SetObjectArrayElement, pEnv, arguments, 2, a3));
    NSK_JNI_VERIFY_VOID(pEnv, NSK_CPP_STUB4(SetObjectArrayElement, pEnv, arguments, 3, a4));
    NSK_JNI_VERIFY_VOID(pEnv, NSK_CPP_STUB4(SetObjectArrayElement, pEnv, arguments, 4, a5));
    NSK_JNI_VERIFY_VOID(pEnv, NSK_CPP_STUB4(SetObjectArrayElement, pEnv, arguments, 5, a6));

    // Swap arguments
    NSK_JNI_VERIFY(pEnv, NULL != (result = NSK_CPP_STUB4(CallObjectMethod, pEnv, mhToCall, mid, arguments)));
    return result;
}

#ifdef __cplusplus
}
#endif
