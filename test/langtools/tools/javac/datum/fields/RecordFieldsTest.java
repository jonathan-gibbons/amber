/*
 * Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
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

/*
 * @test
 * @summary check the existence of private fields with same name and signature of the
 *          declared record components
 * @modules jdk.compiler/com.sun.source.util
 *          jdk.compiler/com.sun.tools.javac.api
 *          jdk.compiler/com.sun.tools.javac.code
 *          jdk.compiler/com.sun.tools.javac.file
 *          jdk.compiler/com.sun.tools.javac.util
 * @ignore
 */

/*
  this test is failing with a message saying that: Field "i" in class RecordFieldsTest has illegal signature "Ljava/util/List<Ljava/lang/String;>;"
 */

import java.lang.reflect.*;
import java.util.*;

//import com.sun.tools.javac.util.Assert;

public record RecordFieldsTest(List<String> i) {

    public static void main(String... args) {
        RecordFieldsTest r = new RecordFieldsTest(null);
        Class<?> classOfRecord = r.getClass();
        Field[] fields = classOfRecord.getDeclaredFields();
        for (Field f: fields) {
            System.out.println("field " + f.toString());
            //Assert.check(Modifier.isPrivate(f.getModifiers()));
            System.out.println("type: " + f.getType());
            System.out.println("signature: " + f.toGenericString());
        }
    }
}
