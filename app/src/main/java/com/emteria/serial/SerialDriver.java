/*
 * Copyright (C) 2017 emteria
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

package com.emteria.serial;

public class SerialDriver
{
    public static native void open();
    public static native void logcat();
    public static native void close();
}
