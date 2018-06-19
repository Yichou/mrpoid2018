package com.edroid.common.utils;

import java.io.FileInputStream;
import java.io.InputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * 安全相关工具类 各种加解密
 * 
 * @author Yichou 2013-4-26
 * 
 */
public final class Md5Utils {
	private final static char[] hexDigitsChr = "0123456789abcdef".toCharArray();

	/**
	 * 计算MD5 32位
	 */
	public static String md532(String s) {
		return md532(s.getBytes());
	}
	
	public static String md532(byte args[]) {
		return md532(args, null);
	}
	
	public static String md532(byte args[], byte keys[]) {
		// 使用MD5创建MessageDigest对象
		try {
			MessageDigest mdTemp = MessageDigest.getInstance("MD5");
			mdTemp.update(args);
			
			if(keys != null)
				mdTemp.update(keys);

			byte[] md = mdTemp.digest();
			return md2s(md);
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		}

		return null;
	}
	
	static String md2s(byte[] md) {
		int j = md.length;
		char str[] = new char[j * 2];
		int k = 0;

		for (int i = 0; i < j; i++) {
			byte b = md[i];

			// 将没个数(int)b进行双字节加密
			str[k++] = hexDigitsChr[b >> 4 & 0xf];
			str[k++] = hexDigitsChr[b & 0xf];
		}

		return new String(str);
	}
	
	/**
	 * 计算MD5 16位 取32位的 8-24位
	 */
	public static String md516(String s) {
		return md516(s.getBytes());
	}
	
	/**
	 * 计算MD5 16位 取32位的 8-24位
	 */
	public static String md516(byte args[]) {
		return md516(args, null);
	}
	
	public static String md516(byte args[], byte keys[]) {
		return md532(args, keys).substring(8, 24);
	}
	
	/**
	 * 计算文件 md5
	 * 
	 * @param path
	 * @return
	 */
	public static byte[] fromFile(String path) {
		InputStream is = null;
		
		try {
			MessageDigest mdTemp = MessageDigest.getInstance("MD5");
			is = new FileInputStream(path);
			
			byte[] buf = new byte[1024];
			int read;
			while((read = is.read(buf)) != -1)  {
				mdTemp.update(buf, 0, read);
			}
			
			byte[] md = mdTemp.digest();
			return md;
		} catch (Exception e) {
		} finally {
			if(is != null)
				try {
					is.close();
				} catch (Exception e) {
				}
		}

		return null;
	}
	
	public static String fromFileS(String path) {
		return md2s(fromFile(path));
	}
}
