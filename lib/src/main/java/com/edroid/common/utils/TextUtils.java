package com.edroid.common.utils;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.util.Locale;
import java.util.Random;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.widget.EditText;

/**
 * 字符串处理工具
 * 
 * @author Yichou 2013-8-7
 *
 */
public final class TextUtils {

	public static boolean isEmpty(String s) {
		return (s == null || s.length() == 0 || s.equals("null"));
	}

	public static boolean isEmpty(CharSequence s) {
		return (s == null || s.length() == 0);
	}
	
	public static int toInt(CharSequence s) {
		return toInt(s.toString());
	}
	
	public static int toInt(EditText edit) {
		return toInt(edit.getText().toString());
	}
	
	public static int toInt(String s) {
		try {
			return Integer.parseInt(s);
		} catch (Exception e) {
		}
		return 0;
	}
	
	public static int toInt(String s, int def) {
		try {
			return Integer.parseInt(s);
		} catch (Exception e) {
		}
		return def;
	}
	
	public static float toFloat(String s) {
		try {
			return Float.parseFloat(s);
		} catch (Exception e) {
		}
		return 0;
	}
	
	public static CharSequence getClipboard(Context c) {
		try {
			ClipboardManager cm = (ClipboardManager) c.getSystemService(Context.CLIPBOARD_SERVICE);
			return cm.getPrimaryClip().getItemAt(0).getText();
		} catch (Exception e) {
		}
		
		return null;
	}
	
	/**
	 * 此方法必须在 UI 线程调用
	 * 
	 * @param c
	 * @param s
	 */
	public static void setClipboard(Context c, String s) {
		if(Thread.currentThread().getId() != 1) 
			return;
		
		try {
			ClipboardManager cm = (ClipboardManager) c.getSystemService(Context.CLIPBOARD_SERVICE);
			cm.setPrimaryClip(ClipData.newPlainText(null, s));
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 随机一组简体汉字
	 * 
	 * @param paramInt 长度
	 */
	public static String makeJianHan(int paramInt) {
		Random localRandom = new Random();
		StringBuilder sb = new StringBuilder(paramInt*2 + 4);
		for (int i=0; i<paramInt; i++) {
			int j = 176 + Math.abs(localRandom.nextInt(39));
			int k = 161 + Math.abs(localRandom.nextInt(93));
			
			byte[] arrayOfByte = new byte[2];
			arrayOfByte[0] = Integer.valueOf(j).byteValue();
			arrayOfByte[1] = Integer.valueOf(k).byteValue();
			try {
				sb.append(new String(arrayOfByte, "GBk"));
			} catch (Exception e) {
			}
		}
		
		return sb.toString();
	}
	
	public static String makeJianHan2() {
		final int delta = 0x4fff - 0x4e00 + 1;

		Random ran = new Random();
		String ret = "";

		int n = 4 + ran.nextInt(8);
		for (int j = 0; j < n; j++) {
			ret += ((char) (0x4e00 + ran.nextInt(delta)));
		}

		return ret;
	}
	
	public static String is2String(InputStream is) {
		if(is == null)
			return null;
		
		ByteArrayOutputStream bos = new ByteArrayOutputStream(1024);
		BufferedInputStream bis = new BufferedInputStream(is);
		
		try {
			byte[] buf = new byte[64];
			int read = 0;
			while((read = bis.read(buf)) != -1) {
				bos.write(buf, 0, read);
			}
			return bos.toString();
		} catch (Exception e) {
		} finally {
		}
		
		return null;
	}
	
	public static String fmt(String format, Object... args) {
		return String.format(Locale.US, format, args);
	}
	
	/**
	 * 字符串转换unicode
	 */
	public static String toUnicode(String string) {
	    StringBuilder unicode = new StringBuilder();
	    for (int i = 0; i < string.length(); i++) {
	        // 取出每一个字符
	        char c = string.charAt(i);
	        // 转换为unicode
	        unicode.append("\\u" + Integer.toHexString(c));
	    }
	    return unicode.toString();
	}
	
	/**
	 * unicode 转字符串
	 */
	public static String fromUnicode(String unicode) {
		StringBuilder string = new StringBuilder();
	    String[] hex = unicode.split("\\\\u");
	    if(hex != null && hex.length > 0) {
		    for (int i = 1; i < hex.length; i++) {
		        // 转换出每一个代码点
		        int data = Integer.parseInt(hex[i], 16);
		        // 追加成string
		        string.append((char) data);
		    }
	    }
	    return string.toString();
	}
	
//	public static void main(String[] args) {
//		System.out.println(toUnicode("你好"));
//	}
	
	public static String nullhack(String src, String def) {
		return isEmpty(src)? def : src;
	}
}
