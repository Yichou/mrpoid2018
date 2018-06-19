package com.mrpoid.apps.procmgr;

public interface IMssageCodes {
	public int MSG_HELLO = 0x1001;
	public int MSG_HEART = 0x1002; // 心跳消息
	
	public int MSG_START = 0x1011;
	public int MSG_PAUSE = 0x1012;
	public int MSG_RESUME = 0x1013;
	public int MSG_EXIT = 0x1014;
	
	public int MSG_LAUNCH_FINISH = 0x1021;
	
	public int MSG_BYBY = 0x1fff;
}
