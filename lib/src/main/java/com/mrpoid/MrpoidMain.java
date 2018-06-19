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
package com.mrpoid;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Process;

import com.mrpoid.app.EmulatorService;
import com.mrpoid.core.EmuLog;
import com.mrpoid.apps.procmgr.AppProcess;
import com.mrpoid.apps.procmgr.AppProcessManager;
import com.mrpoid.apps.procmgr.AppProcessManager.RequestCallback;
import com.edroid.common.utils.UIUtils;

/**
 * MRP 运行管理器
 * 
 * @author Yichou 2013-12-19
 * 
 */
public final class MrpoidMain {
	static final String TAG = "MrpRunner";
	
	public static final String INTENT_KEY_ENTRY_MRP = "entryMrp";
	public static final String INTENT_ACTION_LAUNCH_MRP = "com.mrpoid.launchMrp";
	public static final String INTENT_KEY_ENTRY_ACTIVITY = "entryActivity";
	
	private static AppProcessManager manager;
	
	
	private static Context gContext;
	private static boolean gIsMainProcess = true;
	
	
	/**
	 * @return the gIsMainProcess
	 */
	public static boolean isMainProcess() {
		return gIsMainProcess;
	}
	
	public static Context getContext() {
		return gContext;
	}
	
	public static Resources getResources() {
		return gContext.getResources();
	}
	
	public static void init1(Context context) {
		EmuLog.d("", "EmulatorApplication create! pid=" + Process.myPid());
		
		gContext = context.getApplicationContext();
	}
	
	/**
	 * 启动虚拟机并运行 mrp
	 * 
	 * @param context
	 *            Activity 上下文，（后台运行的时候会返回到此 activity）
	 * 
	 * @param mrpPath
	 *            mrp 路径（绝对路径，或者相对于 mythroad 的路径）
	 * 
	 * @param defProcIndex
	 *            指定在哪个进程中运行 （0~5），若此进程被占用，系统分配一个空闲进程；-1
	 * 
	 * @param foce
	 *            如果 defProcIndex 被占用，强制使用
	 */
	public static void runMrp(final Activity context, final String mrpPath, int defProcIndex, boolean foce) {
		EmuLog.i(TAG, "startMrp(" + mrpPath + ")");
		
		init1(context);
		
		if (manager == null)
			manager = new AppProcessManager(context, "com.mrpoid.apps.AppService", 5);
		
		manager.requestIdleProcess(defProcIndex, foce, mrpPath, new RequestCallback() {
			
			@Override
			public void onSuccess(int procIndex, AppProcess process, boolean alreadyRun) {
				/**
				 * 如果 activity 正在运行，我们需要做的是把 activity 调到前台
				 * 
				 * 下面的启动方法可以达到效果
				 */
				
				
				/*if(alreadyRun) {
					
				} else */
				{
//					UIUtils.ToastMessage(context, "进程获取成功 " + procIndex);
					
					Intent intent = new Intent(EmulatorService.ACTION_STARTMRP);
					intent.setClassName(context, "com.mrpoid.apps.AppService" + procIndex);
					intent.putExtra(INTENT_KEY_ENTRY_MRP, mrpPath);
					intent.putExtra(INTENT_KEY_ENTRY_ACTIVITY, context.getClass().getName());
					
					context.getApplicationContext().startService(intent);
				}
			}
			
			@Override
			public void onFailure(String msg) {
				UIUtils.toastMessage(context, "进程获取失败102 " + msg);
			}
		});
	}
	
	public static void runMrp(Activity activity, String mrpPath) {
		runMrp(activity, mrpPath, -1, false);
	}
}
