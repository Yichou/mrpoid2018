package com.mrpoid.game.keypad;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.AnimationUtils;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.FrameLayout;
import android.widget.FrameLayout.LayoutParams;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.mrpoid.R;
import com.mrpoid.core.MrpoidSettings;
import com.mrpoid.utils.SdkUtils;

/**
 * 虚拟键盘编辑
 * 
 * @author Yichou
 *
 * 
 */
public class KeypadEditActivity extends Activity implements 
	OnSeekBarChangeListener, 
	AnimationListener,
	OnCheckedChangeListener, 
	OnClickListener {

	MyView view;
	Keypad mKeypad;
	SeekBar mOpacitySeekBar;
	CheckBox mOriCheckBox;
	ImageView mTogImageView;
	boolean bShow;
	LinearLayout mLinearLayout;
	Animation animIn, animOut, rotaLeft, rotaRight;
	TextView mTextView;
	Rect mInvalidRect = new Rect();
	
	
	@Override
	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
		if(seekBar.getId() == R.id.seekBar1) {
			
		} else if (seekBar.getId() == R.id.seekBar2) {
			if(progress < 10)
				progress = 10;
			else if(progress > 255)
				progress = 255;
			
			mKeypad.setOpacity(progress);
		}
	}
	
	@Override
	public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
		if(buttonView.getId() == R.id.checkBox1) {
//			saveKeyboard();
			setRequestedOrientation(isChecked? 
					ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE : ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		} else if (buttonView.getId() == R.id.checkBox2) {
//			saveKeyboard();
			mKeypad.setMode(isChecked? 2 : 1);
		}
	}
	
	@Override
	public void onClick(View v) {
		if(v.getId() == R.id.button1) {
			saveKeyboard();
		} else if (v.getId() == R.id.togBtn) {
			bShow = !bShow;
			
			mTogImageView.startAnimation(bShow? rotaRight : rotaLeft);
			mLinearLayout.startAnimation(bShow? animIn : animOut);
		}
	}
	
	@Override
	public void onAnimationStart(Animation animation) {
		if(animation == animIn) {
			mLinearLayout.setVisibility(View.VISIBLE);
		}
	}
	
	@Override
	public void onAnimationRepeat(Animation animation) {
	}
	
	@Override
	public void onAnimationEnd(Animation animation) {
		if(animation == animOut) {
			mLinearLayout.setVisibility(View.INVISIBLE);
		}
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().requestFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

		setContentView(R.layout.activity_keypad);
		
		animIn = AnimationUtils.loadAnimation(this, android.R.anim.slide_in_left);
		animIn.setAnimationListener(this);
		animOut = AnimationUtils.loadAnimation(this, android.R.anim.slide_out_right);
		animOut.setAnimationListener(this);
		rotaLeft = AnimationUtils.loadAnimation(this, R.anim.rota_left);
		rotaLeft.setAnimationListener(this);
		rotaRight = AnimationUtils.loadAnimation(this, R.anim.rota_right);
		rotaRight.setAnimationListener(this);
		
		((SeekBar)findViewById(R.id.seekBar1)).setOnSeekBarChangeListener(this);

		mOpacitySeekBar = ((SeekBar)findViewById(R.id.seekBar2));
		mOpacitySeekBar.setOnSeekBarChangeListener(this);
		
		
		mOriCheckBox = ((CheckBox)findViewById(R.id.checkBox1));
		DisplayMetrics display = getResources().getDisplayMetrics();
		if(display.widthPixels > display.heightPixels)
			mOriCheckBox.setChecked(true);
		mOriCheckBox.setOnCheckedChangeListener(this);
		
		((CheckBox)findViewById(R.id.checkBox2)).setOnCheckedChangeListener(this);
		findViewById(R.id.button1).setOnClickListener(this);
		
		mLinearLayout = (LinearLayout) findViewById(R.id.linerLayout1);
		
		findViewById(R.id.togBtn).setOnClickListener(this);

		bShow = true;
		mTogImageView = (ImageView) findViewById(R.id.imageView1);
		
		mTextView = (TextView) findViewById(R.id.textView1);
		mTextView.getWindowVisibleDisplayFrame(mInvalidRect);

		FrameLayout layout = (FrameLayout) findViewById(R.id.root);
		FrameLayout.LayoutParams p = new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		view = new MyView(this);
		layout.addView(view, p);
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		
		mKeypad.foceReDraw();
		
		SdkUtils.onResume(this);
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		
		SdkUtils.onPause(this);
	}
	
	private void saveKeyboard() {
		try {
			mKeypad.saveLayout();
			
			Toast.makeText(this, "保存成功！", Toast.LENGTH_SHORT).show();
		} catch (Exception e) {
			e.printStackTrace();
			Toast.makeText(this, "保存失败！", Toast.LENGTH_SHORT).show();
		}
	}
	
	@Override
	protected void onDestroy() {
		saveKeyboard();
		
		super.onDestroy();
	}
	
	public class MyView extends View {
		public MyView(Context context) {
			super(context);
			
			mKeypad = new Keypad(context);
			mKeypad.attachView(this);
			mKeypad.setMode(1);
			mOpacitySeekBar.setProgress(MrpoidSettings.keypadOpacity);
//			mKeypad.setOpacity(mOpacitySeekBar.getProgress());
			mKeypad.reset();
		}
		
		@Override
		protected void onSizeChanged(int w, int h, int oldw, int oldh) {
			super.onSizeChanged(w, h, oldw, oldh);

			mKeypad.onViewSizeChanged(w, h);
			mTextView.getWindowVisibleDisplayFrame(mInvalidRect);
		}
		
		@Override
		protected void onDraw(Canvas canvas) {
			super.onDraw(canvas);
			
			mKeypad.draw(canvas, null);
		}
		
		@Override
		public boolean onTouchEvent(MotionEvent event) {
			return mKeypad.dispatchTouchEvent(event);
		}
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekBar) {
	}

	@Override
	public void onStopTrackingTouch(SeekBar seekBar) {
	}
}
