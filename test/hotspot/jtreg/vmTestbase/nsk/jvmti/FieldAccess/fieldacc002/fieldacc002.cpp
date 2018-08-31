/*
 * Copyright (c) 2003, 2018, Oracle and/or its affiliates. All rights reserved.
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
#include <inttypes.h>
#include "jvmti.h"
#include "agent_common.h"
#include "JVMTITools.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JNI_ENV_ARG

#ifdef __cplusplus
#define JNI_ENV_ARG(x, y) y
#define JNI_ENV_PTR(x) x
#else
#define JNI_ENV_ARG(x,y) x, y
#define JNI_ENV_PTR(x) (*x)
#endif

#endif

#define PASSED 0
#define STATUS_FAILED 2

typedef struct {
    jfieldID fid;
    char *m_cls;
    char *m_name;
    char *m_sig;
    jlocation loc;
    char *f_cls;
    char *f_name;
    char *f_sig;
    jboolean is_static;
} writable_watch_info;

typedef struct {
    jfieldID fid;
    const char *m_cls;
    const char *m_name;
    const char *m_sig;
    jlocation loc;
    const char *f_cls;
    const char *f_name;
    const char *f_sig;
    jboolean is_static;
} watch_info;

static jvmtiEnv *jvmti;
static jvmtiEventCallbacks callbacks;
static jvmtiCapabilities caps;
static jint result = PASSED;
static jboolean printdump = JNI_FALSE;
static int eventsExpected = 0;
static int eventsCount = 0;
static watch_info watches[] = {
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticBoolean", "Z", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticByte", "B", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticShort", "S", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticInt", "I", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticLong", "J", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticFloat", "F", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticDouble", "D", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticChar", "C", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticObject", "Ljava/lang/Object;", JNI_TRUE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "staticArrInt", "[I", JNI_TRUE },

    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceBoolean", "Z", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceByte", "B", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceShort", "S", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceInt", "I", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceLong", "J", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceFloat", "F", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceDouble", "D", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceChar", "C", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceObject", "Ljava/lang/Object;", JNI_FALSE },
    { NULL, "Lnsk/jvmti/FieldAccess/fieldacc002;", "check", "(Ljava/lang/Object;)I", 0,
      "Lnsk/jvmti/FieldAccess/fieldacc002a;", "instanceArrInt", "[I", JNI_FALSE }
};

void JNICALL FieldAccess(jvmtiEnv *jvmti_env, JNIEnv *env,
        jthread thr, jmethodID method,
        jlocation location, jclass field_klass, jobject obj, jfieldID field) {
    jvmtiError err;
    jclass cls;
    writable_watch_info watch;
    char *generic;
    size_t i;

    eventsCount++;
    if (printdump == JNI_TRUE) {
        printf(">>> retrieving access watch info ...\n");
    }
    watch.fid = field;
    watch.loc = location;
    watch.is_static = (obj == NULL) ? JNI_TRUE : JNI_FALSE;
    err = jvmti_env->GetMethodDeclaringClass(method, &cls);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetMethodDeclaringClass) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        result = STATUS_FAILED;
    }
    err = jvmti_env->GetClassSignature(cls,
        &watch.m_cls, &generic);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetClassSignature) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        result = STATUS_FAILED;
    }
    err = jvmti_env->GetMethodName(method,
        &watch.m_name, &watch.m_sig, &generic);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetMethodName) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        result = STATUS_FAILED;
    }
    err = jvmti_env->GetClassSignature(field_klass,
        &watch.f_cls, &generic);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetClassSignature) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        result = STATUS_FAILED;
    }
    err = jvmti_env->GetFieldName(field_klass, field,
        &watch.f_name, &watch.f_sig, &generic);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetFieldName) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        result = STATUS_FAILED;
    }
    if (printdump == JNI_TRUE) {
        printf(">>>      class: \"%s\"\n", watch.m_cls);
        printf(">>>     method: \"%s%s\"\n", watch.m_name, watch.m_sig);
        printf(">>>   location: 0x%x%08x\n",
            (jint)(watch.loc >> 32), (jint)watch.loc);
        printf(">>>  field cls: \"%s\"\n", watch.f_cls);
        printf(">>>      field: \"%s:%s\"\n", watch.f_name, watch.f_sig);
        printf(">>>     object: 0x%p\n", obj);
    }
    for (i = 0; i < sizeof(watches)/sizeof(watch_info); i++) {
        if (watch.fid == watches[i].fid) {
            if (watch.m_cls == NULL ||
                    strcmp(watch.m_cls, watches[i].m_cls) != 0) {
                printf("(watch#%" PRIuPTR ") wrong class: \"%s\", expected: \"%s\"\n",
                       i, watch.m_cls, watches[i].m_cls);
                result = STATUS_FAILED;
            }
            if (watch.m_name == NULL ||
                    strcmp(watch.m_name, watches[i].m_name) != 0) {
                printf("(watch#%" PRIuPTR ") wrong method name: \"%s\"",
                       i, watch.m_name);
                printf(", expected: \"%s\"\n", watches[i].m_name);
                result = STATUS_FAILED;
            }
            if (watch.m_sig == NULL ||
                    strcmp(watch.m_sig, watches[i].m_sig) != 0) {
                printf("(watch#%" PRIuPTR ") wrong method sig: \"%s\"",
                       i, watch.m_sig);
                printf(", expected: \"%s\"\n", watches[i].m_sig);
                result = STATUS_FAILED;
            }
            if (watch.loc != watches[i].loc) {
                printf("(watch#%" PRIuPTR ") wrong location: 0x%x%08x",
                       i, (jint)(watch.loc >> 32), (jint)watch.loc);
                printf(", expected: 0x%x%08x\n",
                       (jint)(watches[i].loc >> 32), (jint)watches[i].loc);
                result = STATUS_FAILED;
            }
            if (watch.f_name == NULL ||
                    strcmp(watch.f_name, watches[i].f_name) != 0) {
                printf("(watch#%" PRIuPTR ") wrong field name: \"%s\"",
                       i, watch.f_name);
                printf(", expected: \"%s\"\n", watches[i].f_name);
                result = STATUS_FAILED;
            }
            if (watch.f_sig == NULL ||
                    strcmp(watch.f_sig, watches[i].f_sig) != 0) {
                printf("(watch#%" PRIuPTR ") wrong field sig: \"%s\"",
                       i, watch.f_sig);
                printf(", expected: \"%s\"\n", watches[i].f_sig);
                result = STATUS_FAILED;
            }
            if (watch.is_static != watches[i].is_static) {
                printf("(watch#%" PRIuPTR ") wrong field type: %s", i,
                    (watch.is_static==JNI_TRUE)?"static":"instance");
                printf(", expected: %s\n",
                    (watches[i].is_static==JNI_TRUE)?"static":"instance");
                result = STATUS_FAILED;
            }
            return;
        }
    }
    printf("Unexpected field access catched: 0x%p\n", watch.fid);
    result = STATUS_FAILED;
}

#ifdef STATIC_BUILD
JNIEXPORT jint JNICALL Agent_OnLoad_fieldacc002(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNICALL Agent_OnAttach_fieldacc002(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNI_OnLoad_fieldacc002(JavaVM *jvm, char *options, void *reserved) {
    return JNI_VERSION_1_8;
}
#endif
jint Agent_Initialize(JavaVM *jvm, char *options, void *reserved) {
    jvmtiError err;
    jint res;

    if (options != NULL && strcmp(options, "printdump") == 0) {
        printdump = JNI_TRUE;
    }

    res = JNI_ENV_PTR(jvm)->GetEnv(JNI_ENV_ARG(jvm, (void **) &jvmti),
        JVMTI_VERSION_1_1);
    if (res != JNI_OK || jvmti == NULL) {
        printf("Wrong result of a valid call to GetEnv!\n");
        return JNI_ERR;
    }

    err = jvmti->GetPotentialCapabilities(&caps);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetPotentialCapabilities) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        return JNI_ERR;
    }

    err = jvmti->AddCapabilities(&caps);
    if (err != JVMTI_ERROR_NONE) {
        printf("(AddCapabilities) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        return JNI_ERR;
    }

    err = jvmti->GetCapabilities(&caps);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetCapabilities) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        return JNI_ERR;
    }

    if (caps.can_generate_field_access_events) {
        callbacks.FieldAccess = &FieldAccess;
        err = jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
        if (err != JVMTI_ERROR_NONE) {
            printf("(SetEventCallbacks) unexpected error: %s (%d)\n",
                   TranslateError(err), err);
            return JNI_ERR;
        }

        err = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                JVMTI_EVENT_FIELD_ACCESS, NULL);
        if (err != JVMTI_ERROR_NONE) {
            printf("Failed to enable JVMTI_EVENT_FIELD_ACCESS: %s (%d)\n",
                   TranslateError(err), err);
            return JNI_ERR;
        }
    } else {
        printf("Warning: FieldAccess watch is not implemented\n");
    }

    return JNI_OK;
}

JNIEXPORT void JNICALL Java_nsk_jvmti_FieldAccess_fieldacc002_getReady(JNIEnv *env, jclass clz) {
    jvmtiError err;
    jclass cls;
    size_t i;

    if (!caps.can_generate_field_access_events) {
        return;
    }

    if (printdump == JNI_TRUE) {
        printf(">>> setting field access watches ...\n");
    }

    cls = JNI_ENV_PTR(env)->FindClass(JNI_ENV_ARG(env,
        "nsk/jvmti/FieldAccess/fieldacc002a"));
    if (cls == NULL) {
        printf("Cannot find fieldacc002a class!\n");
        result = STATUS_FAILED;
        return;
    }
    for (i = 0; i < sizeof(watches)/sizeof(watch_info); i++) {
        if (watches[i].is_static == JNI_TRUE) {
            watches[i].fid = JNI_ENV_PTR(env)->GetStaticFieldID(
                JNI_ENV_ARG(env, cls), watches[i].f_name, watches[i].f_sig);
        } else {
            watches[i].fid = JNI_ENV_PTR(env)->GetFieldID(
                JNI_ENV_ARG(env, cls), watches[i].f_name, watches[i].f_sig);
        }
        if (watches[i].fid == NULL) {
            printf("Cannot find field \"%s\"!\n", watches[i].f_name);
            result = STATUS_FAILED;
            return;
        }
        err = jvmti->SetFieldAccessWatch(cls, watches[i].fid);
        if (err == JVMTI_ERROR_NONE) {
            eventsExpected++;
        } else {
            printf("(SetFieldAccessWatch#%" PRIuPTR ") unexpected error: %s (%d)\n",
                   i, TranslateError(err), err);
            result = STATUS_FAILED;
        }
    }

    if (printdump == JNI_TRUE) {
        printf(">>> ... done\n");
    }
}

JNIEXPORT jint JNICALL
Java_nsk_jvmti_FieldAccess_fieldacc002_check(JNIEnv *env, jclass clz, jobject obj) {
    jclass cls;

    if (!caps.can_generate_field_access_events) {
        return result;
    }

    if (printdump == JNI_TRUE) {
        printf(">>> accessing fields ...\n");
    }

    cls = JNI_ENV_PTR(env)->FindClass(JNI_ENV_ARG(env,
        "nsk/jvmti/FieldAccess/fieldacc002a"));
    if (cls == NULL) {
        printf("Cannot find fieldacc002a class!\n");
        return STATUS_FAILED;
    }

    JNI_ENV_PTR(env)->GetStaticBooleanField(JNI_ENV_ARG(env, cls),
        watches[0].fid);
    JNI_ENV_PTR(env)->GetStaticByteField(JNI_ENV_ARG(env, cls),
        watches[1].fid);
    JNI_ENV_PTR(env)->GetStaticShortField(JNI_ENV_ARG(env, cls),
        watches[2].fid);
    JNI_ENV_PTR(env)->GetStaticIntField(JNI_ENV_ARG(env, cls),
        watches[3].fid);
    JNI_ENV_PTR(env)->GetStaticLongField(JNI_ENV_ARG(env, cls),
        watches[4].fid);
    JNI_ENV_PTR(env)->GetStaticFloatField(JNI_ENV_ARG(env, cls),
        watches[5].fid);
    JNI_ENV_PTR(env)->GetStaticDoubleField(JNI_ENV_ARG(env, cls),
        watches[6].fid);
    JNI_ENV_PTR(env)->GetStaticCharField(JNI_ENV_ARG(env, cls),
        watches[7].fid);
    JNI_ENV_PTR(env)->GetStaticObjectField(JNI_ENV_ARG(env, cls),
        watches[8].fid);
    JNI_ENV_PTR(env)->GetStaticObjectField(JNI_ENV_ARG(env, cls),
        watches[9].fid);

    JNI_ENV_PTR(env)->GetBooleanField(JNI_ENV_ARG(env, obj), watches[10].fid);
    JNI_ENV_PTR(env)->GetByteField(JNI_ENV_ARG(env, obj), watches[11].fid);
    JNI_ENV_PTR(env)->GetShortField(JNI_ENV_ARG(env, obj), watches[12].fid);
    JNI_ENV_PTR(env)->GetIntField(JNI_ENV_ARG(env, obj), watches[13].fid);
    JNI_ENV_PTR(env)->GetLongField(JNI_ENV_ARG(env, obj), watches[14].fid);
    JNI_ENV_PTR(env)->GetFloatField(JNI_ENV_ARG(env, obj), watches[15].fid);
    JNI_ENV_PTR(env)->GetDoubleField(JNI_ENV_ARG(env, obj), watches[16].fid);
    JNI_ENV_PTR(env)->GetCharField(JNI_ENV_ARG(env, obj), watches[17].fid);
    JNI_ENV_PTR(env)->GetObjectField(JNI_ENV_ARG(env, obj), watches[18].fid);
    JNI_ENV_PTR(env)->GetObjectField(JNI_ENV_ARG(env, obj), watches[19].fid);

    if (printdump == JNI_TRUE) {
        printf(">>> ... done\n");
    }

    if (eventsCount != eventsExpected) {
        printf("Wrong number of field access events: %d, expected: %d\n",
            eventsCount, eventsExpected);
        result = STATUS_FAILED;
    }
    return result;
}

#ifdef __cplusplus
}
#endif
