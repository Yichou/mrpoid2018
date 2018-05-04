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
package com.mrpoid.game.keypad;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import org.xmlpull.v1.XmlPullParser;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.DisplayMetrics;
import android.util.Xml;
import android.view.MotionEvent;
import android.view.View;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

import com.mrpoid.MrpoidMain;
import com.mrpoid.R;
import com.mrpoid.core.MrDefines;
import com.mrpoid.core.MrpoidSettings;
import com.mrpoid.game.engine.Actor;
import com.mrpoid.game.engine.ActorGroup;
import com.mrpoid.game.engine.Director;
import com.mrpoid.game.engine.Actor.ClickCallback;

/**
 * 虚拟键盘
 * 
 * @author Yichou
 *
 */
public class Keypad extends Director implements ClickCallback {
	public static int BTN_COLOR_NORMAL = 0x000000;
	public static int BTN_COLOR_PRESS= 0xff7f00;
	public static int BTN_CONNER = 5;
	
	private static final String[] titles = {
		"1", "2", "3",
		"4", "5", "6", 
		"7", "8", "9", 
		"*", "0", "#"
	};

	private static final int[] ids = {
		MrDefines.MR_KEY_1, MrDefines.MR_KEY_2, MrDefines.MR_KEY_3,
		MrDefines.MR_KEY_4, MrDefines.MR_KEY_5, MrDefines.MR_KEY_6, 
		MrDefines.MR_KEY_7, MrDefines.MR_KEY_8, MrDefines.MR_KEY_9, 
		MrDefines.MR_KEY_STAR, MrDefines.MR_KEY_0, MrDefines.MR_KEY_POUND
	};
	
	private Context context;
	private View view;
	private Paint paint;
	private KeyLayouter layouter;
	private FloatMenuButton floatMenuBtn;
	private ActorGroup rootGroup;
	private int mode;
	private OnKeyEventListener mListener;
	private boolean mEditMode = false;
	
	int numW = 45, numH = 30, numM = 8, softW = 45, softH = 30;
	int padW = 40, padH = 30, padR = 20, padM = 8;
	
	/**
	 * 键盘布局器
	 * 
	 * @author Yichou
	 *
	 */
	public static interface KeyLayouter {
		/**
		 * 自定义键盘布局
		 * 
		 * @param keypad
		 * @param parent 容器
		 * @param landscape
		 * @param mode
		 */
		public void layout(Keypad keypad, ActorGroup root, boolean landscape, int mode);
		
		/**
		 * 视图尺寸改变通知
		 * 
		 * @param neww
		 * @param newh
		 */
		public void setSize(int neww, int newh);
	}
	
	public static interface OnKeyEventListener {
		public boolean onKeyDown(int key);

		public boolean onKeyUp(int key);
	}
	
	private Keypad() {
	}

	/**
	 * for edit mode
	 * @param c
	 */
	public Keypad(Context c) {
		init(c);
		this.mEditMode = true;
	}
	
	public void init(Context c) {
		this.context = c;
		
		paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		paint.setColor(0xf0f0f0f0);
		paint.setStyle(Paint.Style.FILL);
		paint.setShadowLayer(0, 0, 0, 0);
		paint.setTextSize(context.getResources().getDimension(R.dimen.sp1) * 16);
		
//		setLayouter(new DefKeyLayouter());
		
		rootGroup = new ActorGroup(this);
		addChild(rootGroup);
	}
	
	public void showOpactSeek(Activity activity) {
		SeekBar bar = new SeekBar(activity);
		bar.setMax(255);
		bar.setProgress(getOpacity());
		bar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				if(fromUser){
					if(progress < 20)
						progress = 20;
					else if (progress > 0xff)
						progress = 0xff;
					
					MrpoidSettings.keypadOpacity = progress;
					setOpacity(progress);
				}
			}
		});
		
		new AlertDialog.Builder(activity)
			.setTitle(R.string.pad_opacity)
			.setView(bar)
			.setPositiveButton(R.string.ok, null)
			.create()
			.show();
	}
	
	public void setLayouter(KeyLayouter layouter) {
		this.layouter = layouter;
	}
	
	public void setOnKeyEventListener(OnKeyEventListener l) {
		this.mListener = l;
	}
	
	public int getMode() {
		return mode;
	}
	
	public void setMode(int mode) {
		if(this.mode != mode){
			this.mode = mode;
		}
		reset();
	}
	
	public void switchMode() {
		mode = (mode + 1) % 3;
		reset();
	}
	
	public void startEdit() {
		mEditMode = true;
	}
	
	public void stopEdit() {
		mEditMode = false;
	}
	
	@Override
	public void draw(Canvas canvas, Paint paint) {
		super.draw(canvas, this.paint);
	}
	
	@Override
	public boolean dispatchTouchEvent(MotionEvent event) {
		return super.dispatchTouchEvent(event);
	}
	
	private void initButtonSize() {
		DisplayMetrics metrics = MrpoidMain.getResources().getDisplayMetrics();
		
		float base = MrpoidMain.getResources().getDimension(R.dimen.dp1);
		if(metrics.density >= 2.0)
			base *= 1.2f;
		
		numW = Math.round(base*45);
		numH = Math.round(base*30);
		
		numM = Math.round(base*8); 
		softW = Math.round(base*45);
				
		softH = Math.round(base*30);
		padW = Math.round(base*40); 
		padH = Math.round(base*30); 
		padR = Math.round(base*20); 
		padM = Math.round(base*8);
	}
	
	/////////////////////////////////////////////////////
	/**
	 * 创建横屏模式布局
	 */
	private void createLand() {
		boolean dpadAtLeft = MrpoidSettings.dpadAtLeft;
		boolean smpleMode = mode == 2;
		
		final ActorGroup root = rootGroup;
		
		initButtonSize();
		
		DPad pad = new DPad(this);
		pad.setSize(padW, padH, padR, padM);
		pad.setClickCallback(this);
		pad.setDragAble(mEditMode);
		root.addChild(pad);
		
		// 确定
		TextButton btnOk = new TextButton(this,
				"左软", 
				MrDefines.MR_KEY_SOFTLEFT, 0, 0,
				this);
		btnOk.setSize(softW, softH);
		btnOk.setDragAble(mEditMode);
		root.addChild(btnOk);
		
		// 返回
		TextButton btnCancel = new TextButton(this,
				"右软", 
				MrDefines.MR_KEY_SOFTRIGHT, 0, 0,
				this);
		btnCancel.setSize(softW, softH);
		btnCancel.setDragAble(mEditMode);
		root.addChild(btnCancel);
		
		ActorGroup numGroup = null;
		if(!smpleMode){
			numGroup = new ActorGroup(this);
			root.addChild(numGroup);
			
			int x = 0;
			int y = 0;
			
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 3; j++) {
					TextButton button = new TextButton(this,
							titles[i * 3 + j], 
							ids[i * 3 + j], x, y,
							this);
					button.setSize(numW, numH);
					button.setDragAble(mEditMode);
					numGroup.addChild(button);
					
					x += numW + numM;
				}
				x = 0;
				y += numH + numM;
			}
		}
		
		int y0 = viewH - (numH + numM) * 4;
		if (dpadAtLeft) {
			pad.setPosition(numM, (viewH - pad.getH() - numM));

			int x0 = viewW - (numW + numM) * 3;

			if(!smpleMode) 
				numGroup.setPosition(x0, y0);
		} else {
			pad.setPosition((viewW - pad.getW() - numM), (viewH - pad.getH() - numM));

			if(!smpleMode) 
				numGroup.setPosition(numM, y0);
		}

		btnOk.setPosition(pad.getX(), pad.getY() - btnOk.getH() - numM);
		btnCancel.setPosition(pad.getR() - btnCancel.getW() - numM, pad.getY() - btnCancel.getH() - numM);
	}
	
	/**
	 * 竖屏模式 xhidp 
	 */
	private void createXhidp() {
		boolean dpadAtLeft = MrpoidSettings.dpadAtLeft;
		boolean smpleMode = mode == 2;
		final ActorGroup root = rootGroup;
		
		initButtonSize();

		DPad pad = new DPad(this);
		pad.setSize(padW, padH, padR, padM);
		pad.setClickCallback(this);
		pad.setDragAble(mEditMode);
		root.addChild(pad);
		
		// 确定
		TextButton btnOk = new TextButton(this,
				"左软", 
				MrDefines.MR_KEY_SOFTLEFT, 0, 0,
				this);
		btnOk.setSize(softW, softH);
		btnOk.setDragAble(mEditMode);
		root.addChild(btnOk);
		
		// 返回
		TextButton btnCancel = new TextButton(this,
				"右软", 
				MrDefines.MR_KEY_SOFTRIGHT, 0, 0,
				this);
		btnCancel.setSize(softW, softH);
		btnCancel.setDragAble(mEditMode);
		root.addChild(btnCancel);
		
		if (smpleMode) {
			pad.setPosition((viewW - pad.getW())/2, (viewH - pad.getH() - numM));
			
			btnOk.setPosition(numM*2, (viewH - btnOk.getH() - 2*numM));
			btnCancel.setPosition((viewW - btnCancel.getW() - 2*numM), btnOk.getY());
			return;
		}
		
		ActorGroup numGroup = new ActorGroup(this);
		root.addChild(numGroup);
		int x = 0;
		int y = 0;
		
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++) {
				TextButton button = new TextButton(this,
						titles[i * 3 + j], 
						ids[i * 3 + j], x, y,
						this);
				button.setSize(numW, numH);
				button.setDragAble(mEditMode);
				numGroup.addChild(button);
				
				x += numW + numM;
			}
			x = 0;
			y += numH + numM;
		}
		
		{
			pad.setPosition(dpadAtLeft? numM : (viewW - pad.getW() - numM), (viewH - pad.getH() - numM));
			
			int x0;
			
			x0 = dpadAtLeft? (viewW - (numW+ numM)*3) : numM;
			y = viewH - (numM + numH)*4;
			numGroup.setPosition(x0, y);
			
			btnOk.setPosition(pad.getX(), y);
			btnCancel.setPosition(pad.getR() - numW, y);
		}
	}
	
	////////// 解析区 ////////////////////////////////////////////////////////
	public File getXml(boolean isLandscape, int mode) {
		return MrpoidMain.getContext().getFileStreamPath(isLandscape ? 
				String.format("keypad_land_%d.xml", mode) : String.format("keypad_%d.xml", mode));
	}
	
	private int toInt(String s) {
		return toInt(s, 0);
	}
	
	private int toInt(String s, int def) {
		try {
			return Integer.parseInt(s);
		} catch (Exception e) {
		}
		
		return def;
	}
	
	/**
	 * 从 xml 创建键盘布局
	 * 
	 * @param landscape
	 * @throws Exception
	 */
	private void createFromXml(boolean landscape) throws Exception {
		InputStream is = new FileInputStream(getXml(landscape, mode));
		
		if(is.available() <= 0)
			throw new RuntimeException();

		XmlPullParser pullParser = Xml.newPullParser();
		pullParser.setInput(is, "UTF-8"); // 为Pull解释器设置要解析的XML数据
		int event = pullParser.getEventType();

		ActorGroup group = null;
		
		Rect rect= new Rect();
		view.getWindowVisibleDisplayFrame(rect);
		int yAdd = -rect.top;
		
		initButtonSize();
		
		
		while (event != XmlPullParser.END_DOCUMENT) {
			switch (event) {
			case XmlPullParser.START_TAG: {
				if ("root".equals(pullParser.getName())) {
					int opacity = toInt(pullParser.getAttributeValue(0), 255);
					setOpacity(opacity);

//					float scale = Float.valueOf(pullParser.getAttributeValue(1));
				}
				else if ("group".equals(pullParser.getName())) {
					group = new ActorGroup(this);
					boolean visable = Boolean.valueOf(pullParser.getAttributeValue(0));
					int x = toInt(pullParser.getAttributeValue(1));
					int y = toInt(pullParser.getAttributeValue(2));
					group.setPosition(x, y+yAdd);
					group.setVisible(visable);
					rootGroup.addChild(group);
				}
				else if ("key".equals(pullParser.getName())) {
					boolean visable = Boolean.valueOf(pullParser.getAttributeValue(2));
					
					if(!visable) break;
					
					String title = pullParser.getAttributeValue(0);
					int value = toInt(pullParser.getAttributeValue(1));
					int x = toInt(pullParser.getAttributeValue(3));
					int y = toInt(pullParser.getAttributeValue(4));
					String tag = pullParser.getAttributeValue(5);

					TextButton button = new TextButton(this, title, value);
					button.setVisible(visable);
					button.setClickCallback(this);
					button.setDragAble(mEditMode);
					
					if("soft".equals(tag)) {//most maybe null
						button.setSize(softW, softH);
					} else {
						button.setSize(numW, numH);
					}
					
					if (group != null) {
						group.addChild(button);
						button.setPosition(x, y);
					} else {
						button.setPosition(x, y+yAdd);
						rootGroup.addChild(button);
					}
				}
				else if ("dpad".equals(pullParser.getName())) {
					DPad pad = new DPad(this);
					boolean visable = Boolean.valueOf(pullParser.getAttributeValue(0));
					int x = toInt(pullParser.getAttributeValue(1));
					int y = toInt(pullParser.getAttributeValue(2));
					
					pad.setSize(padW, padH, padR, padM);
					
					pad.setPosition(x, y+yAdd);
					pad.setVisible(visable);
					pad.setClickCallback(this);
					pad.setDragAble(mEditMode);
					
					rootGroup.addChild(pad);
				}
				break;
			}

			case XmlPullParser.END_TAG: {
				if ("group".equals(pullParser.getName())) {
					group = null;
				}
				break;
			}

			}
			
			event = pullParser.next();
		}
	}
	
	//////// xml写入 /////////////////////////////////////////////
	
	public void saveLayout() {
		
	}
	
	public void reset() {
		if(mode == 0){
			rootGroup.setVisible(false);
		} else {
			rootGroup.setVisible(true);
			rootGroup.removeAllChild();
			
			boolean landScape = (viewW > viewH);
			
			try {
				createFromXml(landScape);
			} catch (Exception e) {
				if (layouter != null) {
					layouter.setSize(viewW, viewH);
					layouter.layout(this, rootGroup, landScape, mode);
				} else {
					if(landScape)
						createLand();
					else
						createXhidp();
				}
			}
		}
		
		if(floatMenuBtn != null){
			removeChild(floatMenuBtn);
			floatMenuBtn = null;
		}
		
		//添加拖拽按钮
		floatMenuBtn = new FloatMenuButton(this, 1025);
		floatMenuBtn.setClickCallback(this);
		addChild(floatMenuBtn);
		
		floatMenuBtn.setVisible(MrpoidSettings.showFloatButton);
		
		foceReDraw();
	}
	
	public void setOpacity(int a) {
		setAlpha(a);
		paint.setAlpha(a);
		foceReDraw();
	}
	
	public void foceReDraw() {
		if(view != null)
			view.invalidate();
	}
	
	public int getOpacity() {
		return alpha;
	}
	
	public void attachView(View view) {
		this.view = view;
		
		setOpacity(MrpoidSettings.keypadOpacity);
		reset();
		
//		view.getViewTreeObserver().addOnDrawListener(new ViewTreeObserver.OnDrawListener() {
//			
//			@Override
//			public void onDraw() {
//				// TODO Auto-generated method stub
//				System.out.println("------- onDraw");
//			}
//		});
//		
//		view.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
//			
//			@Override
//			public void onGlobalLayout() {
//				// TODO Auto-generated method stub
//				System.out.println("------- onGlobalLayout");
//			}
//		});
	}
	
	@Override
	public void onViewSizeChanged(int width, int height) {
		super.onViewSizeChanged(width, height);
		
//		System.err.println("----------- onViewSizeChanged");
		
		reset();
	}

	@Override
	public void invalida(Actor a) {
		foceReDraw();
	}

	@Override
	public void onClick(int key, boolean down) {
		if(mEditMode)
			return;
		
		if( mListener != null) {
			if(down)
				mListener.onKeyDown(key);
			else
				mListener.onKeyUp(key);
		}
	}
	
	private static Keypad instance;
	/**
	 * @return the instance
	 */
	public static Keypad getInstance() {
		if(instance == null)
			instance = new Keypad();
		return instance;
	}
}
