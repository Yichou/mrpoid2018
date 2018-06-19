package com.mrpoid.game.keysprite;

/**
 * 按键事件监听
 * 
 * @author root 2013-09-01
 *
 */
public interface KeyEventListener {
	public void onKeyDown(int key, Sprite sprite);
	
	public void onKeyUp(int key, Sprite sprite);
}
