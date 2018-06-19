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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Point;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Environment;
import android.telephony.TelephonyManager;
import android.util.DisplayMetrics;
import android.util.Log;

/**
 * 2012/10/9
 * @author JianbinZhu
 *
 */
public class EmuUtils {
	public static int getNetworkType(Context context) {
		ConnectivityManager connectivity = (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
		
		if (connectivity != null) { 
			// 获取网络连接管理的对象
			NetworkInfo info = connectivity.getActiveNetworkInfo();

			if (info != null && info.isConnected()) {
				// 判断当前网络是否已经连接
				if (info.getState() == NetworkInfo.State.CONNECTED) {
					if(info.getType() == ConnectivityManager.TYPE_WIFI){
						Log.d("", "getNetworkType is WIFI.");
						return MrDefines.NETTYPE_WIFI;
					}else if(info.getType() == ConnectivityManager.TYPE_MOBILE){
						String extInfo = info.getExtraInfo();
						if(extInfo != null && extInfo.toLowerCase().contains("wap")){
							Log.d("", "getNetworkType is WAP."); 
							return MrDefines.NETTYPE_CMWAP;
						}else {
							Log.d("", "getNetworkType is NET.");
							return MrDefines.NETTYPE_CMNET;
						}
					}
				}
			}
		}
		
		return MrDefines.NETTYPE_UNKNOW;
	}
	
	public static int getNetworkID(Context context) {
		String str = ((TelephonyManager) context.getSystemService("phone"))
				.getSubscriberId();

		if (str == null)
			return MrDefines.MR_NET_ID_MOBILE; //返回 NULL 会导致未插卡不能运行

		if ((str.regionMatches(0, "46000", 0, 5))
				|| (str.regionMatches(0, "46002", 0, 5))
				|| (str.regionMatches(0, "46007", 0, 5)))
			return MrDefines.MR_NET_ID_MOBILE;
		else if (str.regionMatches(0, "46001", 0, 5))
			return MrDefines.MR_NET_ID_CN;
		else if (str.regionMatches(0, "46003", 0, 5))
			return MrDefines.MR_NET_ID_CDMA;
		else
			return MrDefines.MR_NET_ID_MOBILE; //返回 NULL 会导致未插卡不能运行
	}
	
	public static boolean checkSDCARD() {
		String sdState = Environment.getExternalStorageState(); // 判断sd卡是否存在
		return sdState.equals(android.os.Environment.MEDIA_MOUNTED);
	}
	
	public static void checkRes(AssetManager manager, String src, String dstPath, String dstName) 
		throws IOException {
		File dir = new File(dstPath);
		if (!dir.exists())
			dir.mkdirs();

		File file = new File(dstPath, dstName);
		if (!file.exists()) {
			InputStream is = null;
			FileOutputStream fos = null;

			is = manager.open(src);
			fos = new FileOutputStream(file);

			byte[] buf = new byte[is.available()];
			is.read(buf);
			fos.write(buf);
			is.close();
			fos.close();
		}
	}
	
	public static boolean bitmapToFile(Bitmap bitmap, File path) {
		FileOutputStream fos = null;
		try {
			fos = new FileOutputStream(path);
			// 将 bitmap 压缩成其他格式的图片数据
			bitmap.compress(CompressFormat.PNG, 50, fos);
			return true;
		} catch (FileNotFoundException e1) {
		}finally {
			try {
				if(fos != null) fos.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		return false;
	}
	
	public static Point getScreenSize(Resources res) {
		DisplayMetrics metrics = res.getDisplayMetrics();
	    return new Point(metrics.widthPixels, metrics.heightPixels);
	}
	
	public static String getDateTimeNow() {
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy/mm/dd HH:mm:ss", Locale.CHINA);
		return sdf.format(new Date());
	}
	
	public static String getTimeNow() {
		SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss", Locale.CHINA);
		return sdf.format(new Date());
	}
	
	public static int getDayOfYear() {
		return Calendar.getInstance(Locale.CHINA).get(Calendar.DAY_OF_YEAR);
	}
	
}
