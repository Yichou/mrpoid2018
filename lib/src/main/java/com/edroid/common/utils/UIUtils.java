package com.edroid.common.utils;

import android.content.Context;
import android.content.res.Resources;
import android.util.DisplayMetrics;
import android.widget.Toast;

public class UIUtils {

	public static void toastMessage(Context cont, String msg) {
		Toast.makeText(cont, msg, Toast.LENGTH_SHORT).show();
	}

	public static void toastMessage(Context cont, int msg) {
		Toast.makeText(cont, msg, Toast.LENGTH_SHORT).show();
	}

	public static void toastMessage(Context cont, String msg, int time) {
		Toast.makeText(cont, msg, time).show();
	}

	public static int px2dp(Resources r, int px) {
		DisplayMetrics metrics = r.getDisplayMetrics();
		return Math.round(px/metrics.density);
	}
	
	public static int dp2px(Resources r, int dp) {
		DisplayMetrics metrics = r.getDisplayMetrics();
		return Math.round(dp*metrics.density);
	}
	
	public static int revColor(int color) {
		int a = (color >> 24);
		int r = 0xFF - (color >> 16) & 0xFF;
		int g = 0xFF - (color >> 8) & 0xFF;
		int b = 0xFF - (color) & 0xFF;
		
		return ((a<<24)|(r<<16)|(g<<8)|b);
	}
}
