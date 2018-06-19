package com.edroid.common.utils;

import java.lang.reflect.Method;

import android.content.Context;


/**
 * 
 * @author YYichou 2013-10-4
 *
 */
public final class UmengUtils {

	public static void updateOnlineParams(Context arg0) {
		try {
			Class<?> clazz = Class.forName("com.umeng.analytics.MobclickAgent");
			
			Method method = clazz.getDeclaredMethod("updateOnlineParams", Context.class);
			method.setAccessible(true);
			method.invoke(null, arg0);
		} catch (Exception e) {
		}
	}
	
	public static String getStringOL(Context context, String name) {
		String adsString = getConfigParams(context, name);

		if (adsString == null || adsString.length() == 0
				|| adsString.equals("null")) {
			return null;
		}

		return adsString;
	}
	
	public static String getConfigParams(Context arg0, String arg1) {
		try {
			Class<?> clazz = Class.forName("com.umeng.analytics.MobclickAgent");
			
			Method method = clazz.getDeclaredMethod("getConfigParams",
					Context.class, String.class);
			method.setAccessible(true);
			return (String)method.invoke(null, 
					arg0, arg1);
		} catch (Exception e) {
		}
		
		return null;
	}

	public static void onEvent(Context arg0, String arg1) {
		onEvent(arg0, arg1, "null"); //第二个参数为空发现报不上！
	}
	
	public static void onEvent(Context arg0, String arg1, String arg2) {
		try {
			Class<?> clazz = Class.forName("com.umeng.analytics.MobclickAgent");
			
			Method method = clazz.getDeclaredMethod("onEvent", Context.class, String.class, String.class);
			method.setAccessible(true);
			method.invoke(null, arg0, arg1, arg2);
		} catch (Exception e) {
		}
	}
	
	public static void onPause(Context arg0) {
		try {
			Class<?> clazz = Class.forName("com.umeng.analytics.MobclickAgent");
			
			Method method = clazz.getDeclaredMethod("onPause", Context.class);
			method.setAccessible(true);
			method.invoke(null, arg0);
		} catch (Exception e) {
		}
	}

	public static void onResume(Context arg0) {
		try {
			Class<?> clazz = Class.forName("com.umeng.analytics.MobclickAgent");
			
			Method method = clazz.getDeclaredMethod("onResume", Context.class);
			method.setAccessible(true);
			method.invoke(null, arg0);
		} catch (Exception e) {
		}
	}
}
