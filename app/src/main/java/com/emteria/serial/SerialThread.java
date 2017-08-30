/*
 * Copyright (C) 2017 emteria
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

package com.emteria.serial;

import android.util.Log;

import java.io.File;

public class SerialThread extends Thread
{
    private static final String TAG = SerialThread.class.getSimpleName();
    static { System.loadLibrary("serial"); }

    private void setPermissions()
    {
        try
        {
            String[] command = { "su", "-c", "chmod 666 /dev/ttyS0" };
            Process proc = Runtime.getRuntime().exec(command);

            int result = proc.waitFor();
            Log.i(TAG, "SU terminated with result=" + result);
        }
        catch (Exception e) { Log.e(TAG, "Failed to set correct permissions", e); }
    }

    @Override
    public void run()
    {
        int tid = android.os.Process.myTid();
        Log.i(TAG, "Started a new serial thread with tid=" + tid);

        /*
         * We need access to the serial port in /dev/ttyS0
         */

        File video = new File("/dev/ttyS0");
        if (!video.canRead()) { setPermissions(); }

        /*
         * At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (i.e. the alt0 function) respectively
         */

        SerialDriver.open();
        SerialDriver.logcat();
        SerialDriver.close();

        Log.i(TAG, "Terminating serial thread with tid=" + tid);
    }
}
