package com.mrpoid.game.keypad;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;

import com.mrpoid.MrpoidMain;
import com.mrpoid.R;
import com.mrpoid.core.MrDefines;
import com.mrpoid.game.engine.Actor;
import com.mrpoid.game.engine.Director;

/**
 * 方向键盘
 * 
 * @author Yichou
 * 
 */
public class DPad extends Actor {
	private int keyWidth, keyHeight;
	private int px, py;
	private int curPressKey = -1;
	
	private int btnW, btnH; 
	private int btnR;
	private int btnPad = 10;

	
	public DPad(Director am) {
		super(am);
		this.id = MrDefines.MR_KEY_SELECT;
	}
	
	/**
	 * 设置各个尺寸
	 * 
	 * @param dw 导航按钮宽
	 * @param dh 导航按钮高
	 * @param dr 确认按钮半径
	 * @param dm 按钮间距
	 */
	public void setSize(int dw, int dh, int dr, int dm) {
		btnW = dw;
		btnH = dh;
		btnPad =dm;
		btnR = dr;
		
		setW(btnW+btnPad+btnR*2+btnPad+btnW);
		setH(btnH+btnPad+btnR*2+btnPad+btnH);
		keyWidth = (int) (w / 3);
		keyHeight = (int) (h / 3);
	}

	@Override
	public boolean isHit(float fx, float fy) {
		if (super.isHit(fx, fy)) {
			if (getPressKey(fx, fy) != -1) {
				return true;
			}
		}

		return false;
	}
	
	protected int getColor(boolean b) {
		return (alpha<<24 | (b? Keypad.BTN_COLOR_PRESS : Keypad.BTN_COLOR_NORMAL));
	}
	
	final RectF rectF = new RectF();
	final Rect rBounds = new Rect();

	@Override
	public void draw(Canvas canvas, Paint paint) {
		final float cr = Keypad.BTN_CONNER * MrpoidMain.getResources().getDimension(R.dimen.dp1);
		
		//上
		paint.setColor(getColor(curPressKey == keyMap[0][1]));
		rectF.set(x+btnW+btnPad, y, x+btnW+btnPad+btnW, y+btnH);
		canvas.drawRoundRect(rectF, cr, cr, paint);
//		canvas.drawRect(x+btnW+btnPad, y, x+btnW+btnPad+btnW, y+btnH, paint);
//		^><v
		//下
		paint.setColor(getColor(curPressKey == keyMap[2][1]));
		rectF.offset(0, btnH+btnPad*2+btnR*2);
		canvas.drawRoundRect(rectF, cr, cr, paint);
//		canvas.drawRect(x+btnW+btnPad, y+btnH+btnPad+btnR*2+btnPad, x+btnW+btnPad+btnW, y+btnH+btnPad+btnR*2+btnPad+btnH, paint);
		
		float my = (y + (btnH*2 + btnR*2 + btnPad*2)/2 - btnH/2);
		
		//左
		paint.setColor(getColor(curPressKey == keyMap[1][0]));
		rectF.offsetTo(x, my);
		canvas.drawRoundRect(rectF, cr, cr, paint);
		canvas.drawText("←", my, my, paint);
//		canvas.drawRect(x, my, x+btnW, my+btnH, paint);
		
		//右
		paint.setColor(getColor(curPressKey == keyMap[1][2]));
		rectF.offset(btnW*2+btnPad*2, 0);
		canvas.drawRoundRect(rectF, cr, cr, paint);
//		canvas.drawRect(x+btnW+btnPad+btnW+btnPad, my, x+btnW+btnPad+btnW+btnPad+btnW, my+btnH, paint);
		
		//圆
		float cx = x+btnW+btnPad+btnR;
		float cy = y+btnH+btnPad+btnR;
		paint.setColor(getColor(curPressKey == keyMap[1][1]));
		canvas.drawCircle(cx, cy, btnR, paint);
		
		String title = "ok";
		if(title != null) {
			paint.getTextBounds(title, 0, title.length(), rBounds);
			paint.setColor(0xfff0f0f0);
			canvas.drawText(title, 
					cx - rBounds.width()/2, 
					cy + rBounds.height()/2, paint);
		}
	}

	/**
	 * 为省事而定义的按键映射数组
	 */
	private static final byte[][] keyMap = { 
		{ -1, MrDefines.MR_KEY_UP, -1 }, 
		{ MrDefines.MR_KEY_LEFT, MrDefines.MR_KEY_SELECT, MrDefines.MR_KEY_RIGHT }, 
		{ -1, MrDefines.MR_KEY_DOWN, -1 } 
	};

	private int getPressKey(float fx, float fy) {
		px = (int) (fx / keyWidth);
		py = (int) (fy / keyHeight);

		if (px > 2)
			px = 2;
		else if (px < 0)
			px = 0;
		if (py > 2)
			py = 2;
		else if (py < 0)
			py = 0;

		return keyMap[py][px];
	}

	@Override
	public boolean touchDown(float fx, float fy) {
		super.touchDown(fx, fy);
		
		curPressKey = getPressKey(fx, fy);
		if (curPressKey != -1) {
			invalida();
			clicked(curPressKey, true);
		}

		return true;
	}

	@Override
	public void touchMove(float fx, float fy) {
		super.touchMove(fx, fy);
		
		if(!isDragAble()) {
			int tmp = getPressKey(fx, fy);
			if (tmp != -1 && tmp != curPressKey) {
				invalida();
				clicked(curPressKey, false);
				curPressKey = tmp;
				clicked(curPressKey, true);
			}
		}
	}

	@Override
	public void touchUp(float fx, float fy) {
		super.touchUp(fx, fy);
		
		if (curPressKey != -1) {
			clicked(curPressKey, false);
			curPressKey = -1;
			invalida();
		}
	}
}
