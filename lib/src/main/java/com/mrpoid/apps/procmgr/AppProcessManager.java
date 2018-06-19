package com.mrpoid.apps.procmgr;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;

import com.mrpoid.core.EmuLog;
import com.edroid.common.utils.Logger;


/**
 * 多开管理器
 * 
 * @author Yichou 2013-12-19
 *
 */
public class AppProcessManager {
	public static final Logger log = Logger.create(EmuLog.isShowLog, "AppProcManager");
	
	private static enum ItemState {
		IDLE,
		WAITING,
		CONNECTED,
		RUNNING
	}
	
	private static final class Item {
		ItemState state;
		long readyTime; //开始等待连接的时刻
		long connectedTime; //连接上的时刻
		String mark;
		ServiceConnection conn;
		AppProcess app;
		
		public Item() {
			reset();
		}
		
		public void reset() {
			this.mark = null;
			this.app = null;
			this.readyTime = 0;
			this.state = ItemState.IDLE;
		}
		
		@Override
		public String toString() {
			return mark;
		}
	}
	
	
	private final int MAX_PROC_COUNT;
	private final Item[] mProcList;
	private final String SERVICE_NAME_PERFIX;
	private Context mContext;

	
	public AppProcessManager(Context context, String serviceNamePerfix, int maxCount) {
		mContext = context.getApplicationContext();
		SERVICE_NAME_PERFIX = serviceNamePerfix;
		MAX_PROC_COUNT = maxCount;
		mProcList = new Item[MAX_PROC_COUNT];

		for(int i=0; i<MAX_PROC_COUNT; i++)
			mProcList[i] = new Item();
	}
	
	private void binProc(int procIndex, String mark, RequestCallback cb) {
		String service = SERVICE_NAME_PERFIX + Integer.valueOf(procIndex);
		MyConnetion connection = new MyConnetion(procIndex, cb);
		
		Intent intent = new Intent();
		intent.setClassName(mContext, service);
		//使用全局 context
		mContext.bindService(intent, connection, Service.BIND_AUTO_CREATE);
		
		synchronized (mProcList) {
			mProcList[procIndex].conn = connection;
			mProcList[procIndex].mark = mark;
			mProcList[procIndex].readyTime = System.currentTimeMillis();
			mProcList[procIndex].state = ItemState.WAITING;
			
			log.i("proc" + procIndex + " wait service connection cb!");
		}
	}
	
	private void markAsConnected(int index, AppProcess process) {
		synchronized (mProcList) {
			mProcList[index].app = process;
			mProcList[index].connectedTime = System.currentTimeMillis();
			mProcList[index].state = ItemState.CONNECTED;
			
			log.i("proc" + index + " connected time=" 
					+ (System.currentTimeMillis() - mProcList[index].readyTime));
		}
	}
	
	protected boolean markAsRunning(int procIndex) {
		synchronized (mProcList) {
			if(mProcList[procIndex].state == ItemState.RUNNING) { //被人抢先了一步，你回去吧
				log.e("proc " + procIndex + " has Preemptived by" + mProcList[procIndex].toString());
				return false;
			}
			
			mProcList[procIndex].state = ItemState.RUNNING;

			log.i("proc" + procIndex + " running time=" 
					+ (System.currentTimeMillis() - mProcList[procIndex].connectedTime));
		}
		
		return true;
	}
	
	
	private void unbindProc(int procIndex) {
		synchronized (mProcList) {
			if (mProcList[procIndex].conn != null) {
				mContext.unbindService(mProcList[procIndex].conn);
				mProcList[procIndex].conn = null;
				
				log.i("unbind proc" + procIndex);
			}
		}
	}
	
	private void resetProc(int procIndex) {
		synchronized (mProcList) {
			unbindProc(procIndex);
			mProcList[procIndex].reset();
		}
	}

	private void exitProc(int procIndex) {
		synchronized (mProcList) {
			unbindProc(procIndex);
			
			if(mProcList[procIndex].app != null) {
				mProcList[procIndex].app.exit(false);
				mProcList[procIndex].app = null;
			}
			
			mProcList[procIndex].reset();
			
			log.i("proc" + procIndex + " exited!");
		}
	}
	
	private void killProc(int procIndex) {
		synchronized (mProcList) {
			unbindProc(procIndex);

			if(mProcList[procIndex].app != null) {
				mProcList[procIndex].app.exit(true);
				mProcList[procIndex].app = null;
			}

			mProcList[procIndex].reset();

			log.i("proc" + procIndex + " killed!");
		}
	}
	
	public Context getContext() {
		return mContext;
	}
	
	private int getIdleIndex(int defProcIndex, boolean foce) {
		synchronized (mProcList) {
			/**
			 * if procIndex specified we shoud check if we can!
			 */
			if(defProcIndex != -1) {
				if(mProcList[defProcIndex].state == ItemState.IDLE) {
					return defProcIndex;
				}
				else if(mProcList[defProcIndex].state == ItemState.WAITING && foce) {
					log.w("foce exit waiting proc" + defProcIndex);
					resetProc(defProcIndex);
					
					return defProcIndex;
				}
				else if (mProcList[defProcIndex].state == ItemState.RUNNING && foce) {
					log.w("foce exit waiting proc" + defProcIndex);
					exitProc(defProcIndex);
					
					return defProcIndex;
				}
			}
			
			for (int i = 0; i < MAX_PROC_COUNT; i++) {
				if (mProcList[i].state == ItemState.IDLE)
					return i;
			}

			// 1.还处于 ready 状态又运行了一个应用？出问题了吧干掉
			for (int i = 0; i < MAX_PROC_COUNT; i++) {
				if (mProcList[i].state == ItemState.WAITING) {
					mProcList[i].reset();
					log.w("use ready proc" + i);
					return i;
				}
			}

			// 2.正在运行，长时间未激活
			long time = System.currentTimeMillis();
			int id = -1;
			for (int i = 0; i < MAX_PROC_COUNT; i++) {
				if (mProcList[i].state == ItemState.RUNNING) {
					if (mProcList[i].app.getLastActiveTime() < time) {
						id = i;
						time = mProcList[i].app.getLastActiveTime();
					}
				}
			}
			
			if (id != -1) {
				log.w("exit running proc" + id);
				exitProc(id);
				return id;
			}
		}
		
		return -1;
	}
	
	/**
	 * 从正在运行列表取
	 * 
	 * @param mark
	 * @return
	 */
	private int checkRuning(String mark) {
		synchronized (mProcList) {
			for (int i = 0; i < MAX_PROC_COUNT; i++)
				if (mark.equals(mProcList[i].mark))
					return i;
		}
		
		return -1;
	}
	
	public static interface RequestCallback {
		public void onSuccess(int procIndex, AppProcess process, boolean alreadyRun);
		
		public void onFailure(String msg);
	}

	
	private final class MyConnetion implements ServiceConnection {
		int procIndex;
		RequestCallback callback;

		
		public MyConnetion(int procIndex, RequestCallback callback) {
			this.procIndex = procIndex;
			this.callback = callback;
		}
		
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			log.d("proc" + procIndex + " onServiceConnected!");
			
			AppProcess process = new AppProcess(AppProcessManager.this, procIndex, service);
			markAsConnected(procIndex, process);

			callback.onSuccess(procIndex, process, false);
		}

		/**
		 * 进程意外被杀，才会回调此方法
		 */
		@Override
		public void onServiceDisconnected(ComponentName name) {
			log.d("proc" + procIndex + " onServiceDisconnected!");

			unbindProc(procIndex);
			resetProc(procIndex);
		}
	}
	
	/**
	 * 在独立进程运行 apk
	 * 
	 * @param context
	 * @param apkPath
	 * 
	 * @return apk 运行 id
	 */
	public synchronized void requestIdleProcess(int defProcIndex, boolean foce, String mark, RequestCallback cb) {
		int procIndex = checkRuning(mark);
		
		/**
		 * if is already running we dont't case defProcIndex
		 */
		if(procIndex != -1) { //is running
			if(mProcList[procIndex].state == ItemState.RUNNING) {
				mProcList[procIndex].app.resume();
				cb.onSuccess(procIndex, mProcList[procIndex].app, true);
				
				return ; //直接返回 running proc
			} 
			else if(mProcList[procIndex].state == ItemState.WAITING) { //怎么还在 waiting ?
				if(System.currentTimeMillis() - mProcList[procIndex].readyTime > 10*1000) { //wait most 10 seconds
					resetProc(procIndex);
				} else {
					procIndex = -1; //开一个新的
				}
			} 
			else if(mProcList[procIndex].state == ItemState.IDLE) {
				resetProc(procIndex);
			}
		} 
		
		if(procIndex == -1) {
			procIndex = getIdleIndex(defProcIndex, foce);
			
			if(procIndex == -1) {
				final String MSG = "no idle process now!";
				log.w(MSG);
				cb.onFailure(MSG);
				
				return ;
			}
		}
		
		binProc(procIndex, mark, cb);
	}
	
	public void kill(int procIndex) {
		killProc(procIndex);
	}
}
