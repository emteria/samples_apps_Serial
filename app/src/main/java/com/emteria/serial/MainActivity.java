/*
 * Copyright (C) 2017-2021 emteria.OS project
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

package com.emteria.serial;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SerialThread thread = new SerialThread();
        thread.start();
    }
}
