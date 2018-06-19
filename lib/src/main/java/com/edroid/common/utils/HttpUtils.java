package com.edroid.common.utils;



import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

import android.util.Log;

/**
 * HttpURLConnection
 * 
 * @author fary
 * @date 2015-08-12
 */
public class HttpUtils {
	static final String UA = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1;SV1)";
	static final String CHARSET = "UTF-8";
	static final Logger log = Logger.create("HttpUtils");
	
	static {
		try {
			initSSL();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private static String is2String(InputStream is, boolean gz) throws IOException {
		ByteArrayOutputStream bos = new ByteArrayOutputStream(1024);
		BufferedInputStream bis = new BufferedInputStream(is);
		
		byte[] buf = new byte[4*1024];
		int read = 0;
		while((read = bis.read(buf)) != -1) {
			bos.write(buf, 0, read);
		}
		if(gz) 
			return new String(GZUtils.ungz(bos.toByteArray()));
		
		return bos.toString();
	}

	public static String get(String url) {
		return get(url, null);
	}
	
	/**
	 * @param param 不为NULL 则url 不能带参数 url+?+param
	 */
	public static String get(String url, String param) {
		return get(url, param, false);
	}
	
	public static String get(String url, String param, boolean gz) {
		InputStream in = null;
		HttpURLConnection httpUrlConnection = null;
		
		try {
			if(param != null) {
				if(url.indexOf('?') == -1)
					url += '?';
				url += param;
			}
			
			log.i("get url=" + url);
			
			httpUrlConnection = (HttpURLConnection) new URL(url).openConnection();
			httpUrlConnection.setRequestProperty("accept", "*/*");
			httpUrlConnection.setRequestProperty("connection", "Keep-Alive");
			httpUrlConnection.setRequestProperty("user-agent", UA);
			httpUrlConnection.setConnectTimeout(5*60*1000);
			
			httpUrlConnection.connect();
			
			// // 获取所有响应头字段
			// Map<String, List<String>> map = connection.getHeaderFields();
			// // 遍历所有的响应头字段
			// for (String key : map.keySet()) {
			// System.out.println(key + "--->" + map.get(key));
			// }
			
			in = httpUrlConnection.getInputStream();
			return is2String(in, gz);
		} catch (Exception e) {
//			log.error(e);
		} finally {
			try {
				in.close();
			} catch (Exception e) {
			}
			try {
				httpUrlConnection.disconnect();
			} catch (Exception e) {
			}
		}
		return null;
	}
	
	public static String post(String url, String param) {
		return post(url, param, false);
	}

	/**
	 * post方法
	 */
	public static String post(String url, String param/*, byte[] data*/, boolean ungz) {
		OutputStream out = null;
		InputStream in = null;
		HttpURLConnection httpUrlConnection = null;
		
		log.i("post url=" + url + "?" + param);
		try {
			// 打开和URL之间的连接
			httpUrlConnection = (HttpURLConnection) new URL(url).openConnection();
			// 设置通用的请求属性
			httpUrlConnection.setRequestProperty("accept", "*/*");
			httpUrlConnection.setRequestProperty("connection", "Keep-Alive");
			httpUrlConnection.setRequestProperty("user-agent", UA);
			// 发送POST请求必须设置如下两行
			httpUrlConnection.setDoOutput(true);
			httpUrlConnection.setDoInput(true);
			
			// 获取URLConnection对象对应的输出流
//			long t0 = System.currentTimeMillis();
			out = httpUrlConnection.getOutputStream();
			//平均 60ms
//			out.write(param.getBytes());
//			out.flush();

			//平均 35ms
			if(param != null) {
				OutputStreamWriter w = new OutputStreamWriter(out);
				w.write(param);
				w.flush();
			}
//			if(data != null) {
//				out.write("gzdata=".getBytes());
//				out.write(data);
//				out.flush();
//			}
			
			{
//				BufferedWriter w = new BufferedWriter(new OutputStreamWriter(out));
//				w.write(param);
//				w.flush();
			}
			{
//				PrintWriter w = new PrintWriter(out);
//				w.write(param);
//				w.flush();
			}
//			System.err.println("t=" + (System.currentTimeMillis() - t0));
			
			in = httpUrlConnection.getInputStream();
			return is2String(in, ungz);
		} catch (Exception e) {
//			log.error(e);
		} finally {
			try {
				in.close();
			} catch (Exception ex) {
			}
			try {
				out.close();
			} catch (Exception ex) {
			}
			try {
				httpUrlConnection.disconnect();
			} catch (Exception e) {
			}
		}
		
		return null;
	}
	
	private static void streamToFile(InputStream is, File dstFile) throws Exception {
		OutputStream os = null;
		try {
			is = new BufferedInputStream(is);
			os = new FileOutputStream(dstFile);
			byte[] buf = new byte[1024*16];
			int read;
			while((read = is.read(buf)) != -1) {
				os.write(buf, 0, read);
			}
			os.flush();
		} catch (Exception e) {
			throw e;
		} finally {
			try {
				os.close();
			} catch (Exception e) {
			}
		}
	}
	
	/**
	 * 简单的阻塞下载，不考虑代理，不考虑文件父目录，不支持续传
	 * 
	 * @param url 网址
	 * @param dstFile 目标文件
	 */
	public static boolean download(String url, File dstFile) {
		HttpURLConnection conn = null;
		InputStream is = null;
		
		try {
			log.i("dl url=" + url + ", file=" + dstFile);
			
//			if(url.startsWith("https"))
				
			
			conn = (HttpURLConnection) new URL(url).openConnection();
			is = new BufferedInputStream(conn.getInputStream());
			streamToFile(is, dstFile);
			
			return true;
		} catch (Exception e) {
//			log.error(e);
		} finally {
			try {
				is.close();
			} catch (Exception e) {
			}
			try {
				conn.disconnect();
			} catch (Exception e) {
			}
		}
		return false;
	}
	
	private static class MyHostnameVerifier implements HostnameVerifier {

		@Override
		public boolean verify(String hostname, SSLSession session) {
			// TODO Auto-generated method stub
			return true;
		}
	}

	private static class MyTrustManager implements X509TrustManager {

		@Override
		public void checkClientTrusted(X509Certificate[] chain, String authType) throws CertificateException {
			// TODO Auto-generated method stub

		}

		@Override
		public void checkServerTrusted(X509Certificate[] chain, String authType) throws CertificateException {
			// TODO Auto-generated method stub

		}

		@Override
		public X509Certificate[] getAcceptedIssuers() {
			// TODO Auto-generated method stub
			return null;
		}
	}
	
	
	private static void initSSL() throws Exception {
		SSLContext sc = SSLContext.getInstance("TLS");
		sc.init(null, new TrustManager[] { new MyTrustManager() }, new SecureRandom());
		HttpsURLConnection.setDefaultSSLSocketFactory(sc.getSocketFactory());
		HttpsURLConnection.setDefaultHostnameVerifier(new MyHostnameVerifier());
	}

	public static String encode(String src) {
		try {
			return URLEncoder.encode(src, CHARSET);
		} catch (Exception e) {
		}
		return src;
	}
	
	public static String decode(String src) {
		try {
			return URLDecoder.decode(src, CHARSET);
		} catch (Exception e) {
		}
		return src;
	}
}
