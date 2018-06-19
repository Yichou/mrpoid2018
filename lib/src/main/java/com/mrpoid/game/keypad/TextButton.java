package com.mrpoid.game.keypad;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;

import com.mrpoid.MrpoidMain;
import com.mrpoid.R;
import com.mrpoid.game.engine.Actor;
import com.mrpoid.game.engine.Director;


/**
 * 显示文本按钮
 * 
 * @author Yichou
 */
public class TextButton extends Actor {
	protected String title;
	private final Rect rBounds = new Rect();
	final RectF rectF = new RectF();
	
	public TextButton(Director am, 
			String title, int id, float x, float y,
			ClickCallback cb) {
		super(am);
		
		this.title = title;
		this.id = id;
		this.x = x;
		this.y = y;
		this.clickCallback = cb;
	}
	
	public TextButton(Director am, String title, int id) {
		this(am, title, id, 0, 0, null);
	}
	
	protected int getColor() {
		return (alpha<<24 | (pressed? Keypad.BTN_COLOR_PRESS : Keypad.BTN_COLOR_NORMAL));
	}
	
	public void draw(Canvas canvas, Paint paint) {
		final float cr = Keypad.BTN_CONNER * MrpoidMain.getResources().getDimension(R.dimen.dp1);
		
		paint.setColor(getColor());
//		canvas.drawRect(x, y, x+w, y+h, paint);
		rectF.set(x, y, x+w, y+h);
		canvas.drawRoundRect(rectF, cr, cr, paint);

		if(title != null) {
			paint.getTextBounds(title, 0, title.length(), rBounds);
			paint.setColor(0xfff0f0f0);
			canvas.drawText(title, x+(w-rBounds.width())/2, y+h-(h-rBounds.height())/2, paint);
		}
	}
	
	public boolean touchDown(float fx, float fy) {
		super.touchDown(fx, fy);
		
		invalida();
		clicked(id, true);
		return true;
	}
	
	public void touchUp(float fx, float fy){
		super.touchUp(fx, fy);
		
		invalida();
		clicked(id, false);
	}

	@Override
	public void touchMove(float fx, float fy) {
		super.touchMove(fx, fy);
	}
	
	public String getTitle() {
		return title;
	}
}