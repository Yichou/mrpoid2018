package com.mrpoid.game.keysprite;

import java.io.File;
import java.util.ArrayList;

public interface KeySprite {
	public ArrayList<Sprite> getSpriteList();
	
	public void add(Sprite sprite) ;
	
	public void remove(int index);

	public Sprite get(int index);
	
	public int count();
	
	/**
	 * 运行精灵
	 * 
	 * @param repeatTimes 重复次数，-1 永久重复
	 */
	public void run(KeyEventListener l);
	
	/**
	 * 停止运行
	 */
	public void stop();

	/**
	 * 写入文件
	 * 
	 * @param path
	 */
	public void toXml(File file) throws Exception;
	
	/**
	 * 从文件读取
	 * 
	 * @param path
	 */
	public void fromXml(File file) throws Exception;
}
