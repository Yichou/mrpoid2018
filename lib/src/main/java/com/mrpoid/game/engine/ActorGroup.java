package com.mrpoid.game.engine;

import java.util.ArrayList;

import android.graphics.Canvas;
import android.graphics.Paint;


/**
 * Actor 容器，zOrder = 0 在最上层
 * 
 * @author Yichou
 *
 */
public class ActorGroup extends Actor {
	private ArrayList<Actor> actors;
	

	public ActorGroup(Director am) {
		super(am);
		
		actors = new ArrayList<Actor>(10);
	}
	
	/**
	 * zOrder 逐渐变大
	 * 
	 * @param a
	 */
	public void addChild(Actor a) {
		a.setParent(this);
		actors.add(a);
	}
	
	/**
	 * zOrder = 0 在最上层，最先接受事件
	 * 
	 * @param actor
	 * @param zOrder
	 */
	public void addChild(Actor actor, int zOrder) {
		actor.setParent(this);
		actors.add(zOrder, actor);
	}
	
	public Actor getChild(int index) {
		return actors.get(index);
	}
	
	/**
	 * 获取子 Actor 数
	 * @return
	 */
	public int getChildCount() {
		return actors.size();
	}

	/**
	 * 移除
	 * @param actor
	 */
	public void removeChild(Actor actor) {
		actors.remove(actor);
	}
	
	public void removeChild(int index) {
		actors.remove(index);
	}
	
	/**
	 * 移除所有子节点
	 */
	public void removeAllChild() {
		actors.clear();
	}
	
	/**
	 * 改变 Z序，0 为 最上层
	 * 
	 * <p>zOrder 必须在 0 ~ ({@link #getChildCount()} - 1)</p>
	 * 
	 * @param actor
	 * @param zOrder
	 */
	public void setZOrder(Actor actor, int zOrder) {
		if(zOrder < 0 || zOrder > actors.size()-1)
			return;
		
		int index = actors.indexOf(actor);
		if(index != -1 && index != zOrder){
			actors.remove(index);
			actors.add(zOrder, actor);
		}
	}

	@Override
	public void draw(Canvas canvas, Paint paint) {
		Actor actor;
		for (int i=actors.size()-1; i>=0; --i) { //最后添加的 zOrder 最大，在后添加的上层
			actor = actors.get(i);
			if(actor.isShow){
				actor.move(this.x, this.y);
				actor.setAlpha(alpha);
				actor.draw(canvas, paint);
				actor.move(-this.x, -this.y);
			}
		}
	}
	
	private Actor touchedActor = null;

	@Override
	public boolean touchDown(float fx, float fy) {
		Actor actor;
		
		touchedActor = null;
		for (int i=0; i<actors.size(); ++i) {
			actor = actors.get(i);
			
			if(!actor.isShow) continue;
			
			float tmpX = fx-actor.getX();
			float tmpY = fy-actor.getY();
			if(actor.isHit(tmpX, tmpY) && actor.touchDown(tmpX, tmpY)){
				touchedActor = actor;
				actor.pressed = true;
				return true;
			}
		}
		
		return false;
	}

	@Override
	public void touchMove(float fx, float fy) {
		if(touchedActor != null){
			float tmpX = fx-touchedActor.getX();
			float tmpY = fy-touchedActor.getY();
			touchedActor.touchMove(tmpX, tmpY);
		}
	}

	@Override
	public void touchUp(float fx, float fy) {
		if(touchedActor != null){
			float tmpX = fx-touchedActor.getX();
			float tmpY = fy-touchedActor.getY();
			touchedActor.touchUp(tmpX, tmpY);
			touchedActor.pressed = false;
		}
	}

	@Override
	public boolean isHit(float fx, float fy) {
		Actor actor;
		
		for (int i=0; i<actors.size(); ++i) {
			actor = actors.get(i);
			
			if(!actor.isShow) continue;
			
			float tmpX = fx-actor.getX();
			float tmpY = fy-actor.getY();
				
			if (actor.isHit(tmpX, tmpY)) {
				return true;
			}
		}
		
		return false;
	}
}
