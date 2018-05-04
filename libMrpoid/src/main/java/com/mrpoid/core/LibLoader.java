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

import android.content.Context;

//import com.edroid.common.dl.DownloadListener;
//import com.edroid.common.dl.DownloadManager;
//import com.edroid.common.utils.HttpUtils;

/**
 * @author Yichou 2013-12-29
 *
 */
public class LibLoader {
//	private static final String TAG = "LibLoader";
//
//	private static final String SP_NAME = "libloader";
//
//	private static final String PLUGIN_NAME_SO = "mrpoid_so";
//	
//	public static final String API_URL_UPDATE_PLUGIN = "http://ad.iskyw.com/api/updatePlugin";
//	public static final int API_PROVER_UPDATE_PLUGIN = 1;
//	
//	
//	
//	private static  void setLastPluginName(Context context, String pluginKey, String value) {
//		SharedPreferences sp = context.getSharedPreferences(SP_NAME, Context.MODE_PRIVATE);
//		sp.edit().putString(pluginKey, value).commit();
//	}
//
//	private static  String getLastPluginName(Context context, String pluginKey) {
//		SharedPreferences sp = context.getSharedPreferences(SP_NAME, Context.MODE_PRIVATE);
//		return sp.getString(pluginKey, null);
//	}
//
//	
//	private static  void downloadPlugin(final Context context, final String name, String extName, final int ver, String url) {
//		final String saveName = name + "_" + ver + extName;
//		File saveFile =  new File(context.getDir("mylib", 0), saveName);
//		final String savePath = saveFile.getAbsolutePath();
//		
//		
//		if(saveFile.exists()) {
//			EmuLog.i(TAG, saveName + " local exist!");
//			setLastPluginName(context, name, savePath);
//			return;
//		}
//		
//		DownloadManager.startSync(context, savePath, url, new DownloadListener() {
//
//			@Override
//			public void onStart(long start, long total) {
//				EmuLog.i(TAG,  "start dl plugin from=" + start + ", total=" + total);
//			}
//
//			@Override
//			public void onFinish() {
//				EmuLog.i(TAG, "dl finish! saveTo:" + savePath);
//				
//				setLastPluginName(context, name, savePath);
//			}
//
//			@Override
//			public void onError(String msg, Exception e) {
//				EmuLog.i(TAG, "dl error" + msg);
//			}
//		});
//	}
//	
//	private static void querryServer(Context context, String name, String extName, int curVer) {
//		final String MSG = "querryServer(" + name + "," + curVer + ") ";
//
//		String errMsg = "";
//
//		try {
//			StringBuilder sb = new StringBuilder(128);
//			
//			sb.append(API_URL_UPDATE_PLUGIN).append('?')
//			  .append("proVer=").append(API_PROVER_UPDATE_PLUGIN)
//			  .append("&name=").append(name)
//			  .append("&ver=").append(curVer);
//			
//			String dat = sb.toString();
//			
//			EmuLog.i(TAG, "updatePlugin dat=" + dat);
//			
//			String retString = HttpUtils.get(context, dat);
//			
//			EmuLog.i(TAG, "updatePlugin ret=" + retString);
//			
//			if(retString != null) {
//				JSONObject jsonObject = new JSONObject(retString);
//				int code = jsonObject.getInt("code");
//				if(code == 200) {
//					downloadPlugin(context, 
//							name, extName,
//							jsonObject.getInt("ver"),
//							jsonObject.getString("url"));
//					
//					return;
//				} else if (code == 202 || code == 201){
//					EmuLog.i(TAG, "checkUpdate no now!");
//
//					return;
//				} else {
//					errMsg = "unknow error code=" + code;
//				}
//			}
//		} catch (Exception e) {
//			errMsg = "querry server error! " + e.getClass().getName() + " " + e.getMessage();
//		}
//		
//		EmuLog.e(MSG, "querryServer fail! " + errMsg);
//	}
//	
//	private static int getVerFromName(String name) {
//		int i = name.lastIndexOf('_');
//		if(i == -1)
//			return 0;
//		
//		int j = name.lastIndexOf('.');
//		if(j == -1)
//			j = name.length();
//		
//		try {
//			return Integer.parseInt(name.substring(i+1, j));
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		
//		return 0;
//	}
//	
//	private static void checkUpdate(final Context context, final int curVer) {
//		new Thread(new Runnable() {
//			
//			@Override
//			public void run() {
//				querryServer(context, PLUGIN_NAME_SO, ".so", curVer);
//			}
//		}).start();
//	}
	
	public static void load(Context context) {
		System.loadLibrary("mrpoid");
		
//		String path = getLastPluginName(context, PLUGIN_NAME_SO);
//		
//		if(path == null) {
//			System.loadLibrary("mrpoid");
//			checkUpdate(context, 0);
//		} else {
//			System.load(path);
//			checkUpdate(context,  getVerFromName(path));
//		}
	}
}
