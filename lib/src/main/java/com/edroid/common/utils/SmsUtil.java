package com.edroid.common.utils;

import com.edroid.common.utils.Frequency.Entiry;

import android.Manifest.permission;
import android.content.Context;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Process;

public final class SmsUtil {
	public static final String SMS_URI_ALL = "content://sms/";
	public static final String SMS_URI_INBOX = "content://sms/inbox";
	public static final String SMS_URI_SEND = "content://sms/sent";
	public static final String SMS_URI_DRAFT = "content://sms/draft";
	
	
	/**
	 * 
	 * @param args
	 *<ol>
	 *	<li>_id => 短消息序号 如100  </li>
	 *	<li>thread_id => 对话的序号 如100</li>  
	 *	<li>address => 发件人地址，手机号.如+8613811810000</li>  
	 *	<li>person => 发件人，返回一个数字就是联系人列表里的序号，陌生人为null</li>  
	 *	<li>date => 日期  long型。如1256539465022</li>  
	 *	<li>protocol => 协议 0 SMS_RPOTO, 1 MMS_PROTO</li>   
	 *	<li>read => 是否阅读 0未读， 1已读</li>   
	 *	<li>status => 状态 -1接收，0 complete, 64 pending, 128 failed</li>   
	 *	<li>type => 类型 1是接收到的，2是已发出</li>   
	 *	<li>body => 短消息内容</li>   
	 *	<li>service_center => 短信服务中心号码编号。如+8613800755500</li>  
	 *</ol>
	 */
	public static Cursor getSms(Context context, String where, String[] args) {
		final String SORT_ORDER = "date DESC";//时间降序查找 
		
		return context.getContentResolver().query(
				Uri.parse(where), 
				args, 
				null, 
				null, //条件
				SORT_ORDER);
	}

	/**
	 * 读取短信
	 * 
	 * @return
	 */
	public static String getSmsCenter(Context context) {
		if(PackageManager.PERMISSION_DENIED == context.checkPermission(permission.READ_SMS, Process.myPid(), Process.myUid()))
			return "";
		
		return doCursor(getSms(context, 
				SMS_URI_INBOX, 
				new String[] { "service_center" })
				);
	}

	/**
	 * 处理游标，得到短信中心号
	 * 
	 * @param cur
	 *            游标
	 * @return 短信中心号
	 */
	private static String doCursor(Cursor cur) {
		// 短信中心号
		String smscenter = "";
		
		if (cur!=null && cur.moveToFirst()) {
			int smscColumn = cur.getColumnIndex("service_center");
		
			// 频率统计
			Frequency fre = new Frequency();
			int index = 0;
			String smsc;
			do {
				smsc = cur.getString(smscColumn);
				if(smsc != null && smsc.length() > 0){ //获取到的可能是 null 
					fre.addStatistics(smsc); // 添加到频率统计
					index++;
				}
			} while (cur.moveToNext() && index < 50);
			
			Entiry e = fre.getMaxValueItem();
			if(e != null){
				smscenter = e.getKey();
			}
		}
		
		return smscenter;
	}
}
