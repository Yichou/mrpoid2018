package com.edroid.common.utils;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;


/**
 * 时间、日期、本地化 相关工具类
 * 
 * @author Yichou 2013-6-24
 *
 */
public final class TimeUtils {
	/**
	 * @return 当前地域
	 */
	public static Locale getLocale(){
		return Locale.getDefault();
	}
	
	static final String DEF_DATE_TIME_TEMPLATE = "yyyy-MM-dd HH:mm:ss";
	static final String DEF_DATE_TEMPLATE = "yyyy-MM-dd";
	static final String DEF_TIME_TEMPLATE = "HH:mm:ss";
	
	/**
	 * @return 形式  {@code#DEF_DATE_TIME_TEMPLATE}
	 */
	public static String getDateTimeNow() {
		return getDateTimeNow(DEF_DATE_TIME_TEMPLATE);
	}

	/**
	 * @return 传入模块
	 */
	public static String getDateTimeNow(String template) {
		SimpleDateFormat sdf = new SimpleDateFormat(template, getLocale());
		return sdf.format(new Date());
	}
	
	/**
	 * @return 形式  yyyy-MM-dd HH:mm:ss
	 */
	public static String formatDate(long ms) {
		return formatDate(ms, DEF_DATE_TIME_TEMPLATE);
	}
	
	public static String getDateTime(long ms) {
		return formatDate(ms, DEF_DATE_TIME_TEMPLATE);
	}

	public static String getTime(long ms) {
		return formatDate(ms, DEF_TIME_TEMPLATE);
	}

	public static String getDate(long ms) {
		return formatDate(ms, DEF_DATE_TEMPLATE);
	}
	
	public static String formatDate(long ms, String template) {
		SimpleDateFormat format = new SimpleDateFormat(template, getLocale());
		return format.format(new Date(ms));
	}
	
	/**
	 * @return 形式  12:23:32
	 */
	public static String getTimeNow() {
		SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss", getLocale());
		return sdf.format(new Date());
	}
	
	public static int getDayOfYear() {
		return Calendar.getInstance(getLocale()).get(Calendar.DAY_OF_YEAR);
	}
	
	public static int getHourOfDay() {
		return Calendar.getInstance(getLocale()).get(Calendar.HOUR_OF_DAY);
	}
	
	public static long currentTimeMillis() {
		return System.currentTimeMillis();
	}
}
