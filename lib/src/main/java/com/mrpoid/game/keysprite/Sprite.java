package com.mrpoid.game.keysprite;

public class Sprite {
	public String title;
	public int value;
	public int time;
	
	
	public Sprite(String title, int value, int time) {
		this.title = title;
		this.value = value;
		this.time = time;
	}
	
//	public int nameToValue(String name) {
//		if(name)
//	}

	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder("按键 ");
		sb.append(title);
		sb.append("，持续 ");
		sb.append(String.valueOf(time));
		sb.append(" ms");
		
		return sb.toString();
	}
}
