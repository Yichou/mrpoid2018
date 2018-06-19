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

import com.edroid.common.utils.SdkUtils;
import com.edroid.common.utils.UIUtils;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.graphics.Point;
import android.preference.PreferenceManager;

/**
 * 配置管理
 * 
 * @author Yichou
 *
 */
public class MrpoidSettings implements OnSharedPreferenceChangeListener {
	static final String TAG = "Prefer";
	
	public static final long SD_RESERVE_SIZE = 1024*1024*4;		//SD卡剩余空间小于这个值时, 不再创建新文件
	public static final long ROM_RESERVE_SIZE = 1024*1024*2;	//ROM剩余空间小于这个值时, 不再创建新文件
	
	public static final String kUsePrivateDir = "usePrivateDir";
	public static final String kEnableAntiAtial = "enableAntiAtial";
	public static final String kMythroadPath = "mythroadPath";
	public static final String kSdcardPath = "sdpath";
	public static final String kEnableKeyVirb = "enableKeyVirb";
	public static final String kVmMem= "memSize";
	public static final String kUseExram= "enableExram";
	public static final String kScaleMode= "scalingMode";
	public static final String kScreenSize= "screensize";
	public static final String kShowStatusBar= "showStatusBar";
	public static final String kMultiPath= "runUnderMultiPath";
	public static final String kLimitInputLength = "limitInputLength";
	public static final String kKeypadMode = "keypadMode";
	public static final String kKeypadOpacity = "keypadOpacity";
	public static final String kUseFullDsm = "useFullDsm";
	public static final String kFullScreenEdit = "fullScnEditor";
	
	
	public static boolean enableKeyVirb = true;
	public static boolean fullScnEditor = false;
	public static boolean catchVolumekey = false;
	public static boolean dpadAtLeft = false;
	public static boolean autoUpdate = false;
	public static boolean notHintAdvSet = false;
//	public static int lrbX, lrbY;
	public static boolean enableAntiAtial = true;
	public static int screenOrientation;
	public static boolean enableSound;
	public static boolean showStatusBar;
	public static boolean noKey;
	public static boolean showMemInfo;
	public static boolean showFloatButton;
	public static boolean showMenu = true;
	public static boolean limitInputLength = true;
	public static boolean usePrivateDir = false;
	public static boolean useFullDsm = false;
	public static String mythoadPath = "";
	public static String sdPath = "";
	public static String privateDir;
	
	
	/**
	 * 不同分辨率在不同目录下运行
	 * 
	 * <p>如果为 true：
	 * 		1.ROOT_DIR = DEF_ROOT_DIR + 分辨率 </p>
	 * 
	 * <p>如果为 false:
	 * 		1.ROOT_DIR</p>
	 */
	public static boolean differentPath;
	public static int volume;

	/**
	 * 状态栏高度
	 */
	public static int statusBarHeight;
	
	/**
	 * 键盘透明度
	 */
	public static int keypadOpacity;
	
	
	private Context context;
	public SharedPreferences sp;
	private boolean bInited; //初始化标志
	
	private static MrpoidSettings instance;
	
	public static MrpoidSettings getInstance() {
		if(instance == null){
			instance = new MrpoidSettings();
		}
		return instance;
	}
	
	private MrpoidSettings() {
	}
	
	public void tempSave() {
		Editor e = sp.edit();

//		e.putInt(kKeypadMode, Keypad.getInstance().getMode());
//		e.putInt(kKeypadOpacity, Keypad.getInstance().getOpacity());
		e.putBoolean(kShowStatusBar, showStatusBar);
		e.putBoolean(kEnableKeyVirb, enableKeyVirb);
		e.putBoolean(kEnableAntiAtial, enableAntiAtial);
		e.putBoolean(kFullScreenEdit, fullScnEditor);
		e.putBoolean(kLimitInputLength, limitInputLength);
		
		e.commit();
	}
	
	public void tempRead() {
//		Keypad.getInstance().setMode(sp.getInt(kKeypadMode, 0));
//		Keypad.getInstance().setOpacity(sp.getInt(kKeypadOpacity, 0x80));

		showStatusBar = sp.getBoolean(kShowStatusBar, true);
		enableKeyVirb = sp.getBoolean(kEnableKeyVirb, true);
		enableAntiAtial = sp.getBoolean(kEnableAntiAtial, true);
		fullScnEditor = sp.getBoolean(kFullScreenEdit, false);
		limitInputLength = sp.getBoolean(kLimitInputLength, true);
	}

	public void otherSave() {
		// 需要注销，否则还会收到回调事件
		sp.unregisterOnSharedPreferenceChangeListener(this);
		
		Editor e = sp.edit();

//		e.putInt("keypadMode", Keypad.getInstance().getMode());
//		e.putInt("lrbX", lrbX);
//		e.putInt("lrbY", lrbY);
		e.putInt("keypadOpacity", keypadOpacity);
		e.putBoolean("notHintAdvSet", notHintAdvSet);
		
//		e.putBoolean(KEY_SHOW_STATUSBAR, showStatusBar);
		
		e.commit();
	}
	
	// apilog 选项
	private static final String[] logs = { 
		"enable_log_input", 
		"enable_log_file", 
		"enable_log_net", 
		"enable_log_mrplat", 
		"enable_log_timer", 
		"enable_log_fw", 
		"enable_log_mrprintf" 
	};
	
	public static final boolean B_DEF_CATCH_VOLUME_KEY = true;
	public static final boolean B_DEF_PLAT_DRAW_CHAR = false;
	public static final boolean B_DEF_MULTI_PATH = true;
	public static final String DEF_MEM_SIZE = "2";

	
	public static boolean getBooleanS(Context context, String key, boolean def) {
		SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
		return sp.getBoolean(key, def);
	}
	
	/**
	 * 初始化读取配置
	 * 
	 */
	public void init(Context c, Emulator e) {
		this.context = c;
		
		sp = PreferenceManager.getDefaultSharedPreferences(context);
		sp.registerOnSharedPreferenceChangeListener(this);
		
		
		Point scSize = EmuUtils.getScreenSize(context.getResources());
		


		dpadAtLeft = sp.getBoolean("dpadAtLeft", false);
		noKey = sp.getBoolean("noKey", false);
		enableAntiAtial = sp.getBoolean(kEnableAntiAtial, true);
		fullScnEditor = sp.getBoolean("fullScnEditor", false);
		catchVolumekey = sp.getBoolean("catchVolumekey", B_DEF_CATCH_VOLUME_KEY);
		volume = sp.getInt("volume", 100);
		enableSound = sp.getBoolean("enableSound", true);
		showMemInfo = sp.getBoolean("showMemInfo", false);
		differentPath = sp.getBoolean(kMultiPath, B_DEF_MULTI_PATH);
		showFloatButton = sp.getBoolean("showFloatButton", false);
		limitInputLength = sp.getBoolean(kLimitInputLength, true);
		enableKeyVirb = sp.getBoolean(kEnableKeyVirb, true);
		usePrivateDir = sp.getBoolean(kUsePrivateDir, false);
		sdPath = sp.getString(kSdcardPath, Emulator.SDCARD_ROOT);
		mythoadPath = sp.getString(kMythroadPath, Emulator.DEF_WORK_PATH);
		privateDir = context.getFilesDir().getAbsolutePath() + "/"; //以 / 结尾
		useFullDsm = sp.getBoolean(kUseFullDsm, false);
		
//		Keypad.getInstance().setMode(sp.getInt("keypadMode", 0));
		
//		lrbX = sp.getInt("lrbX", 1);
//		lrbY = sp.getInt("lrbY", 1);
		keypadOpacity = sp.getInt("keypadOpacity", 0xf0);
		notHintAdvSet = sp.getBoolean("notHintAdvSet", false);
		showFloatButton = false;
	}
	
	public void setNative() {
		Emulator.native_setIntOptions("enableSound", sp.getBoolean("enableSound", true)? 1 : 0);
		Emulator.native_setIntOptions("platdrawchar", sp.getBoolean("platdrawchar", B_DEF_PLAT_DRAW_CHAR)? 1 : 0);
		Emulator.native_setIntOptions("uselinuxTimer", 0);
		Emulator.native_setIntOptions("enableExram", 0);
		Emulator.native_setIntOptions("platform", 12); // linux
		Emulator.native_setIntOptions("uselinuxTimer", 1);
		Emulator.native_setIntOptions("memSize", Integer.valueOf(sp.getString(kVmMem, DEF_MEM_SIZE))); //虚拟机内存 单位 M
		
		String fact = SdkUtils.getOnlineString(context, "dsmFactory");
		if(fact != null)
			Emulator.native_setStringOptions("dsmFactory", fact);
		String type = SdkUtils.getOnlineString(context, "dsmType");
		if(fact != null)
			Emulator.native_setStringOptions("dsmType", type);
		
		// api log
		Emulator.native_setIntOptions("enableApilog", sp.getBoolean("enableApilog", false) ? 1 : 0);
		for (String key : logs) {
			Emulator.native_setIntOptions(key, sp.getBoolean(key, false) ? 1 : 0);
		}
	}
	
//	public SharedPreferences getSp() {
//		return sp;
//	}

	@Override
	public void onSharedPreferenceChanged(SharedPreferences sp, String key) {
		if (key.equals("scalingMode")) {
		} else if (key.equals("enableApilog")) {
			boolean b = sp.getBoolean(key, true);
			Emulator.native_setIntOptions(key, b ? 1 : 0);
			if (b) {
				for (String s : logs) {
					Emulator.native_setIntOptions(s, sp.getBoolean(s, false) ? 1 : 0);
				}
			}
		} else if (key.equals("enableSound")) {
			enableSound = sp.getBoolean(key, true);
		} else if (key.equals("volume")) {
			volume = sp.getInt("volume", 100);
		} else if (key.equals("orientation")) {

		} else if (key.equals(kShowStatusBar)) {
			showStatusBar = sp.getBoolean(key, false);
		}  else if (key.equals("showMemInfo")) {
			showMemInfo = sp.getBoolean(key, false);
		} else if (key.equals("noKey")) {
			noKey = sp.getBoolean(key, false);
		} else if (key.equals("dpadAtLeft")) {
			dpadAtLeft = sp.getBoolean(key, false);
		} else if (key.equals("fullScnEditor")) {
			fullScnEditor = sp.getBoolean(key, false);
		} else if (key.equals(kSdcardPath)) {
			sdPath = sp.getString(kSdcardPath, Emulator.SDCARD_ROOT);
			Emulator.getInstance().setVmRootPath(sdPath);
		} else if (key.equals(kMythroadPath)) {
			mythoadPath = sp.getString(kMythroadPath, Emulator.DEF_WORK_PATH);
			Emulator.getInstance().setVmWorkPath(mythoadPath);
		} else if (key.equals(kScreenSize)) {

			if (differentPath) {
			}
		} else if (key.equals("showFloatButton")) {
			showFloatButton = sp.getBoolean(key, false);
		} else if (key.equals("catchVolumekey")) {
			catchVolumekey = sp.getBoolean("catchVolumekey", B_DEF_CATCH_VOLUME_KEY);
		} else if (key.equals("platform")) {
//			emulator.setIntOptions(key, Integer.valueOf(sp.getString(key, "12")));
		} else if (key.equals(kVmMem)) {
			Emulator.native_setIntOptions(key, Integer.valueOf(sp.getString(key, DEF_MEM_SIZE)));
		} else if (key.equals(kMultiPath)) {
			differentPath = sp.getBoolean(key, B_DEF_MULTI_PATH);
			
			System.out.println("mutil = " + differentPath);
			
			Emulator.getInstance().initPath();
		} else if (key.equals(kLimitInputLength)) {
			limitInputLength = sp.getBoolean(kLimitInputLength, false);
		} else if (key.equals(kUseExram)) {
			Emulator.native_setIntOptions(kUseExram, sp.getBoolean(key, false) ? 1 : 0);
		} else if (key.equals(kEnableKeyVirb)) {
			enableKeyVirb = sp.getBoolean(kEnableKeyVirb, true);
		} else if (key.equals(kEnableAntiAtial)) {
			enableAntiAtial = sp.getBoolean(kEnableAntiAtial, true);
		} else if (key.equals(kUsePrivateDir)) {
			usePrivateDir = sp.getBoolean(kUsePrivateDir, false);
			
			System.out.println("private = " + usePrivateDir);
			
			Emulator.getInstance().initPath(); //重新初始化
		} else if (key.equals(kUseFullDsm)) {
			useFullDsm = sp.getBoolean(kUseFullDsm, false);
			
			UIUtils.toastMessage(context, "重启后生效！");
		} else {
			//应该全部改为使用 native_setStringOptions() 这样可以避免转型失败
			try { //转型失败 BUG
				Emulator.native_setIntOptions(key, sp.getBoolean(key, true) ? 1 : 0);
			} catch (Exception e) {
			}
		}
	}
	
	public static void setVmMemSize(int size) {
		Emulator.getInstance();
		Emulator.native_setIntOptions("memSize", size);
	}
	
	public static void setPlatDrawChar(boolean b) {
		Emulator.getInstance();
		Emulator.native_setIntOptions("platdrawchar", b? 1 : 0);
	}
	
	/**
	 * 模拟器主界面是否显示菜单
	 */
	public static void setShowMenu(boolean showMenu) {
		MrpoidSettings.showMenu = showMenu;
	}
}
