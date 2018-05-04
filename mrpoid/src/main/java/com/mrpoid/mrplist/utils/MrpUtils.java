package com.mrpoid.mrplist.utils;

import java.io.RandomAccessFile;
import java.nio.charset.Charset;

import com.mrpoid.mrplist.moduls.MrpInfo;

public class MrpUtils {
	public static String readMrpAppName(String path) {
		RandomAccessFile raf = null;
		try {
			raf = new RandomAccessFile(path, "r");
			byte[] buf = new byte[32];
			
			raf.seek(28);
			raf.read(buf, 0, 24);
			buf[24] = 0;
			
			return new String(buf, 0, 24, Charset.forName("GB2312"));
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				raf.close();
			} catch (Exception e) {
			}
		}
		
		return null;
	}
	
	public static int byteArrayToInt(byte[] b, int offset) {
		int value = 0;
		for (int i = 0; i < 4; i++) {
			int shift = (4 - 1 - i) * 8;
			value += (b[i + offset] & 0x000000FF) << shift;
		}
		
		return value;
	}
	
	public static MrpInfo readMrpInfo(String path) {
		MrpInfo mrpInfo = new MrpInfo();
		
		RandomAccessFile raf = null;
		try {
			raf = new RandomAccessFile(path, "r");
			byte[] buf = new byte[128];
			
			final Charset charset = Charset.forName("GB2312");
			
			raf.seek(28);
			raf.read(buf, 0, 22);
			mrpInfo.label = new String(buf, 0, 22, charset);
			
			raf.seek(16);
			raf.read(buf, 0, 12);
			mrpInfo.name = new String(buf, 0, 12, charset);

			raf.seek(68);
			raf.read(buf, 0, 4);
			mrpInfo.appid = Integer.reverse(byteArrayToInt(buf, 0));

			raf.seek(72);
			raf.read(buf, 0, 4);
			mrpInfo.version = Integer.reverse(byteArrayToInt(buf, 0));
			
			raf.seek(88);
			raf.read(buf, 0, 38);
			mrpInfo.vendor = new String(buf, 0, 38, charset);

			raf.seek(128);
			raf.read(buf, 0, 60);
			mrpInfo.detail = new String(buf, 0, 60, charset);
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				raf.close();
			} catch (Exception e) {
			}
		}
		
		return mrpInfo;
	}
}
