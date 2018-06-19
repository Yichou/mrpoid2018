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

import android.app.Activity;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

/**
 * 模拟器日志打印工具
 * 
 * @author Yichou
 *
 */
public class EmuLog {
	public static boolean isShowLog;
	private static Toast m_toast;

	
	static {
		isShowLog = new File(Environment.getExternalStorageDirectory(), "debug-mrpoid").exists();
		isShowLog = true;
	}
	
	private EmuLog() {
	}
	
	public static void setShowLog(boolean isShow) {
		isShowLog = isShow;
	}

	public static void i(String tag, String msg) {
		if (isShowLog) {
			Log.i(tag, msg!=null? msg : "");
		}
	}

	public static void d(String tag, String msg) {
		if (isShowLog) {
			Log.d(tag, msg!=null? msg : "");
		}
	}

	public static void e(String tag, String msg) {
		Log.e(tag, msg!=null? msg : "");
	}

	public static void v(String tag, String msg) {
		Log.v(tag, msg!=null? msg : "");
	}

	public static void w(String tag, String msg) {
		if (isShowLog) {
			Log.w(tag, msg!=null? msg : "");
		}
	}

	public static void showScreenLog(final Activity activity, final String info) {
		if (isShowLog) {
			activity.runOnUiThread(new Runnable() {
				public void run() {
					if (m_toast == null) {
						//避免每次新建 Toast
						m_toast = Toast.makeText(activity, info, Toast.LENGTH_LONG);
					} else {
						m_toast.setText(info);
					}
					m_toast.show();
				}
			});
		}
	}
}
