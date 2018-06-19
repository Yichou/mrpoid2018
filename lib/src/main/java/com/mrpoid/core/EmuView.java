/*
 * Copyright (C) 2013 The Mrpoid Project
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
package com.mrpoid.core;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * 
 * @author Yichou 2012-10-09
 *
 */
public class EmuView extends SurfaceView {
	public static final String TAG = "EmulatorSurface";
	

	public EmuView(Context context) {
		super(context);
		
//		getHolder().addCallback(this);
//		getHolder().setFormat(PixelFormat.TRANSPARENT);
		setFocusableInTouchMode(true);
//		requestFocus();
		setFocusable(true);
		setKeepScreenOn(true);
	}
}

