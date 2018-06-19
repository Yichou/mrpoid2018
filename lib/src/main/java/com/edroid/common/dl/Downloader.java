package com.edroid.common.dl;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.Proxy;
import java.net.SocketTimeoutException;
import java.net.URL;

import android.content.Context;
import android.os.Handler;

import com.edroid.common.utils.FileUtils;
import com.edroid.common.utils.FileUtils.MyFileLock;
import com.edroid.common.utils.HttpUtils;
import com.edroid.common.utils.Logger;
import com.edroid.common.utils.PhoneUtils;

/**
 * 下载器
 * 
 * @author Yichou 2013-9-9
 * 
 */
public class Downloader implements Runnable {
	static final String TAG = "Downloader";
	public static final Logger log = Logger.create("Downloader");

	private static final int RW_BUF_SIZE = 32 * 1024;
	private static final int MAX_RETRY_TIMES = 5;

	private String fileSavePath;
	private String urlString;
	private IDownloadListener listener;
	private boolean canceled = false;
	private Context mContext;
	private Handler mUserHandler;
	private boolean async = true; //异步下载
	private int retryTimes = 0;
	
	
	/**
	 * 仅在 wifi 下载
	 */
	public boolean wifiOnly = true;

	
	public String getFileSavePath() {
		return fileSavePath;
	}

	public String getUrlString() {
		return urlString;
	}

	public Downloader(Context context, String fileSavePath, String urlString,
			IDownloadListener listener) {
		this(context, fileSavePath, urlString, null, listener);
	}

	public Downloader(Context context, String fileSavePath, String urlString,
			Handler userHandler, IDownloadListener listener) {
		
		if(fileSavePath == null) {
			throw new NullPointerException("file save path can't be null!");
		}
		
		this.fileSavePath = fileSavePath;
		this.urlString = urlString;
		this.listener = listener;
		this.mContext = context.getApplicationContext();
		this.mUserHandler = userHandler;
	}

	public void setUserHandler(Handler userHandler) {
		this.mUserHandler = userHandler;
	}
	
	public void setAsync(boolean async) {
		this.async = async;
	}

	public Downloader setDownloadListener(IDownloadListener listener) {
		this.listener = listener;
		return this;
	}

	/**
	 * 取消下载
	 */
	public void cancel() {
		canceled = true;
	}

	/**
	 * 获取一个 用于下载的 HttpURLConnection
	 * 
	 * @param urlString
	 *            地址
	 * @param startPos
	 *            起始位置
	 * @return
	 */
	public HttpURLConnection getDownloadConnection(String urlString, long startPos) {
		try {
			URL url = new URL(urlString);

			HttpURLConnection conn;
				conn = (HttpURLConnection) url.openConnection();

			conn.setAllowUserInteraction(true);
			conn.setRequestProperty("User-Agent", "NetFox");
			conn.setReadTimeout(5 * 1000); // 设置超时时间
			conn.setConnectTimeout(5 * 1000);
			conn.setRequestMethod("GET");
			conn.addRequestProperty("Range", "bytes=" + startPos + "-");
			// conn.addRequestProperty("Connection" , "Kepp-Alive");

			return conn;
		} catch (MalformedURLException e) {
		} catch (ProtocolException e) {
		} catch (IOException e) {
		}

		return null;
	}

	private void callOnStart(final long start, final long total) {
		if (listener != null) {
			if (mUserHandler == null)
				listener.onStart(start, total);
			else
				mUserHandler.post(new Runnable() {

					@Override
					public void run() {
						listener.onStart(start, total);
					}
				});
		} else {
		}
	}

	private void callOnProgress(final long cur, final byte prog) {
		if (listener != null) {
			if (mUserHandler == null)
				listener.onProgress(cur, prog);
			else
				mUserHandler.post(new Runnable() {

					@Override
					public void run() {
						listener.onProgress(cur, prog);
					}
				});
		}
	}

	private void callOnCancel() {
		if (listener != null) {
			if (mUserHandler == null)
				listener.onCancel();
			else
				mUserHandler.post(new Runnable() {

					@Override
					public void run() {
						listener.onCancel();
					}
				});
		}
	}

	private void callOnFinish() {
		if (listener != null) {
			if (mUserHandler == null)
				listener.onFinish();
			else
				mUserHandler.post(new Runnable() {

					@Override
					public void run() {
						listener.onFinish();
					}
				});
		}
	}

	private void callOnError(final String msg, Exception e) {
		final Exception e2 = e!=null? e : new RuntimeException(msg);
		log.e(msg);
		
		if (listener != null) {
			if (mUserHandler == null)
				listener.onError(msg, e2);
			else
				mUserHandler.post(new Runnable() {

					@Override
					public void run() {
						listener.onError(msg, e2);
					}
				});
		}
	}
	
	protected void callOnWait() {
		if (listener != null) {
			if (mUserHandler == null)
				listener.onWait();
			else
				mUserHandler.post(new Runnable() {

					@Override
					public void run() {
						listener.onWait();
					}
				});
		}
	}

	protected void callOnClose() {
		if (listener != null) {
			if (mUserHandler == null)
				listener.onClose();
			else
				mUserHandler.post(new Runnable() {
					
					@Override
					public void run() {
						listener.onClose();
					}
				});
		}
	}
	
	private boolean precheckEnv() {
		if(!PhoneUtils.isNetAvailable(mContext)) {
			log.e("net not available!");
			callOnError("net not available!", null);
			return false;
		}
		
		return true;
	}
	
	/**
	 * 新开一个线程跑下载
	 */
	public void startAsync() {
		if(!precheckEnv())
			return;
		new Thread(this).start();
	}
	
	public void start() {
		if(!precheckEnv())
			return;
		
		if(async)
			new Thread(this).start();
		else
			run();
	}
	
	private boolean retry = false;
	
	@Override
	public void run() {
		InputStream is = null;
		RandomAccessFile raf = null;
		HttpURLConnection conn = null;
		MyFileLock lock = null;
		File lckFile = null;
		
		if(fileSavePath == null) {
			log.e("file save path can't be null!");
			return;
		}

		// 检查并创建父目录
		FileUtils.checkParentPath(fileSavePath);
		
		//重试的时候资源没有关闭，以提高效率
		if(!retry) {
			// 文件加锁
			lckFile = new File(fileSavePath + ".lck");
			lock = FileUtils.tryFileLock(lckFile);
			if (lock == null)
				return;
			
			if(!DownloadManager.add(this)) {
				log.e(fileSavePath + " 已经在下载 ！");
				return;
			}
		}
		retry = false;

		try {
			long startPos = 0;

			File file2 = new File(fileSavePath);
			if (file2.exists()) {
				if(listener == null) {
					// 文件已存在，
					return;
				}
				
				//同步的调用监听器，立马获取返回值进行处理
				int ret = listener.onExist();
				if(ret == 0) {
					// 文件已存在，直接回调成功
					callOnFinish();
					return;
				} else if (ret == 1) {
					if (file2.isDirectory()) // 如果文件存在，删除重下
						FileUtils.removeDir(file2);
					else
						file2.delete();
				}
			} else { // 目标文件不存在，下载到零时文件
				file2 = new File(fileSavePath + ".tmp"); // 临时文件
				if (file2.exists()) {
					if (file2.isFile()) {
						startPos = file2.length(); // 临时文件存在，获取其长度，续传
					} else { // 存在，但不是文件，不会鬼吧？管他是什么删掉他
						startPos = 0;
						FileUtils.removeDir(file2);
					}
				}
			}

			raf = new RandomAccessFile(file2, "rw");
			raf.seek(startPos);

			conn = getDownloadConnection(urlString, startPos);
			if (conn == null) {
				callOnError("open Connection FAIL!", null);
				return;
			}

			long remainLength = conn.getContentLength();
			if (remainLength == -1) {
				callOnError("get ContentLength FAIL!", null);
				return;
			}
			long mTotalLength = startPos + remainLength;
			long sdLeft = FileUtils.getSDLeftSpace();
			if(remainLength + 1*1024*1024 > sdLeft) {
				callOnError("sd not enough! need:" + remainLength + " left:"+sdLeft, null);
				return;
			}
			
			if(listener != null) {
				int ret = listener.onLengthGet(mTotalLength, remainLength);
				if(ret == 1) {
					callOnCancel();
					return;
				}
			}
			
			int read = 0;
			byte buf[] = new byte[RW_BUF_SIZE];

			// 使用 BufferedInputStream 提高性能
			is = new BufferedInputStream(conn.getInputStream()); 
			// 开始回调
			callOnStart(startPos, mTotalLength);
			log.d("begin from " + startPos);

			long mCurLength = startPos;
			long t0 = 0;
			do {
				read = is.read(buf);
				if (read == -1) { // 下完了
					if(file2.renameTo(new File(fileSavePath))) // 改名
						callOnFinish();
					else
						callOnError("dl suc, but tmp rename fail!", new Exception("dl suc, but tmp rename fail!"));
					
					return; // 下完了，直接返回
				}

				raf.write(buf, 0, read);
				mCurLength += read;

				if(System.currentTimeMillis() - t0 > 1000) { //1s回调一次
					t0 = System.currentTimeMillis();
					callOnProgress(mCurLength, (byte) ((mCurLength / (float) mTotalLength) * 100));
				}

				//休眠 10 ms
				try {
					Thread.sleep(10);
				} catch (Exception e) {
				}
			} while (!canceled);// 点击取消就停止下载

			if (canceled) {
				callOnCancel();
			}
		} catch (Exception e) {
			if(PhoneUtils.isWifi(mContext) && e instanceof SocketTimeoutException) { //超时重试
				retry = true;
			}
			
			callOnError(e.getClass().getName() + ": " + e.getMessage(), e);
		} finally {
			try {
				raf.close();
			} catch (Exception e) {
			}
			try {
				is.close();
			} catch (Exception e) {
			}
			if (conn != null)
				conn.disconnect();
			
			if(retry && retryTimes < MAX_RETRY_TIMES) {
				retryTimes++;
				log.w("重试" + retryTimes);
				run();
			} else {
				DownloadManager.remove(this);
				FileUtils.releaseFileLock(lock);
				
				callOnClose();
			}
		}
	}
}
