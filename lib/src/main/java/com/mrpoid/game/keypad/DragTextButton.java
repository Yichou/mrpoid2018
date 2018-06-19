package com.mrpoid.game.keypad;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;

import com.mrpoid.game.engine.Director;

/**
 * 可拖拽按钮
 * 
 * @author Yichou
 *
 */
public class DragTextButton extends TextButton {
	public DragTextButton(Director am, String title, int id, float x, float y) {
		super(am, title, id, x, y, null);

		paint = new Paint();
		paint.setColor(Color.CYAN);
		paint.setAlpha(0x80);
	}
	
	Paint paint;
	
	@Override
	public void draw(Canvas canvas, Paint paint) {
		super.draw(canvas, paint);
		
		if(moving){
			canvas.drawLine(0, y, am.viewW, y, this.paint);
			canvas.drawLine(0, y+h, am.viewW, y+h, this.paint);
			canvas.drawLine(x, 0, x, am.viewH, this.paint);
			canvas.drawLine(x+w, 0, x+w, am.viewH, this.paint);
		}
	}

	private boolean moving;
	private float lastX, lastY; 
	@Override
	public boolean touchDown(float fx, float fy) {
		moving = false;
		lastX = fx+this.x;
		lastY = fy+this.y;
		
		invalida();
		
		return true;
	}

	@Override
	public void touchMove(float fx, float fy) {
		fx += this.x;
		fy += this.y;
		
			x += (fx - lastX);
			y += (fy - lastY);
			lastX = fx;
			lastY = fy;
			moving = true;
			
			invalida();
	}

	@Override
	public void touchUp(float fx, float fy) {
		if(!moving){
			clicked(0, false);
		}
		moving = false;

		invalida();
	}
}
