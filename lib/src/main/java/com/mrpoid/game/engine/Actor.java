package com.mrpoid.game.engine;

import android.graphics.Canvas;
import android.graphics.Paint;


/**
 * 演员
 * 
 * @author Yichou
 *
 */
public abstract class Actor {
	public static interface ClickCallback {
		public void onClick(int key, boolean down);
	}
	
	protected float x, y, w, h;
	protected boolean pressed;
	public int id;
	public Object tag;
	public float scale;
	
	private boolean moving;
	private float lastX, lastY;
	private boolean dragAble = false;
	
	protected boolean isShow;
	protected Director am;
	protected ClickCallback clickCallback;
	protected ActorGroup mParent;
	protected int alpha;
	
//	private Paint mLinePaint;
	
	public Actor(Director am) {
		super();
		this.am = am;
		isShow = true;
		x = y = w = h = 0;
		
//		mLinePaint = new Paint();
//		mLinePaint.setColor(Color.CYAN);
//		mLinePaint.setStyle(Style.STROKE);
//		mLinePaint.setStrokeWidth(1);
//		mLinePaint.setAlpha(0x80);
	}
	
	/**
	 * 设置是否可以拖拽移动位置
	 */
	public void setDragAble(boolean dragAble) {
		this.dragAble = dragAble;
	}
	
	public boolean isDragAble() {
		return dragAble;
	}

	public void setClickCallback(ClickCallback clickCallback) {
		this.clickCallback = clickCallback;
	}
	
	public int getId() {
		return id;
	}
	
	public float getX() {
		return x;
	}
	
	public float getR() {
		return x+w-1;
	}
	
	public float getB() {
		return y+h-1;
	}

	public void setX(float x) {
		this.x = x;
	}

	public float getY() {
		return y;
	}

	public void setY(float y) {
		this.y = y;
	}
	
	public float getW() {
		return w;
	}
	
	public float getH() {
		return h;
	}
	
	/**
	 * @param w the w to set
	 */
	public void setW(float w) {
		this.w = w;
	}
	
	/**
	 * @param h the h to set
	 */
	public void setH(float h) {
		this.h = h;
	}
	
	public void setSize(float w, float h) {
		this.w = w;
		this.h = h;
	}
	
	public void setPosition(float x, float y) {
		this.x = x;
		this.y = y;
	}
	
	public void move(float xOff, float yOff) {
		this.x += xOff;
		this.y += yOff;
	}

	public void invalida() {
		am.invalida(this);
	}
	
	public boolean isHit(float fx, float fy) {
		if (fx < 0 || fx > w || fy < 0 || fy > h)
			return false;
		return true;
	}
	
	public boolean isShow() {
		return isShow;
	}
	
	public void setVisible(boolean isShow) {
		this.isShow = isShow;
	}
	
	public void setTag(Object tag) {
		this.tag = tag;
	}
	
	public Object getTag() {
		return tag;
	}
	
	protected void clicked(int key, boolean down) {
		if(clickCallback != null){
			clickCallback.onClick(key, down);
		}
	}
	
	/**
	 * @param alpha the alpha to set
	 */
	public void setAlpha(int alpha) {
		this.alpha = alpha;
	}
	
	/**
	 * @return the alpha
	 */
	public int getAlpha() {
		return alpha;
	}
	
	protected void setParent(ActorGroup parent) {
		this.mParent = parent;
	}
	
	public ActorGroup getParent() {
		return mParent;
	}
	
	public Position getPositionInWorld(Position pos) {
		return am.getPositionInWorld(this, pos);
	}
	
	protected int getViewPortW() {
		return am.getViewW();
	}

	protected int getViewPortH() {
		return am.getViewH();
	}
	
	protected int getWroldW() {
		return 0;
	}

	protected int getWroldH() {
		return 0;
	}
	
	public void draw(Canvas canvas, Paint paint) {
		if(moving) {
//			canvas.drawLine(0, y, getViewPortW(), y, this.mLinePaint);
//			canvas.drawLine(0, y+h, getViewPortW(), y+h, this.mLinePaint);
//			canvas.drawLine(x, 0, x, getViewPortH(), this.mLinePaint);
//			canvas.drawLine(x+w, 0, x+w, getViewPortH(), this.mLinePaint);
		}
	}
	
	public boolean touchDown(float fx, float fy) {
		if(dragAble) {
			moving = false;
			lastX = fx + this.x;
			lastY = fy + this.y;
			
			invalida();
			return true;
		}
		
		return false;
	}
	
	public void touchMove(float fx, float fy) {
		if(dragAble) {
			fx += this.x;
			fy += this.y;
			
			this.x += (fx - lastX);
			this.y += (fy - lastY);
			lastX = fx;
			lastY = fy;
			moving = true;
			
			invalida();
		}
	}
	
	public void touchUp(float fx, float fy) {
		if(dragAble) {
//			if(!moving){
//				clicked(0, false);
//			}
			moving = false;
			invalida();
		}
	}
}