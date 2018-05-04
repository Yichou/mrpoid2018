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
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.widget.Toast;

/**
 * MRP 屏幕
 * 
 * @author Yichou
 * 
 * 2013-4-19 13:20:02
 */
public class MrpScreen {
	static final String TAG = "MrpScreen";
	
	private static final String TEST = "1.你好，Hellogfa"; 
	
	private Emulator emulator;
	private SurfaceHolder mHolder;
	
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
	
	
	public MrpScreen(Emulator emulator) {
		this.emulator = emulator;
		
		paint = new Paint();
		paint.setTextSize(16);
		paint.setAntiAlias(true);
		
		bmpPaint = new Paint();
		
//		if(FileUtils.assetExist(emulator.getContext().getAssets(), "fonts/COUR.TTF")) {
//			mTypeface = Typeface.createFromAsset(emulator.getContext().getAssets(), "fonts/COUR.TTF");
//			paint.setTypeface(mTypeface);
//		}
	}
	
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
	
	float lastX, lastY;
	public void onTouchEvent(MotionEvent event) {
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
	
	Rect testRect = new Rect();
	Paint testPaint = new Paint();
	
	{
		testPaint.setStyle(Style.STROKE);
	}
	
	boolean debugDraw = false;
	
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

		if(debugDraw)
		{
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
		
		native_reset(cacheBitmap, bitmap, screenW, screenH);
	}
	
	public void init() {
		setMrpScreenSize(screenSize.width, screenSize.height);
		clear(Color.WHITE);
	}
	
	/**
	 * emulatorView 的尺寸改变后应该重新设置屏幕缩放比例
	 * 
	 * @param width
	 * @param height
	 */
	public void surfaceChanged(int width, int height) {
		viewW = width;
		viewH = height;
		initScale();
	}
	
	public void initScale() {
		switch (screenSize.scaleMode) {
		case SCALE_STRE:
			this.scaleX = viewW/(float)screenW;
			this.scaleY = viewH/(float)screenH;
			break;
		case SCALE_2X:
			this.scaleX = this.scaleY = 2;
			break;
		case SCALE_PRO: //取最小值
			this.scaleY = this.scaleX = Math.min(viewW/(float)screenW, viewH/(float)screenH);
			break;
			
		default:
			this.scaleY = this.scaleX = 1;
			break;
		}
		
		setScale(this.scaleX, this.scaleY);
	}
	
	/**
	 * 设置 mrp 屏幕大小
	 * 
	 * @param w
	 * @param h
	 */
	private void setMrpScreenSize(int w, int h) {
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
	
	/**
	 * 设置屏幕 x,y 缩放比
	 * 
	 * @param sx
	 * @param sy
	 */
	public void setScale(float sx, float sy) {
		this.scaleX = sx;
		this.scaleY = sy;
		
		float w = screenW*scaleX;
		region.left = (viewW - w)/2;
		region.right = region.left + w;
		region.top = 0;
		region.bottom = screenH*scaleY;
		
		//竖直居中
//		if(Prefer.keypadMode == 0 && region.height() < viewH){
//			region.offset(0, (viewH-region.height())/2);
//		}
	}
	
	public void pause() {
		
	}
	
	public void resume() {
		if(emulator.isRunning()){
			if(cacheBitmap == null || cacheBitmap.isRecycled()){ //无效了，重启mrp
				
			}
			
			if(bitmap == null || bitmap.isRecycled()){
				bitmap = Bitmap.createBitmap(screenW, screenH, Config.RGB_565);
				native_reset(cacheBitmap, bitmap, screenW, screenH);
			}
			
			bmpPaint.setAntiAlias(MrpoidSettings.enableAntiAtial);
		}
	}
	
	public void draw(Canvas canvas) {
		if(!emulator.isRunning())
			return;
		
		if(bitmap==null || bitmap.isRecycled()) {
			return;
		}
		
		//这个设置缩放绘制bmp抗锯齿
		bmpPaint.setFilterBitmap(MrpoidSettings.enableAntiAtial);
		
		/**
		 * 刷屏非主线程，这里涉及到 bitmap 同时占有的问题，所以先进底层锁住
		 */
//		native_lockBitmap();
		canvas.drawBitmap(bitmap, null, region, bmpPaint);
//		native_unLockBitmap();
	}
	
	public void switchAnt() {
		MrpoidSettings.enableAntiAtial = !MrpoidSettings.enableAntiAtial;
		bmpPaint.setFilterBitmap(MrpoidSettings.enableAntiAtial);
	}
	
	private Rect dRectSrc = new Rect(), dRectDst = new Rect();
	public void drawBitmap(Bitmap bitmap, int sx, int sy, int sw, int sh) {
		dRectSrc.set(0, 0, bitmap.getWidth()-1, bitmap.getHeight()-1);
		dRectDst.set(sx, sy, sx+sw-1, sy+sh-1);
		cacheCanvas.drawBitmap(bitmap, dRectSrc, dRectDst, null);
	}
	
	public enum ScaleMode {
		/** 全屏 */
		SCALE_STRE("stretch"),
		/** 等比拉伸 */
		SCALE_PRO("proportional"),
		/** 2倍 */
		SCALE_2X("2x"),
		SCALE_ORI("original");
		
		String tag;
		
		private ScaleMode(String s){
			tag = s;
		}
	}
	
	private static final class ScreenSize {
		private int width;
		private int height;
		private String size;
		private ScaleMode scaleMode;
	}
	
	private static ScreenSize screenSize;
	
	static {
		screenSize = new ScreenSize();
		screenSize.width = 240;
		screenSize.height = 320;
		screenSize.size = "240x320";
		screenSize.scaleMode = ScaleMode.SCALE_PRO;
	}
	
	public static String getSizeTag() {
		return screenSize.size;
	}
	
	public static String getScaleModeTag() {
		return screenSize.scaleMode.tag;
	}
	
	/**
	 * 解析屏幕尺寸
	 * 
	 * @param size
	 */
	public static void parseScreenSize(String size) {
		String[] ss = size.split("x");
		if(ss == null || ss.length!=2)
			return;
		
		screenSize.width = Integer.parseInt(ss[0]);
		screenSize.height = Integer.parseInt(ss[1]);
		screenSize.size = size;
		
		EmuLog.d(TAG, "screen size set to" + screenSize.size);
	}
	
	/**
	 * 设置屏幕缩放模式
	 */
	public static void parseScaleMode(String mode) {
		ScaleMode scaleMode = ScaleMode.SCALE_STRE;
		for(ScaleMode m : ScaleMode.values()){
			if(m.tag.equals(mode)){
				scaleMode = m;
				break;
			}
		}

		screenSize.scaleMode = scaleMode;
	}
	
	/**
	 * 截屏
	 * 
	 * @param context
	 */
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
	
	public native void native_lockBitmap();
	public native void native_unLockBitmap();
	public native void native_reset(Bitmap cache, Bitmap real, int w, int h);
}
