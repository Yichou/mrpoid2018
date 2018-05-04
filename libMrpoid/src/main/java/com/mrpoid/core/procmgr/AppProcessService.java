package com.mrpoid.core.procmgr;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Handler.Callback;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.Process;
import android.os.RemoteException;

/**
 * 
 * @author Yichou 2013-12-19
 * 
 */
public class AppProcessService extends Service implements IMssageCodes, Callback {
	static final String TAG = "AppProcessService";
	
	private final Handler mHandler = new Handler(this);
	private final Messenger mLocalMessenger = new Messenger(mHandler);
	private int mProcIndex = -1;
	
	
	/**
	 * @return the mProcIndex
	 */
	public int getProcIndex() {
		return mProcIndex;
	}
	
	@Override
	public boolean handleMessage(Message msg) {
		switch (msg.what) {
		case MSG_HELLO: {
			mProcIndex = msg.arg1;
			sendMsg(msg.replyTo, msg.what, Process.myPid(), 0, null);
			break;
		}
			
		case MSG_EXIT: {
			AppProcessManager.log.i(TAG, "byby!");
			sendMsg(msg.replyTo, msg.what, Process.myPid(), 0, null);
			break;
		}
			
		default:
			sendMsg(msg.replyTo, msg.what, Process.myPid(), 0, null);
			return false;
		}
		
		return true;
	}
	
	private boolean sendMsg(Messenger remote, int what, int arg0, int arg1, Bundle bundle) {
		if (remote == null) {
			AppProcessManager.log.e(TAG, "who am I talking with? remote lost!");
			return false;
		}
		
		Message msg = new Message();
		msg.what = what;
		msg.arg1 = arg0;
		msg.arg2 = arg1;
		msg.obj = bundle;
		msg.replyTo = mLocalMessenger;
		
		try {
			remote.send(msg);
			return true;
		} catch (RemoteException e) {
		}
		
		return false;
	}
	
	@Override
	public IBinder onBind(Intent intent) {
		AppProcessManager.log.i(TAG, "onBind:" + intent + "pid=" + Process.myPid());
		
		return mLocalMessenger.getBinder();
	}
	
}
