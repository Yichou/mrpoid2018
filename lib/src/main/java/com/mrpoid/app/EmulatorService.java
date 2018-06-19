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
package com.mrpoid.app;

import android.app.Notification;
import android.app.PendingIntent;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.support.v4.app.NotificationCompat;

import com.mrpoid.MrpoidMain;
import com.mrpoid.R;
import com.mrpoid.core.EmuLog;
import com.mrpoid.core.Emulator;
import com.mrpoid.apps.procmgr.AppProcessService;

/**
 * 模拟器 Service
 * 
 * @author Yichou 2013-9-6
 * 
 */
public class EmulatorService extends AppProcessService {
	static final String TAG = "EmulatorService";
	
	public static final String ACTION_STARTMRP = "com.mrpoid.actions.STARTMRP";
	public static final String ACTION_FOREGROUND = "com.mrpoid.actions.FOREGROUND";
	public static final String ACTION_BACKGROUND = "com.mrpoid.actions.BACKGROUND";
	
	
	@Override
	public void onCreate() {
		super.onCreate();
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		handleStart(intent);
		return START_STICKY_COMPATIBILITY;
	}
	
	private void handleStart(Intent intent) {
		if (ACTION_STARTMRP.equals(intent.getAction())) {
			Emulator emulator = Emulator.getInstance();
			emulator.setProcIndex(getProcIndex());
			
			String path = intent.getStringExtra(MrpoidMain.INTENT_KEY_ENTRY_MRP);
			if(emulator.isRunning()) {
				//如果已有mrp在运行，必须停止他
				if(emulator.getRunningMrpPath().equals(path)) {
					EmuLog.i(TAG, path + " already running!");
				} else {
					emulator.stop();
				}
			}
			
			Intent activityIntent = new Intent(MrpoidMain.INTENT_ACTION_LAUNCH_MRP);
			activityIntent.setClassName(this, EmulatorActivity.APP_ACTIVITY_NAME);
			activityIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			activityIntent.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
			activityIntent.putExtras(intent);
			getApplicationContext().startActivity(activityIntent);
		}
		else if (ACTION_FOREGROUND.equals(intent.getAction())) {
			// 建立新的Intent
			Intent notifyIntent = new Intent();
			notifyIntent.setClassName(this, EmulatorActivity.APP_ACTIVITY_NAME);
			notifyIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK
					| Intent.FLAG_ACTIVITY_SINGLE_TOP
					| Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
			
			// 建立PendingIntent作为设定递延执行的Activity
			PendingIntent appIntent = PendingIntent.getActivity(this, 0, notifyIntent, PendingIntent.FLAG_UPDATE_CURRENT);
			NotificationCompat.Builder builder = new NotificationCompat.Builder(this);
			builder.setContentTitle(Emulator.getInstance().getCurMrpAppName());
			builder.setContentText(getString(R.string.hint_click_to_back));
			builder.setContentIntent(appIntent);
			builder.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_notify));
			
			Notification n = builder.build();
			n.flags = Notification.FLAG_NO_CLEAR; // 不可清楚

			startForeground(R.drawable.ic_notify + getProcIndex(), n);
		} else if (ACTION_BACKGROUND.equals(intent.getAction())) {
			stopForeground(true);
		}
	}
	
	@Override
	public void onDestroy() {
		stopForeground(true);
	}
}
