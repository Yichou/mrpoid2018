/*
 * Copyright (C) 2013 The Mrpoid Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.mrpoid.core;

/**
 * 保存 MR 宏定义
 * 
 * @author JianbinZhu
 *
 */
public interface MrDefines {
	public int MR_FAILED = -1,
		MR_SUCCESS = 0,
		MR_IGNORE = 1,
		MR_WAITING = 2;
	
	public int
    	MR_DIALOG_KEY_OK = 0,
    	MR_DIALOG_KEY_CANCEL = 1;
    	
	public int	MR_LOCALUI_KEY_OK = 0,
    	MR_LOCALUI_KEY_CANCEL = 1,
    	MR_LOCALUI_ACTIVE = 2;
    	
	public int	MR_EDIT_ANY = 0,  //任何字符
    	MR_EDIT_NUMERIC = 1,         // 数字
    	MR_EDIT_PASSWORD = 2,       //密码，用"*"显示
    	MR_EDIT_ALPHA = 3;			//字母
	
	public int  MR_KEY_0=0, //按键 0
        MR_KEY_1=1, //按键 1
        MR_KEY_2=2, //按键 2
        MR_KEY_3=3, //按键 3
        MR_KEY_4=4, //按键 4
        MR_KEY_5=5, //按键 5
        MR_KEY_6=6, //按键 6
        MR_KEY_7=7, //按键 7
        MR_KEY_8=8, //按键 8
        MR_KEY_9=9, //按键 9
        MR_KEY_STAR=10, //按键 *
        MR_KEY_POUND=11, //按键 #
        MR_KEY_UP=12, //按键 上
        MR_KEY_DOWN=13, //按键 下
        MR_KEY_LEFT=14, //按键 左
        MR_KEY_RIGHT=15, //按键 右
        MR_KEY_POWER=16, //按键 挂机键
        MR_KEY_SOFTLEFT=17, //按键 左软键
        MR_KEY_SOFTRIGHT=18, //按键 右软键
        MR_KEY_SEND=19, //按键 接听键
        MR_KEY_SELECT=20, //按键 确认/选择（若方向键中间有确认键，建议设为该键）
        MR_KEY_VOLUME_UP=21, //按键 侧键上
        MR_KEY_VOLUME_DOWN=22, //按键 侧键下
        MR_KEY_CLEAR=23,
        MR_KEY_A=24, //游戏模拟器A键
        MR_KEY_B=25, //游戏模拟器B键
        MR_KEY_CAPTURE=26, //拍照键
        MR_KEY_NONE=27; //按键 保留
	
	public int  MR_DIALOG_OK=0, //对话框有"确定"键。
        MR_DIALOG_OK_CANCEL=1,
        MR_DIALOG_CANCEL=2,
        MR_DIALOG_NONE=100;

	public int MR_KEY_PRESS=0, /*0*/
        MR_KEY_RELEASE=1, /*1*/
        MR_MOUSE_DOWN=2, /*2*/
        MR_MOUSE_UP=3, /*3*/
        MR_MENU_SELECT=4, /*4*/
        MR_MENU_RETURN=5, /*5*/
        MR_DIALOG_EVENT=6, /*6*/
        MR_SMS_INDICATION=7, /*7*/
        MR_EVENT_EXIT=8, /*8*/
        MR_SMS_RESULT=9, /*9*/
        MR_LOCALUI_EVENT=10, /*10*/
        MR_OSD_EVENT=11, /*11*/
        MR_MOUSE_MOVE=12, /*12*/
        MR_ERROR_EVENT=13, /*13执行异常通过这个事件来通知*/
        MR_PHB_EVENT=14,
        MR_SMS_OP_EVENT=15,
        MR_SMS_GET_SC=16,
        MR_DATA_ACCOUNT_EVENT=17,
        MR_MOTION_EVENT=18,
    
        MR_TIMER_EVENT=1001,
        MR_NET_EVENT=1002,

	    MR_EMU_ON_TIMER = 2001; //add by yichou
	
	public int NETTYPE_WIFI=0,
		NETTYPE_CMWAP=1,
		NETTYPE_CMNET=2,
		NETTYPE_UNKNOW=3;
	
	public int MR_SOUND_MIDI=0,
	   MR_SOUND_WAV=1,
	   MR_SOUND_MP3=2,
	   MR_SOUND_AMR=3,
	   MR_SOUND_PCM=4;  //8K

	public int MR_NET_ID_MOBILE=0,                  //移动
	   MR_NET_ID_CN=1,          // 联通gsm
	   MR_NET_ID_CDMA=2,       //联通CDMA
	   MR_NET_ID_NONE=3,       //未插卡
	   MR_NET_ID_OTHER=4;     /*其他网络*/

	public int MR_MEDIA_IDLE= 1001, /*1.not init; 2.device closed*/
	   MR_MEDIA_INITED = 1002,
	   MR_MEDIA_LOADED = 1003, /*1.loaded not play; 2.play finished; 3.play stoped*/
	   MR_MEDIA_PLAYING = 1004,/*1.playing*/
	   MR_MEDIA_PAUSED = 1005, /*1.paused*/
	   MR_MEDIA_SUSPENDED = 1006,
	   MR_MEDIA_SUSPENDING = 1007,
	   MR_MEDIA_NULL = 1008;

	//	public static final int
}
