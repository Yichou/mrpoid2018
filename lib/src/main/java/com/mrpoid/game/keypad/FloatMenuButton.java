package com.mrpoid.game.keypad;

import android.graphics.Canvas;
import android.graphics.Paint;

import com.mrpoid.core.MrpoidSettings;
import com.mrpoid.game.engine.Director;

/**
 * 浮动菜单按钮
 * 
 * @author Yichou
 *
 */
public class FloatMenuButton extends TextButton {
	
	
	public FloatMenuButton(Director am, int id) {
		super(am, null, id);
		
		this.x = 5;
		this.y = 5;
		this.w = 100;
		this.h = 80;
		this.alpha = 0x80;
		this.title = "菜单";
	}

	private boolean moved;
	private float lastX, lastY; 
	
	@Override
	public boolean touchDown(float fx, float fy) {
		moved = false;
		lastX = fx+this.x;
		lastY = fy+this.y;
		
		invalida();
		
		return true;
	}
	
//	@Override
//	public void draw(Canvas canvas, Paint paint) {
//		paint.setColor(alpha<<24 & 0xf00000);
//		canvas.drawRoundRect(x, y, x+w, y+h, 5, 5, paint);
//	}

	@Override
	public void touchMove(float fx, float fy) {
		fx += this.x;
		fy += this.y;
		
		if((Math.abs(lastX - fx) > 5 && Math.abs(lastY - fy) > 5)){
			moved = true;
		}
			x += (fx - lastX);
			y += (fy - lastY);
			lastX = fx;
			lastY = fy;
			
			if(x < 0) 
				x=0;
			else if(x+w > am.viewW) 
				x = am.viewW - w;
			
			if(y < 0) 
				y=0;
			else if(y+h > am.viewH) 
				y = am.viewH-h;
			
			invalida();
	}

	@Override
	public void touchUp(float fx, float fy) {
		if(!moved){
			clicked(id, false);
		}
		moved = false;

		invalida();
	}
}
