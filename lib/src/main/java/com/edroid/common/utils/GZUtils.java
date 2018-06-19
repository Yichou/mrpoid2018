package com.edroid.common.utils;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;



/**
 * 数据压缩解压工具，优化网络传输
 * 
 * @author yihou 2015-12-24
 *
 */
public class GZUtils {
	
	public static byte[] gz(byte[] data) {
		GZIPOutputStream os = null;
		try {
			ByteArrayOutputStream buf = new ByteArrayOutputStream(1024);
			os = new GZIPOutputStream(buf);
			os.write(data);
			os.finish();
			
			return buf.toByteArray();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return null;
	}

	public static byte[] ungz(byte[] data) {
		GZIPInputStream is = null;
		
		try {
			is = new GZIPInputStream(new ByteArrayInputStream(data));
			ByteArrayOutputStream os = new ByteArrayOutputStream(1024);
			
			int read = 0;
			byte[] buf = new byte[256];
			while((read = is.read(buf)) !=-1) {
				os.write(buf, 0, read);
			}
			
			return os.toByteArray();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return null;
	}
}
