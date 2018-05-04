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

import com.mrpoid.R;

/**
 * 
 * @author Yichou 2012-10-09
 *
 */
public class EmuSurface extends SurfaceView implements SurfaceHolder.Callback, Handler.Callback {
	public static final String TAG = "EmulatorSurface";
	
	private static final int	MSG_DRAW = 0x1001;
	private static final int	MSG_HELLO = 0x1002;

	private Emulator emulator;
	private MrpScreen screen;
	private SurfaceHolder holder;
	private Paint paint;
	private HandlerThread drawThread; //刷屏线程
	private Handler drawHandler;
	private int backgroundColor;
	private Bitmap bitmapBg = null;

	
	@Override
	public boolean handleMessage(Message msg) {
		switch (msg.what) {
		case MSG_DRAW: {// 刷屏
			myDraw();
			break;
		}
		
		case MSG_HELLO:
			EmuLog.i(TAG, "hello I am " + Thread.currentThread().getName());
			break;
			
		default:
			return false;
			
		}
		
		return true;
	}
	
	private void createDrawThread() {
		if(drawThread != null)
			return;
		
		drawThread = new HandlerThread("drawThread");
		drawThread.start();
		drawHandler = new Handler(drawThread.getLooper(), this);
		drawHandler.sendEmptyMessage(MSG_HELLO);

		EmuLog.i(TAG, "drawThread id = " + drawThread.getId());
	}
	
	public EmuSurface(Context context) {
		super(context);
		
		createDrawThread();
		
		getHolder().addCallback(this);
//		getHolder().setFormat(PixelFormat.TRANSPARENT);
		setFocusableInTouchMode(true);
		setFocusable(true);
		requestFocus();
		
		emulator = Emulator.getInstance();
		emulator.attachSurface(this);
		screen = emulator.getScreen();
		
		paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		paint.setColor(Color.WHITE);
		paint.setTextSize(16);
		paint.setStyle(Paint.Style.FILL);

		backgroundColor = 0xfff0f0f0;
	}
	
	public synchronized void onActivityDestroy() {
		if(drawThread != null) {
			drawThread.quit();
			
			//此处应该 join 等待 drawThread 结束？
			try {
				drawThread.join();
				EmuLog.i(TAG, "drawThread join finish!");
			} catch (InterruptedException e) {
			}
			
			drawHandler = null;
			drawThread = null;
		}
	}
	
	public void setBitmapBg(Bitmap bitmapBg) {
		this.bitmapBg = bitmapBg;
		
		postDraw();
	}
	
	private void myDraw() {
		Canvas canvas = holder.lockCanvas();
		if (canvas != null) {
			if(bitmapBg != null) {
				canvas.drawBitmap(bitmapBg, 0, 0, null);
			} else {
				canvas.drawColor(backgroundColor);
			}
			
//			if (Prefer.enableAntiAtial)
//				canvas.setDrawFilter(filter);
//			else
//				canvas.setDrawFilter(null);
			
			screen.draw(canvas);
			
			holder.unlockCanvasAndPost(canvas);
		}
	}
	
	public void setBgColor(int bgColor) {
		this.backgroundColor = bgColor;
		
		postDraw();
	}
	
	public int getBgColor() {
		return backgroundColor;
	}
	
	public void refresh() {
		postDraw();
	}
	
	public void postDraw() {
		drawHandler.sendEmptyMessage(MSG_DRAW);
	}
	
	
	////////////// 回调 /////////////////////////////////////////////////
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
//		EmuLog.d(TAG, "surfaceChanged");
		screen.surfaceChanged(width, height);
		postDraw();
	}

	private boolean surfaceCreated = false;
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
//		EmuLog.d(TAG, "surfaceCreated");

		this.holder = holder;
		surfaceCreated = true;
		
		/**
		 * 如果 surfaceCreated 是立即执行的会怎么样？
		 * 
		 * 好在不是。。。
		 */
		if (!emulator.isRunning()) {
//			emulator.start();
		} else {
			postDraw();
		}
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
//		EmuLog.d(TAG, "surfaceDestroyed");
		
		surfaceCreated = false;
	}
	
	public void flush() {
		if (surfaceCreated)
			postDraw();
	}
	
	public static int transKeycode(int andcode) {
		switch (andcode) {
		case KeyEvent.KEYCODE_DPAD_UP:
			return MrDefines.MR_KEY_UP;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			return MrDefines.MR_KEY_RIGHT;
		case KeyEvent.KEYCODE_DPAD_LEFT:
			return MrDefines.MR_KEY_LEFT;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			return MrDefines.MR_KEY_DOWN;
		case KeyEvent.KEYCODE_DPAD_CENTER:
			return MrDefines.MR_KEY_SELECT;
			
		case KeyEvent.KEYCODE_SOFT_LEFT:
		case KeyEvent.KEYCODE_MENU:
			return MrDefines.MR_KEY_SOFTLEFT;
		case KeyEvent.KEYCODE_SOFT_RIGHT:
		case KeyEvent.KEYCODE_BACK:
		case KeyEvent.KEYCODE_CLEAR:
			return MrDefines.MR_KEY_SOFTRIGHT;
			
		case KeyEvent.KEYCODE_VOLUME_UP:
			return MrDefines.MR_KEY_UP;
		case KeyEvent.KEYCODE_VOLUME_DOWN:
			return MrDefines.MR_KEY_DOWN;
			
		case KeyEvent.KEYCODE_1:
			return MrDefines.MR_KEY_1;
		case KeyEvent.KEYCODE_2:
			return MrDefines.MR_KEY_2;
		case KeyEvent.KEYCODE_3:
			return MrDefines.MR_KEY_3;
		case KeyEvent.KEYCODE_4:
			return MrDefines.MR_KEY_4;
		case KeyEvent.KEYCODE_5:
			return MrDefines.MR_KEY_5;
		case KeyEvent.KEYCODE_6:
			return MrDefines.MR_KEY_6;
		case KeyEvent.KEYCODE_7:
			return MrDefines.MR_KEY_7;
		case KeyEvent.KEYCODE_8:
			return MrDefines.MR_KEY_8;
		case KeyEvent.KEYCODE_9:
			return MrDefines.MR_KEY_9;
		case KeyEvent.KEYCODE_0:
			return MrDefines.MR_KEY_0;
		case KeyEvent.KEYCODE_STAR:
			return MrDefines.MR_KEY_STAR;
		case KeyEvent.KEYCODE_POUND:
			return MrDefines.MR_KEY_POUND;
		case KeyEvent.KEYCODE_CALL:
			return MrDefines.MR_KEY_SEND;
			
		case KeyEvent.KEYCODE_ENTER:
			return MrDefines.MR_KEY_SELECT;
		}
		
		return -1;
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		//触屏
		screen.onTouchEvent(event);
		return true;
	}

	private boolean handled = false;
	static int lastX, lastY;

	@Override
	public boolean onTrackballEvent(MotionEvent event) {
		int x = (int) event.getX();
		int y = (int) event.getY();

		if (!handled) {
			lastX = (int) event.getX();
			lastY = (int) event.getY();
		} else {
			if (x < lastX)
				emulator.postMrpEvent(MrDefines.MR_KEY_PRESS, MrDefines.MR_KEY_LEFT, 0);
			else if (x > lastX)
				emulator.postMrpEvent(MrDefines.MR_KEY_PRESS, MrDefines.MR_KEY_RIGHT, 0);
			else if (y > lastY)
				emulator.postMrpEvent(MrDefines.MR_KEY_PRESS, MrDefines.MR_KEY_DOWN, 0);
			else if (y < lastY)
				emulator.postMrpEvent(MrDefines.MR_KEY_PRESS, MrDefines.MR_KEY_UP, 0);
		}

		return super.onTrackballEvent(event);
	}
	
	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if(MrpoidSettings.noKey && keyCode == KeyEvent.KEYCODE_BACK)
			keyCode = KeyEvent.KEYCODE_MENU;
		
		if (keyCode == KeyEvent.KEYCODE_MENU) {
			return super.onKeyUp(keyCode, event);
		} else if ((keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
				&& !MrpoidSettings.catchVolumekey) {
			return super.onKeyUp(keyCode, event);
		} else {
			emulator.postMrpEvent(MrDefines.MR_KEY_RELEASE, transKeycode(keyCode), 0);
		}
		
		return true;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (MrpoidSettings.noKey && keyCode == KeyEvent.KEYCODE_BACK)
			keyCode = KeyEvent.KEYCODE_MENU;
		
		if (keyCode == KeyEvent.KEYCODE_MENU) {
			return super.onKeyDown(keyCode, event);
		} else if ((keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
				&& !MrpoidSettings.catchVolumekey) {
			return super.onKeyDown(keyCode, event);
		} else {
			emulator.postMrpEvent(MrDefines.MR_KEY_PRESS, transKeycode(keyCode), 0);
		}
		
		return true;
	}
	
}

