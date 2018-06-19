package com.edroid.common.utils;

import java.io.FileInputStream;
import java.io.FileWriter;
import java.util.Locale;

import android.util.Log;

/**
 * 
 * 日志打印模块
 * 
 * @author Yichou 2013-12-12
 * 
 * <p>2014-10-10 本地调试标志，默认是否开启依据本地调试
 * <p>2014-11-16 加入静态方法
 * <p>2015-3-7 增加单独 log 开关，SD卡 showlog 文件存在则开启
 */
public final class Logger {
	/**
	 * 本机是否开启调试
	 */
	public static boolean LOCAL_DEBUG_ON;
	
	/**
	 * 日志默认开关
	 */
	private static boolean SHOW_LOG;
	
	private boolean debug, show;
	private String tag;
	private String tmpTag;
	private String filepath;
	private FileWriter fw;
	
	private static final class A {
		static Object s;
		
		static void a() {
			s = A.class;
			
			LOCAL_DEBUG_ON = false;
			SHOW_LOG = false;
			
			//2017-6-2 改版
			try {
				FileInputStream is = new FileInputStream("/data/local/tmp/debug");
				System.out.println(is.read());
				is.close();
				LOCAL_DEBUG_ON = true;
			} catch (Exception e) {
			}
//			Logger.LOCAL_DEBUG_ON = new File("/data/local/tmp/debug").exists();
//			Logger.SHOW_LOG = new File("/data/local/tmp/showlog").exists();
			
			if(!SHOW_LOG) 
				SHOW_LOG = LOCAL_DEBUG_ON;

			Logger.sd("Jok", "Is there a shit? " + s.toString() + SHOW_LOG + ", " + LOCAL_DEBUG_ON);
		}
	}
	
	static {
		//不要惊奇，这只是一个混淆视听的调用
		A.a();
	}
	
	public static void a() {
		A.a();
	}

	public void setTagPerfix(String tagPerfix) {
		if(tagPerfix == null)
			tag = tmpTag;
		else {
			tag = tagPerfix + tag;
		}
	}
	
	/**
	 * 设置后将打印到文件
	 * 
	 * @param filepath 日志输出文件路径，传 NULL 相当于关闭
	 */
	public void setFilepath(String filepath) {
		if(filepath == null) { //相当于关闭
			if(this.fw != null) {
				try {
					this.fw.close();
				} catch (Exception e) {
				}
			}
			this.filepath = null;
			return;
		}
		
		if(this.filepath != null && this.filepath.equals(filepath)) 
			return;
		
		this.filepath = filepath;
		try {
			FileUtils.checkParentPath(filepath);
			this.fw = new FileWriter(filepath, true);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private Logger(boolean show, boolean debug, String tag, String filepath) {
		setDebug(debug);
		setShow(show || debug);
		setTag(tag);
		setFilepath(filepath);
	}
	
	public void setDebug(boolean debug) {
		this.debug = debug;
	}
	
	public void setShow(boolean show) {
		this.show = show;
	}

	public boolean isDebug() {
		return debug;
	}
	
	public void setTag(String tag) {
		this.tag = tag;
		this.tmpTag = tag;
	}
	
	public String getTag() {
		return tmpTag;
	}
	
	public static Logger create(boolean debug, String tag) {
		return new Logger(debug, debug, tag, null);
	}

	public static Logger create(String tag) {
		return new Logger(SHOW_LOG, LOCAL_DEBUG_ON, tag, null);
	}

	public static Logger create() {
		return new Logger(SHOW_LOG,LOCAL_DEBUG_ON, null, null);
	}
	
	public static Logger create(Class<?> cls) {
		return new Logger(SHOW_LOG, LOCAL_DEBUG_ON, cls.getSimpleName(), null);
	}
	
	/**
	 * 注意：Log 不允许 msg = null ，否则报空指针异常 2013-7-3 yichou
	 * 
	 * @param tag
	 * @param msg
	 */
	public void i(String msg) {
		i(tag, msg);
	}

	public void d(String msg) {
		d(tag, msg);
	}
	
	public void ifm(String s, Object... args) {
		i(tag, String.format(Locale.US, s, args));
	}
	
	public void dfm(String s, Object... args) {
		d(tag, String.format(Locale.US, s, args));
	}

	public void e(String msg) {
		e(tag, msg);
	}

	public void v(String msg) {
		v(tag, msg);
	}

	public void w(String msg) {
		w(tag, msg);
	}
	
	public void error(Throwable e) {
		if (show) {
			Log.e(tag, e.getMessage(), e);
		}
	}
	
	private void tof(String type, String tag, String msg) {
		try {
			fw.append(type).append(':').append(TimeUtils.getTimeNow()).append(' ').append(tag).append(' ').append(msg).append('\n');
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public void i(String tag, String msg) {
		if (show) {
			Log.i(tag, msg != null ? msg : "");
			if(fw != null)
				tof("I", tag, msg);
		}
	}

	public  void d(String tag, String msg) {
		if (show) {
			Log.d(tag, msg != null ? msg : "");
			if(fw != null)
				tof("D", tag, msg);
		}
	}

	public void e(String tag, String msg) {
		if (show) {
			Log.e(tag, msg != null ? msg : "");
			if(fw != null)
				tof("E", tag, msg);
		}
	}

	public void v(String tag, String msg) {
		if (show) {
			Log.v(tag, msg != null ? msg : "");
			if(fw != null)
				tof("V", tag, msg);
		}
	}

	public void w(String tag, String msg) {
		w(tag, msg, null);
	}

	public void we(Throwable e) {
		w(tag, e.getMessage(), e);
	}

	public void w(String tag, String msg, Throwable t) {
		if (show) {
			Log.w(tag==null? this.tag : tag, msg != null ? msg : "", t);
			if(fw != null)
				tof("W", tag, msg);
		}
	}
	
	public void error(String msg, Throwable e) {
		error(tag, msg, e);
	}
	
	public void error(String tag, String msg, Throwable e) {
		if (show) {
			Log.e(tag, msg, e);
			if(fw != null)
				tof("E", tag, msg);
		}
	}
	
	public static void si(String tag, String msg) {
		if (SHOW_LOG) {
			Log.i(tag, msg != null ? msg : "null");
		}
	}
	
	public static void sd(String tag, String msg) {
		if (SHOW_LOG) {
			Log.d(tag, msg != null ? msg : "null");
		}
	}
	
	public static void sw(String tag, String msg) {
		if (SHOW_LOG) {
			Log.w(tag, msg != null ? msg : "null");
		}
	}
	
	public static void se(String tag, String msg) {
		if (SHOW_LOG) {
			Log.e(tag, msg != null ? msg : "null");
		}
	}

	public static Logger getLogger(Class<?> cls) {
		return create(cls);
	}
}
