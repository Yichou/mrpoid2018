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
		
		// 防止他到屏幕外面去了
		if (MrpoidSettings.lrbX < 0)
			MrpoidSettings.lrbX = 0;
		else if (MrpoidSettings.lrbX + w > am.viewW)
			MrpoidSettings.lrbX = (int) (am.viewW - w);

		if (MrpoidSettings.lrbY < 0)
			MrpoidSettings.lrbY = 0;
		else if (MrpoidSettings.lrbY + h > am.viewH)
			MrpoidSettings.lrbY = (int) (am.viewH - h);
		this.x = MrpoidSettings.lrbX;
		this.y = MrpoidSettings.lrbY;
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
	
	/* (non-Javadoc)
	 * @see com.mrpoid.gui.engine.TextButton#draw(android.graphics.Canvas, android.graphics.Paint)
	 */
	@Override
	public void draw(Canvas canvas, Paint paint) {
		paint.setColor(alpha<<24 & 0x181818);
		canvas.drawRect(x, y, x+w, y+h, paint);
	}

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
		MrpoidSettings.lrbX = (int) x;
		MrpoidSettings.lrbY = (int) y;

		if(!moved){
			clicked(id, false);
		}
		moved = false;

		invalida();
	}
}
