package com.edroid.common.dl;

import java.io.File;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map.Entry;

import android.content.Context;
import android.os.Handler;

import com.edroid.common.utils.FileUtils;
import com.edroid.common.utils.FileUtils.MyFileLock;

/**
 * 管理所有下载
 * 
 * @author Yichou 2013-8-9
 *
 */
public final class DownloadManager {
	static final Object mSync = new Object();
	
	public static int MAX_DOWNLOAD_TASK = -1;
	
	private static final HashMap<String, Downloader> hashMap = new HashMap<String, Downloader>();
	private static final LinkedList<Downloader> queue = new LinkedList<Downloader>();
	
	
	protected static boolean add(Downloader downloader) {
		synchronized (mSync) {
			if(hashMap.containsKey(downloader.getFileSavePath()))
				return false;
			
			hashMap.put(downloader.getFileSavePath(), downloader);
			return true;
		}
	}

	protected static Downloader get(String path) {
		synchronized (mSync) {
			return hashMap.get(path);
		}
	}
	
	protected static void remove(Downloader downloader) {
		synchronized (mSync) {
			hashMap.remove(downloader.getFileSavePath());
		}
		checkQueue();
	}
	
	public static int runningCount() {
		return hashMap.size();
	}
	
	public static void stopAll() {
		synchronized (mSync) {
			for(Entry<String, Downloader> e : hashMap.entrySet()){
				e.getValue().cancel();
			}
		}
	}
	
	public static boolean isIdle() {
		synchronized (mSync) {
			return hashMap.isEmpty();
		}
	}
	
	public static boolean has(String fileSavePath) {
		synchronized (mSync) {
			return hashMap.containsKey(fileSavePath);
		}
	}
	
	public static boolean isTempDownload(String fileSavePath) {
		// 临时文件
		return new File(fileSavePath + ".tmp").exists();
	}
	
	/**
	 * 检查某个文件是否已经在下载了
	 * 
	 * @param fileSavePath
	 * @return
	 */
	public static boolean isFileDownloadIng(String fileSavePath) {
		if(has(fileSavePath)) //进程内
			return true;
			
		File lckFile = new File(fileSavePath + ".lck"); //进程间
		MyFileLock lock = FileUtils.tryFileLock(lckFile);
		if(lock == null)
			return true;
		FileUtils.releaseFileLock(lock);
		
		return false;
	}
	
	/**
	 * 添加到下载队列，不立即开始
	 * 
	 * @param downloader
	 */
	public static void addToQueue(Downloader downloader) {
		synchronized (queue) {
			queue.add(downloader);
		}
	}
	
	/**
	 * 正在等待中。。。
	 * 
	 * @param path
	 * @return
	 */
	public static boolean isInQueue(String path) {
		if (!queue.isEmpty()) {
			for (Downloader d : queue) {
				if (d.getFileSavePath().equals(path))
					return true;
			}
		}
		return false;
	}
	
	private static void checkQueue() {
		if(!queue.isEmpty() && runningCount() < MAX_DOWNLOAD_TASK) {
			synchronized (queue) {
				queue.removeFirst().start();
			}
		}
	}
	
	/**
	 * 启动一个下载（异步）
	 */
	public static Downloader startAsync(Context context, String localPath, String serverPath, IDownloadListener listener) {
		return start(context, localPath, serverPath, null, true, listener);
	}

	public static Downloader startAsync(Context context, String localPath, String serverPath, Handler userHandler, IDownloadListener listener) {
		return start(context, localPath, serverPath, userHandler, true, listener);
	}
	
	public static Downloader startSync(Context context, 
			String localPath, String serverPath,
			IDownloadListener listener) {
		return start(context, localPath, serverPath, null, false, listener);
	}

	public static Downloader startSync(Context context, 
			String localPath, String serverPath,
			Handler userHandler,
			IDownloadListener listener) {
		return start(context, localPath, serverPath, userHandler, false, listener);
	}
	
	/**
	 * 启动一个下载
	 * 
	 * @param context
	 * @param localPath
	 *            下载后文件存储位置
	 * @param serverPath
	 *            下载文件的服务器地址
	 * @param minSdRemain
	 *            最小SD卡剩余容量 mb
	 * @param maxFileReaminSize
	 *            剩余字节 < 此值才下载，-1 无限制
	 * @param listener
	 *            回调
	 * @param newThread
	 *            true 开启一个线程去下载
	 * 
	 * @return
	 */
	public static Downloader start(Context context, 
			String localPath, String serverPath, 
			Handler userHandler,
			boolean async,
			IDownloadListener listener) {
		if(isInQueue(localPath))
			return null;
		
		Downloader downloader = null;
		
		downloader = get(localPath);
		if(downloader != null)
			return downloader;
		
		downloader = new Downloader(context, localPath, serverPath, userHandler, listener);
		downloader.setAsync(async);
		
		if(MAX_DOWNLOAD_TASK == -1 || runningCount() < MAX_DOWNLOAD_TASK) {
			downloader.start();
		} else {
			downloader.callOnWait();
			addToQueue(downloader);
		}
		
		return downloader;
	}
}
