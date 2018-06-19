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

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Rect;
import android.graphics.RectF;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.Toast;

import com.edroid.common.utils.Logger;

/**
 * @author Yichou 2013-4-19
 */
public class EmuScreen implements View.OnTouchListener, View.OnKeyListener, SurfaceHolder.Callback, Handler.Callback {
	static final String TAG = "EmuScreen";
	static final Logger log = Emulator.log;
	
	private static final String TEST = "1.你好，Hellogfa"; 
	
	private Emulator emulator;
	private EmuConfig cfg;
	private SurfaceHolder surfaceHolder;

	private HandlerThread drawThread;
	private Handler drawHandler;
	private boolean surfaceOk;


	public Bitmap bitmap, cacheBitmap;
	public Canvas cacheCanvas = new Canvas();
	private RectF region = new RectF(); //绘制区域

	private float scaleX, scaleY;
//	public Point size = new Point(); //大小
	private int viewW, viewH;
	private int screenW, screenH;
	
	private float CHR_H;
	private Paint paint, bmpPaint;
	private Rect textRect = new Rect(); //测量字符用
	private Rect textRectD = new Rect(); //测量字符用
	private char[] tmpBuf = new char[2];
	
//	private Typeface mTypeface;
	private int font_ansi_w,font_ansi_h,font_w,font_h;

	Rect testRect = new Rect();
	Paint testPaint = new Paint();
	boolean debugDraw = false;

	private Rect dRectSrc = new Rect(), dRectDst = new Rect();

	@Override
	public boolean handleMessage(Message msg) {
		if(msg.what == 1)
			flush();
		else if(msg.what == 2) {
			flush();
			drawHandler.sendEmptyMessageDelayed(2, 10000); //5s自动刷屏玩玩看
		}
		return true;
	}

	public EmuScreen(Emulator emulator) {
		this.emulator = emulator;
		cfg = EmuConfig.getInstance();

		drawThread = new HandlerThread("draw");
		drawThread.start();
		drawHandler = new Handler(drawThread.getLooper(), this);
		
		paint = new Paint();
		paint.setTextSize(cfg.sysfontSize);
		paint.setAntiAlias(true);
		
		bmpPaint = new Paint();
		
//		if(FileUtils.assetExist(emulator.getContext().getAssets(), "fonts/COUR.TTF")) {
//			mTypeface = Typeface.createFromAsset(emulator.getContext().getAssets(), "fonts/COUR.TTF");
//			paint.setTypeface(mTypeface);
//		}

//		scaleX = cfg.
		testPaint.setStyle(Style.STROKE);

		EmuView view = emulator.getView();
		view.setOnKeyListener(this);
		view.setOnTouchListener(this);

		surfaceHolder = view.getHolder();
		surfaceHolder.addCallback(this);
		if(!surfaceHolder.isCreating())
			surfaceOk = true;

		setSize(cfg.scnw, cfg.scnh);

		drawHandler.sendEmptyMessageDelayed(2, 100000);
	}

//	public void init() {
//		setMrpScreenSize(screenSize.width, screenSize.height);
//		clear(Color.WHITE);
//	}

	/**
	 * 初始化paint，设置字体宽高，优化：等宽字体不用检测直接赋值
	 * 
	 * @param size
	 */
	public void setTextSize(int size) {
		paint.setTextSize(size);
		paint.getTextBounds(TEST, 0, TEST.length(), textRect);
		tmpBuf[0] = '鼎'; 
		tmpBuf[1] = 0;
		paint.getTextBounds(tmpBuf, 0, 1, textRectD);
		CHR_H = textRect.height();
		
		font_w=size;
		font_h=size;
		font_ansi_w=(int) Math.ceil(paint.measureText(tmpBuf, 0, 1));
		font_ansi_h=size;// textRect.height();
	}
	
	public synchronized void recyle() {
		if(bitmap != null) {
			bitmap.recycle();
			bitmap = null;
		}
		
		if(cacheBitmap != null){
			cacheBitmap.recycle();
			cacheBitmap = null;
		}
	}

	public void N2J_drawChar(int c, int x, int y, int color) {
		tmpBuf[0] = (char) c; 
		tmpBuf[1] = 0;
		
//		System.out.println("" + tmpBuf[0] + " " + x + "," + y);

		paint.setColor(color);
		paint.getTextBounds(tmpBuf, 0, 1, testRect);
		
		if(debugDraw)
		{
			testPaint.setAlpha(0xff);
			testPaint.setColor(Color.RED);
			cacheCanvas.drawLine(x, y, x+textRect.width(), y, testPaint);
			cacheCanvas.drawLine(x, y, x, y+textRect.height(), testPaint);
		}
		
		//顶、左 对齐 +charH-2
//		x += -testRect.left;
//		y += -textRect.top;
//		cacheCanvas.drawText(tmpBuf, 0,1, x, y, paint);
		
		//顶、左 对齐 +charH-2  风的影子
		cacheCanvas.drawText(tmpBuf, 0,1, x, y+CHR_H-2, paint);

		if(debugDraw) {
			testRect.offset((int)x, (int)y);
			
			testPaint.setColor(Color.BLUE);
			testPaint.setAlpha(0x80);
			
			cacheCanvas.drawRect(testRect, testPaint);
		}
	}
	
	StringBuilder sb = new StringBuilder(256);
	
	public void N2J_measureChar(int ch) {
		tmpBuf[0] = (char)ch;
		tmpBuf[1] = 0;
		
//		sb.append(tmpBuf[0]);
//		if(sb.length() > 200) {
//			System.out.println(sb.toString());
//			sb = new StringBuilder(256);
//		}

		paint.getTextBounds(tmpBuf, 0, 1, textRect);
		
		if (ch < 128) {
			emulator.N2J_charW = font_ansi_w;// (int)Math.ceil(paint.measureText(tmpBuf,
												// 0, 1));
			emulator.N2J_charH = font_ansi_h;
		} else {
			emulator.N2J_charW = font_w;
			emulator.N2J_charH = font_h;
		}
		
		emulator.N2J_charW = textRect.width();//(int)Math.ceil(paint.measureText(tmpBuf, 0, 1));
		emulator.N2J_charH = textRect.height();
		
//		if(emulator.N2J_charH > CHR_H)
//			CHR_H = emulator.N2J_charH;
		
//		Log.i("---", "measure" + String.valueOf(tmpBuf));
	}
	
	public void N2J_drawBitmap(int ch) {
//		bitmap.
	}
	
	private void createBitmap() {
		//重新创建屏幕位图
		if(bitmap != null)
			bitmap.recycle();
		
		if(cacheBitmap != null)
			cacheBitmap.recycle();
		
		cacheBitmap = Bitmap.createBitmap(screenW, screenH, Config.RGB_565);
		bitmap = Bitmap.createBitmap(screenW, screenH, Config.RGB_565);
		cacheCanvas.setBitmap(cacheBitmap);
		
		emulator.native_screenReset(cacheBitmap, bitmap, screenW, screenH);
	}
	
	/**
	 * 设置屏幕可视区域尺寸
	 */
	void setViewportSize(int width, int height) {
		viewW = width;
		viewH = height;

		setScale(cfg.scaleMode);
	}

	public void setScale(int mode) {
		switch (mode) {
		case EmuConfig.SCALE_STRE:
			this.scaleX = viewW/(float)screenW;
			this.scaleY = viewH/(float)screenH;
			break;
		case EmuConfig.SCALE_2X:
			this.scaleX = this.scaleY = 2;
			break;
		case EmuConfig.SCALE_PRO: //取最小值
			this.scaleY = this.scaleX = Math.min(viewW/(float)screenW, viewH/(float)screenH);
			break;
			
		default:
			this.scaleY = this.scaleX = 1;
			break;
		}
		
		float w = screenW*scaleX;
		region.left = (viewW - w)/2;
		region.right = region.left + w;
		region.top = 0;
		region.bottom = screenH*scaleY;
	}
	
	/**
	 * 设置 mrp 屏幕大小
	 * 
	 * @param w
	 * @param h
	 */
	private void setSize(int w, int h) {
//		if(screenW == w && screenH == h)
//			return;
		
		switch (w) {
		case 176:
			setTextSize(14);
			break;
			
		case 240:
			setTextSize(16);
			break;
			
		case 320:
			setTextSize(20);
			break;
			
		case 480:
			setTextSize(40);
			break;
		}
		
		screenW = w;
		screenH = h;

		createBitmap();
	}
	
	public void setPosition(int x, int y) {
		region.offsetTo(x, y);
	}
	
	public void clear(int color) {
		cacheCanvas.drawColor(color);
		bitmap.eraseColor(color);
	}
	
	public void pause() {
	}
	
	public void resume() {
		if(emulator.isRunning()){
			if(cacheBitmap == null || cacheBitmap.isRecycled()){ //无效了，重启mrp
				
			}
			
			if(bitmap == null || bitmap.isRecycled()){
				bitmap = Bitmap.createBitmap(screenW, screenH, Config.RGB_565);
				emulator.native_screenReset(cacheBitmap, bitmap, screenW, screenH);
			}
			
			bmpPaint.setAntiAlias(cfg.anti);
		}
	}
	
	private void draw(Canvas canvas) {
		if(bitmap==null || bitmap.isRecycled()) {
			return;
		}
		
		//这个设置缩放绘制bmp抗锯齿
		bmpPaint.setFilterBitmap(cfg.anti);
		
		/**
		 * 刷屏非主线程，这里涉及到 bitmap 同时占有的问题，所以先进底层锁住
		 */
//		native_lockBitmap();
		canvas.drawBitmap(bitmap, null, region, bmpPaint);
//		native_unLockBitmap();
	}

	public void switchAnt() {
		cfg.anti = !cfg.anti;
		bmpPaint.setFilterBitmap(cfg.anti);

		flush();
	}

	public void drawBitmap(Bitmap bitmap, int sx, int sy, int sw, int sh) {
		dRectSrc.set(0, 0, bitmap.getWidth()-1, bitmap.getHeight()-1);
		dRectDst.set(sx, sy, sx+sw-1, sy+sh-1);
		cacheCanvas.drawBitmap(bitmap, dRectSrc, dRectDst, null);
	}

	public void flush() {
		if(!surfaceOk)
			return;

		Canvas canvas = surfaceHolder.lockCanvas();
		if (canvas != null) {
			canvas.drawColor(cfg.bgColor);

//			if (Prefer.enableAntiAtial)
//				canvas.setDrawFilter(filter);
//			else
//				canvas.setDrawFilter(null);

			draw(canvas);

			surfaceHolder.unlockCanvasAndPost(canvas);
		}
	}

	public void postFlush() {
		drawHandler.sendEmptyMessage(1);
	}

	public void screenShot(Context context) {
		// 获取系统图片存储路径
		File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
		if(path == null){
			path = new File(Environment.getExternalStorageDirectory(), "Pictures");
		}
		path = new File(path, "screenshot");
		path.mkdirs();

		// 根据当前时间生成图片名称
		SimpleDateFormat sdf = new SimpleDateFormat("yyyymmddHHmmss", Locale.CHINA);
		String name = sdf.format(new Date()) + ".png";

		path = new File(path, name);
		if(EmuUtils.bitmapToFile(bitmap, path)){
			Toast.makeText(context, "截图成功！\n文件保存在：" + path.getPath(), Toast.LENGTH_SHORT).show();
		}else {
			Toast.makeText(context, "截图失败！", Toast.LENGTH_SHORT).show();
		}
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

	private boolean handleKeyEvent(KeyEvent e, int keyCode) {
		if(e.getAction() == KeyEvent.ACTION_DOWN) {
			if (cfg.catchMenuButton && keyCode == KeyEvent.KEYCODE_BACK) {
				keyCode = KeyEvent.KEYCODE_MENU;
			}

			if (keyCode == KeyEvent.KEYCODE_MENU) {
				return false;
			} else if ((keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN) && !cfg.catchVolumeButton) {
				return false;
			} else {
				emulator.postMrpEvent(MrDefines.MR_KEY_PRESS, transKeycode(keyCode), 0);
			}
		}

		return true;
	}

	private float lastX, lastY;
	private void handleMotionEvent(MotionEvent event) {
		float fx =  event.getX();
		float fy =  event.getY();

		if(region.contains(fx, fy)){
			int x = (int) ((fx-region.left)/scaleX);
			int y = (int) ((fy-region.top)/scaleY);

			switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					lastX = fx;
					lastY = fy;
					emulator.postMrpEvent(MrDefines.MR_MOUSE_DOWN, x, y);
					break;

				case MotionEvent.ACTION_MOVE: {
					float dx = lastX-fx;
					float dy = lastY-fy;
					float sx = scaleX*5;
					float sy = scaleY*6;

					if(dx>sx || dx<-sx ||dy>sy || dy<-sy){
						emulator.postMrpEvent(MrDefines.MR_MOUSE_MOVE, x, y);
						lastX = fx;
						lastY = fy;
					}

					break;
				}

				case MotionEvent.ACTION_UP:
					emulator.postMrpEvent(MrDefines.MR_MOUSE_UP, x, y);
					break;
			}
		}
	}

	@Override
	public boolean onKey(View view, int keyCode, KeyEvent e) {
		return handleKeyEvent(e, keyCode);
	}

	@Override
	public boolean onTouch(View view, MotionEvent e) {
		handleMotionEvent(e);
		return true;
	}

	@Override
	public void surfaceCreated(SurfaceHolder h) {
		log.d("surfaceCreated " + h);
		surfaceOk = true;
		surfaceHolder = h;
	}

	@Override
	public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int w, int h) {
		log.d("surfaceChanged " + w + " " + h);
		setViewportSize(w, h);
	}

	//activity pause 也会调用
	@Override
	public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
		log.d("surfaceDestroyed " + surfaceHolder);
		surfaceOk = false;
//		this.surfaceHolder = null;
//		this.surfaceHolder.removeCallback(this);
	}
}
