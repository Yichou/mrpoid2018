package com.edroid.common.utils;

import java.lang.Thread.UncaughtExceptionHandler;

import android.os.Handler;
import android.os.Handler.Callback;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;

/**
 * 一条工作线程，可以用来执行网络等耗时操作
 * 
 * 2016-12-18：死亡检测
 * 
 * @author Yichou 2015-11-18
 *
 */
public final class WorkThread implements Callback, UncaughtExceptionHandler {
	public static final Logger log = Logger.create("WorkThread");
	public static final int ALIVE_TIME_MAX = 45*1000;
	
	private static final int ALIVE_TIME_MAX_R = 40*1000;
	private static final Handler mUiHandler = new Handler(Looper.getMainLooper());

	
	private String name;
	private HandlerThread mThread;
	private Handler mHandler;
	private long t_last_alive = System.currentTimeMillis();
	
	
	
	@Override
	public void uncaughtException(Thread t, Throwable e) {
		log.error("crashed by " + e + ' ' + t.getId() + " " + t.getName(), e);
		e.printStackTrace();
	}
	
	public WorkThread() {
		this("default");
	}
	
	public WorkThread(String name) {
		this.name = name;
		
		create();
	}
	
	void create() {
		mThread = new HandlerThread(name);
		mThread.setUncaughtExceptionHandler(this);
		mThread.start();
		
		mHandler = new Handler(mThread.getLooper(), this);
		mHandler.sendEmptyMessage(0xf0000000);
	}
	
	/**
	 * 检查线程存活，如果死了重新创建线程
	 */
	public boolean checkAlive() {
		if(SystemClock.elapsedRealtime() - t_last_alive > ALIVE_TIME_MAX) {
			log.w(name + " thread die recreate");
			Thread.dumpStack();

			mThread.quit();
			mHandler.removeCallbacksAndMessages(null);
			
			create();
			return false;
		}
		
		return true;
	}
	
	@Override
	public boolean handleMessage(Message msg) {
		switch (msg.what) {
		case 0xf0000000:
			t_last_alive = SystemClock.elapsedRealtime();
			mHandler.sendEmptyMessageDelayed(0xf0000000, ALIVE_TIME_MAX_R);
			log.i(name + " alive " + t_last_alive);
			break;
		
		default:
			return false;
		}

		return true;
	}
	
	/**
	 * 任务完成回调，UI线程
	 * 
	 * @author Jainbin
	 *
	 */
	public interface IFinishCallback {
		public void onFinish(Object result);
	}

	/**
	 * 执行任务回调，任务线程
	 * 
	 * @author Jainbin
	 *
	 */
	public interface IDoCallback {
		public abstract Object onDo(Object doParam);
	}
	
	private class FinishRunable implements Runnable {
		private Object mData;
		private IFinishCallback mCallback;
		
		
		private FinishRunable(IFinishCallback cb, Object data) {
			mCallback = cb;
			mData = data;
		}
		
		@Override
		public void run() {
			mCallback.onFinish(mData);
		}
	}
	
	private class DoRunable implements Runnable {
		private IDoCallback mDoCallback;
		private IFinishCallback mFinishCallback;
		private Object mDoData;

		
		private DoRunable(IDoCallback cb, Object data, IFinishCallback cb2) {
			mDoCallback = cb;
			mDoData = data;
			mFinishCallback = cb2;
		}

		@Override
		public void run() {
			Object ret = mDoCallback.onDo(mDoData);
			mUiHandler.post(new FinishRunable(mFinishCallback, ret));
		}
	}
	
	public void postCb(IDoCallback cb, Object doData, IFinishCallback cb2) {
		mHandler.post(new DoRunable(cb, doData, cb2));
	}
	
	private class IRunable implements Runnable {
		Runnable taskRunnable;
		Runnable callbackRunnable;
		
		public IRunable(Runnable task, Runnable callback) {
			this.taskRunnable = task;
			this.callbackRunnable = callback;
		}
		
		@Override
		public void run() {
			taskRunnable.run();
			mUiHandler.post(callbackRunnable);
		}
	}
	
	public void postDelay(Runnable r, long delayMillis) {
		mHandler.postDelayed(r, delayMillis);
	}
	
	/**
	 * 发送一个带回调的任务
	 * 
	 * @param task 主任务，在 sdk 线程执行
	 * @param callback 回调器，主任务在 sdk 线程执行完毕后，在UI线程调此回调
	 */
	public void postEx(Runnable task, Runnable callback) {
		mHandler.post(new IRunable(task, callback));
	}
	
	public interface ITask {
		/**
		 * 执行回调，在任务线程
		 * 
		 * @param args 参数
		 * 
		 * @return 执行结果，作为 {@link #onResult(Object)} 参数
		 */
		public Object onDo(Object... args);
		
		/**
		 * 任务完成回调，在 UI 线程
		 * 
		 * @param ret {@link #onDo(Object...)} 的返回值
		 */
		public void onResult(Object ret);
	}
	
//	public static class CommonTask implements ITask {
//		private ProgressDialog mDialog;
//
//		public CommonTask showDialog(Activity activity, String msg) {
//			mDialog = new Prgre
//			dialog = Common.createLoadingDialog(activity);
//	        dialog.show();
//	        return this;
//	    }
//		
//		@Override
//		public Object onDo(Object... args) {
//			// TODO Auto-generated method stub
//			return null;
//		}
//
//		@Override
//		public void onResult(Object ret) {
//			// TODO Auto-generated method stub
//			
//		}
//		
//	}
	
	private class ITaskRunable implements Runnable {
		private ITask mTask;
		private Object[] mArgs;
		private Object mRet;
		
		private ITaskRunable(ITask task, Object... args) {
			this.mTask = task;
			this.mArgs = args;
		}

		@Override
		public void run() {
			try {
				mRet = mTask.onDo(mArgs);
			} catch (Exception e) {
				e.printStackTrace();
			}
			
			mUiHandler.post(new Runnable() {
				
				@Override
				public void run() {
					try {
						mTask.onResult(mRet);
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
			});
		}
	}
	
	/**
	 * 2016-1-5 by yichou
	 * 
	 * @param task 任务回调器
	 * @param args 参数
	 */
	public void postTask(ITask task, Object... args) {
		mHandler.post(new ITaskRunable(task, args));
	}

	public void postTask(ITask task) {
		mHandler.post(new ITaskRunable(task, (Object[])null));
	}
	
	public void postTaskDelay(int t, ITask task) {
		mHandler.postDelayed(new ITaskRunable(task, (Object[])null), t);
	}
	
	public void post(Runnable r) {
		mHandler.post(r);
	}

	public void postd(Runnable r, int delay) {
		mHandler.postDelayed(r, delay);
	}
	
	/**
	 * 发送一个任务到UI线程
	 * 
	 * @param r 任务
	 */
	public void postUi(Runnable r) {
		mUiHandler.post(r);
	}

	public void postUiDelay(Runnable r, int ms) {
		mUiHandler.postDelayed(r, ms);
	}
	
	public Handler getHandler() {
		return mHandler;
	}
	
	public Handler getUiHandler() {
		return mUiHandler;
	}
	
	public void init() {
	}
	
	public void exit() {
		mThread.quit();
		try {
			mThread.join(3000);
		} catch (InterruptedException e) {
		}
		log.i("join finish!");
		log.d("exited");
	}
	
	private static final Singleton<WorkThread> gDefault = new Singleton<WorkThread>() {

		@Override
		protected WorkThread create() {
			return new WorkThread();
		}
	};
	
	public static WorkThread getDefault() {
		return gDefault.get();
	}
}
