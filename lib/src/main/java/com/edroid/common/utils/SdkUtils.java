package com.edroid.common.utils;

import android.content.Context;

/**
 * 
 * SDK 接口封装类
 * 
 * @author Yichou
 * 
 */
public final class SdkUtils {
	
	public static String getOnlineString(Context context, String name) {
		return null;
		
//		String adsString = OnlineParams.getDefault().getString(name, null);
//
//		if (adsString == null || adsString.length() == 0
//				|| adsString.equals("null")) {
//			return null;
//		}
//
//		return adsString;
	}

	public static int getOnlineInt(Context context, String name, int def) {
		return 0;
		
//		return OnlineParams.getDefault().getInt(name, def);
	}
	
	public static void sendEvent(Context context, String id, String data) {
		UmengUtils.onEvent(context, id, data);
	}
	
	public static void onPause(Context context) {
		UmengUtils.onPause(context);
	}

	public static void onResume(Context context) {
		UmengUtils.onResume(context);
	}
}


