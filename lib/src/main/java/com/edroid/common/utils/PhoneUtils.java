package com.edroid.common.utils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.List;
import java.util.Locale;

import android.Manifest.permission;
import android.content.Context;
import android.content.pm.PackageManager;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.provider.Settings.Secure;
import android.telephony.TelephonyManager;

/**
 * 手机相关的一些参数获取
 * 
 * @author yichou 2014-10-25
 * 
 */
public final class PhoneUtils {
	/**
	 * 联网类型
	 */
	public static final int NETTYPE_UNKNOW = 0;
	public static final int NETTYPE_WIFI = 1;
	public static final int NETTYPE_2G = 2;
	public static final int NETTYPE_3G = 3;
	public static final int NETTYPE_4G = 4;
	public static final int NETTYPE_5G = 5;

	public static final int NETOPT_UNKNOW = 0; // 移动
	public static final int NETOPT_MOBILE = 1; // 移动
	public static final int NETOPT_UNICOM = 2; // 联通
	public static final int NETOPT_TELECOM = 3; // 电信


	/**
	 * 获取手机当前网络
	 * 
	 * @param context
	 * @return
	 */
	public static NetworkInfo getActiveNetworkInfo(Context context) {
		ConnectivityManager connectivity = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
		
		if (connectivity != null) {
			// 获取网络连接管理的对象
			NetworkInfo info = connectivity.getActiveNetworkInfo();
			
			if (info != null && info.isAvailable()) // 有联网 且 可以联网
				return info;
			
			// if (info != null
			// && info.isConnected() //这样是判断当前活动网络是不是正在联网数据收发
			// && info.getState() == NetworkInfo.State.CONNECTED)
			// {
			// return info;
			// }
		}
		
		return null;
	}
	
	public static boolean isNetAvailable(Context context) {
		return (getActiveNetworkInfo(context) != null);
	}

	public static String getNetTypeString(Context context) {
		switch (getNetType(context)) {
		case NETTYPE_2G:
			return "2G";
		case NETTYPE_3G:
			return "3G";
		case NETTYPE_4G:
			return "4G";
		case NETTYPE_WIFI:
			return "WIFI";
		}
		
		return "unknow";
	}
	
	public static int getNetType(Context context) {
		ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo info = cm.getActiveNetworkInfo();
		
		if (info != null) {
			if (info.getType() == ConnectivityManager.TYPE_MOBILE) {
				TelephonyManager tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
				
				switch (tm.getNetworkType()) {
				case TelephonyManager.NETWORK_TYPE_GPRS:
				case TelephonyManager.NETWORK_TYPE_EDGE:
				case TelephonyManager.NETWORK_TYPE_CDMA:
				case TelephonyManager.NETWORK_TYPE_1xRTT:
				case TelephonyManager.NETWORK_TYPE_IDEN:
					return NETTYPE_2G;
				case TelephonyManager.NETWORK_TYPE_UMTS:
				case TelephonyManager.NETWORK_TYPE_EVDO_0:
				case TelephonyManager.NETWORK_TYPE_EVDO_A:
				case TelephonyManager.NETWORK_TYPE_HSDPA:
				case TelephonyManager.NETWORK_TYPE_HSUPA:
				case TelephonyManager.NETWORK_TYPE_HSPA:
				case TelephonyManager.NETWORK_TYPE_EVDO_B:
				case TelephonyManager.NETWORK_TYPE_EHRPD:
				case TelephonyManager.NETWORK_TYPE_HSPAP:
					return NETTYPE_3G;
				case TelephonyManager.NETWORK_TYPE_LTE:
					return NETTYPE_4G;
				}
			} else if (info.getType() == ConnectivityManager.TYPE_WIFI) {
				return NETTYPE_WIFI;
			}
		}

		return NETOPT_UNKNOW;
	}
	
	public static boolean isWifi(Context context) {
		return (getNetType(context) == NETTYPE_WIFI);
	}
	
	/**
	 * 判断当前连接的网络是 wifi或者3g（3G以上）
	 * 
	 * @return true 你不用担心用户流量了
	 */
	public static boolean isWifiOr3G(Context context) {
		switch (getNetType(context)) {
		case NETTYPE_3G: case NETTYPE_4G: case NETTYPE_5G: case NETTYPE_WIFI:
			return true;
		}

		return false;
	}

	public static boolean is3G(Context context) {
		switch (getNetType(context)) {
		case NETTYPE_3G: case NETTYPE_4G: case NETTYPE_5G:
			return true;
		}

		return false;
	}
	
	/**
	 * 获取手机运营商
	 * 
	 * @param context
	 * 
	 * @return yd,lt,dx,unknow
	 */
	public static String getNetOptString(Context context) {
		switch (getNetOpt(context)) {
		case NETOPT_MOBILE:
			return "mobile";
		case NETOPT_UNICOM:
			return "unicom";
		case NETOPT_TELECOM:
			return "telecom";
		}
		return "unknow";
	}
	
	/**
	 * 获取SIM卡运营商
	 * 
	 * @param context
	 * @return
	 */
	public static int getNetOpt(Context context) {
		TelephonyManager tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
		String imsi = tm.getSubscriberId();
		int mnc = -1;

		/**
		 * 谨记此处错误，很多手机要么返回 null 要么长度不对
		 */
		if (imsi != null && imsi.length() == 15) { // 很多奇葩手机
			mnc = imsi.charAt(4) - '0';
		} else {
			mnc = context.getResources().getConfiguration().mnc;
		}

		switch (mnc) {
		case 1:
			return NETOPT_UNICOM;
		case 0: case 2: case 7:
			return NETOPT_MOBILE;
		case 3:
			return NETOPT_TELECOM;
		}

		return NETOPT_UNKNOW;
	}

	/**
	 * 获取当前连接的网络的 apn 名称
	 * 
	 * @param context
	 * @return
	 */
	public static String getApn(Context context) {
		NetworkInfo info = getActiveNetworkInfo(context);
		if (info != null) {
			String apn = info.getExtraInfo();
			if (apn != null && apn.length() > 0) {
				return apn.toLowerCase(Locale.getDefault());
			}
		}

		return null;
	}

	/**
	 * 获取IMSI
	 * 
	 * @param context
	 * @return 失败返回 Null
	 */
	public static String getImsi(Context context) {
		try {
			TelephonyManager tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
			String ret = tm.getSubscriberId();
			if (ret != null && ret.length() == 15)
				return ret;
		} catch (Exception e) {
		}

		return null;
	}

	/**
	 * 获取IMEI
	 * 
	 * @param context
	 * @return 失败返回 Null
	 */
	public static String getImei(Context context) {
		String ret = null;
		try {
			TelephonyManager tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
			ret = tm.getDeviceId();
		} catch (Exception e) {
		}
		
		return ret;
	}

	/**
	 * 获取手机号
	 * 
	 * @param context
	 * @return
	 */
	public static String getPhoneNumber(Context context) {
		try {
			TelephonyManager tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
			String ret = tm.getLine1Number();
			if (ret != null && ret.length() > 0)
				return ret;
		} catch (Exception e) {
		}

		return null;
	}
	

	/**
	 * 获取 Android ID
	 * @param context
	 * @return 失败返回 Null
	 */
	public static String getAndroidId(Context context) {
		String android_id = null;
		try {
			android_id = Secure.getString(context.getContentResolver(), Secure.ANDROID_ID);
		} catch (Exception e) {
		}
		
		return android_id;
	}
	
	/**
	 * 用 getprop 命令来获取系统属性
	 */
	private static String getSystemProperties(String name) {
		BufferedReader r = null;
		try {
			Process p = Runtime.getRuntime().exec("getprop " + name);
			r = new BufferedReader(new InputStreamReader(p.getInputStream()));
			return r.readLine();
		} catch (Exception e) {
		} finally {
			try {
				r.close();
			} catch (Exception e2) {
			}
		}
		
		return null;
	}
	
	/**
	 * 获取 net.hostname
	 * @return 失败返回 null
	 */
	public static String getHostname() {
		String hostname = null;
		try {
			hostname = getSystemProperties("net.hostname");
		} catch (Exception e) {
		}
		return hostname;
	}

	/**
	 * 获取 Android 基带号
	 * @return 失败返回 null
	 */
	public static String getBaseband() {
		String radio = null;
		try {
			radio = getSystemProperties("gsm.version.baseband");
		} catch (Exception e) {
		}
		return radio;
	}
	
	/**
	 * 获取 Android 序列号
	 * @return 失败返回 null
	 */
	public static String getSerial() {
		String serial = null;
		try {
			serial = getSystemProperties("ro.serialno");
			if(serial == null)
				serial = getSystemProperties("ro.boot.serialno");
//			if(serial == null)
//				serial = Build.SERIAL;
		} catch (Exception e) {
		}
		
		return serial;
	}
	
	/**
	 * 获取 mac 地址
	 * @return 失败返回 Null
	 */
	public static String getMac(Context context) {
		try {
			WifiManager wifi = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);  
			WifiInfo info = wifi.getConnectionInfo();  
			return info.getMacAddress();  
		} catch (Exception e) {
		}
		
		return null;
	}

	/**
	 * 电话状态是否可读
	 * 
	 * @param context
	 * @return
	 */
	public static boolean isPhoneStateReadable(Context context) {
		PackageManager pm = context.getPackageManager();
		String pkgName = context.getPackageName();
		int readable = pm.checkPermission(permission.READ_PHONE_STATE, pkgName);

		return readable == PackageManager.PERMISSION_GRANTED;
	}

	
	public static Address getAddress(Context context) {
		LocationManager manager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
		
		Criteria criteria = new Criteria();
		criteria.setAccuracy(Criteria.ACCURACY_FINE);
		criteria.setAltitudeRequired(false);
		criteria.setBearingRequired(false);
		criteria.setCostAllowed(false);
		criteria.setPowerRequirement(Criteria.POWER_LOW);
		
		// 取得效果最好的criteria
		String provider = manager.getBestProvider(criteria, true);
		if (provider == null) return null;
		
		// 得到坐标相关的信息
		Location location = manager.getLastKnownLocation(provider);
		if (location == null) return null;

		// 更具地理环境来确定编码
		Geocoder gc = new Geocoder(context, Locale.getDefault());
		try {
			double latitude = location.getLatitude();
			double longitude = location.getLongitude();
			// 取得地址相关的一些信息\经度、纬度
			List<Address> addresses = gc.getFromLocation(latitude, longitude, 1);
			if (addresses.size() > 0) {
				return addresses.get(0);
			}
		} catch (IOException e) {
		}
		
		return null;
	}
}
