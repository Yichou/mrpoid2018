package com.mrpoid.game.engine;

public class Position {
	public float x, y;

	
	
	public Position() {
		super();
	}

	public Position(float x, float y) {
		set(x, y);
	}

	public void set(float x, float y) {
		this.x = x;
		this.y = y;
	}

	public void add(float x, float y) {
		this.x += x;
		this.y += y;
	}

	public void sub(float x, float y) {
		this.x -= x;
		this.y -= y;
	}
}
