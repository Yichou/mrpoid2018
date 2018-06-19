package com.mrpoid.apps.procmgr;

import android.os.Bundle;
import android.os.Handler;
import android.os.Handler.Callback;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.Process;
import android.os.RemoteException;

import com.mrpoid.core.EmuLog;
import com.edroid.common.utils.Logger;


/**
 * 
 * @author Yichou 2013-12-19
 *
 */
public class AppProcess implements Callback, IMssageCodes {
	public static final Logger log = Logger.create(EmuLog.isShowLog, "AppProcess");
	
	/**
	 * 进程生命监听
	 * 
	 * @author Yichou
	 *
	 */
	public static interface ProcessLifeListener {
		public void onHello();
		public void onResume();
		public void onGoodbye();
	}
	
	private Messenger mRemoteMessenger;
	private int mProcIndex;
	private final AppProcessManager manager;
	private long startTime; //启动时间
	private long lastActiveTime; //最后活动时间（根据此时间决定被杀优先级）
	private int pid;
	private ProcessLifeListener mLifeListener;

	private final Messenger mLocal = new Messenger(new Handler(this));
	
	
	public AppProcess(AppProcessManager manager, int procIndex, IBinder remote) {
		this.mProcIndex = procIndex;
		this.manager = manager;
		this.mRemoteMessenger = new Messenger(remote);
		
		sendMsg(MSG_HELLO, mProcIndex, 0, null);
	}
	
	private void reStart() {
		pid = 0;
//		synchronized (sProcStates) {
//			sProcStates[mProcIndex].state = ProcState.STATE_READY;
//			//TODO 重启进程
//		}
	}
	
	public void resume() {
		boolean ret = sendMsg(MSG_RESUME, 0, 0, null);
		
		//应该检测进程还在不在
		if(!ret) {
			reStart();
		}
	}
	
	public void exit(boolean kill) {
		sendMsg(MSG_EXIT, 0, 0, null);
		
		if(kill)
			manager.kill(mProcIndex);
	}
	
	public void killSelf() {
		Process.killProcess(pid);
	}

	private boolean sendMsg(int what, int arg1, int arg2, Bundle bundle) {
		Message msg = new Message();
		msg.what = what;
		msg.arg1 = arg1;
		msg.arg2 = arg2;
		msg.obj = bundle;
		msg.replyTo = mLocal;

		try {
			mRemoteMessenger.send(msg);
		} catch (RemoteException e) {
			return false;
		}
		
		return true;
	}

	@Override
	public boolean handleMessage(Message msg) {
		switch (msg.what) {
		case MSG_HELLO: { //回馈 pid packgeName
			startTime = System.currentTimeMillis();
			lastActiveTime = System.currentTimeMillis();
			pid = msg.arg1;
			
			log.i("proc" + mProcIndex + " hello fb from pid=" + pid );
			if(!manager.markAsRunning(mProcIndex))
				break;
			log.i("proc" + mProcIndex + " is running!");
			
			if(mLifeListener != null)
				mLifeListener.onHello();
			
			break;
		}
		
		case MSG_RESUME: {
			lastActiveTime = System.currentTimeMillis();
			log.i("proc" + mProcIndex + " resumed!");
			
			if(mLifeListener != null)
				mLifeListener.onResume();
			
			break;
		}
		
		case MSG_EXIT: {
			log.i("proc" + mProcIndex + " exited!");

			if(mLifeListener != null)
				mLifeListener.onGoodbye();
			break;
		}

		default:
			return false;
		}

		return true;
	}
	
	public int getProcIndex() {
		return mProcIndex;
	}
	
	public int getPid() {
		return pid;
	}
	
	public long getStartTime() {
		return startTime;
	}
	
	public long getLastActiveTime() {
		return lastActiveTime;
	}
}
