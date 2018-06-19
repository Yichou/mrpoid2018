package com.mrpoid.mrplist.moduls;

import java.io.File;
import java.util.Locale;

import com.mrpoid.mrplist.utils.MrpUtils;
import com.edroid.common.utils.TimeUtils;

/**
 * 2012/10/9
 * @author JianbinZhu
 *
 */
public class MpFile implements Comparable<MpFile> {
	private String path;
	private String name;
	
	private FileType type;
	private long length;
	private boolean isDir;
	private boolean isParent;
	String lastTimeS;
	
	private String title;
	private String msg;
	private String sizeStr;
	

	public MpFile() {
		this.name = "..";
		this.msg = "父目录";
		this.isDir = true;
		this.isParent = true;
	}
	
	public MpFile(String path) {
		this(new File(path));
	}
	
	/**
	 * @param f 可为NULL
	 */
	public MpFile(File f) {
		this.path = f.getParent();
		this.name = f.getName();
		this.isDir = f.isDirectory();
		this.length = f.length();
		this.lastTimeS = TimeUtils.getDateTime(f.lastModified());
	}
	
	private String coverSize(long size) {
		String s = "";

		if (size < 1024)
			s += size + "b";
		else if (size < 1024 * 1024) {
			s = String.format(Locale.getDefault(), "%.2f K", size / 1024f);
		} else if (size < 1024 * 1024 * 1024) {
			s = String.format(Locale.getDefault(), "%.2f M", size / 1024 / 1024f);
		} else {
			s = String.format(Locale.getDefault(), "%.2f G", size / 1024 / 1024 / 1024f);
		}

		return s;
	}
	
	public FileType getType() {
		if(type == null) {
			type = isDir? FileType.FOLDER : FileType.getTypeByName(name);
		}
		
		return type;
	}
	
	public String getMsg() {
		if(msg == null) {
			if(getType() == FileType.FOLDER) {
				msg = "文件夹";
			} else {
				//get permission
				msg = lastTimeS;
//				msg = "rwx---r--";
			}
		}
		
		return msg;
	}
	
	public String getSizeString() {
		if(sizeStr == null) {
			if(getType() == FileType.FOLDER) {
				sizeStr = "";
			} else {
				sizeStr = coverSize(length);
			}
		}
		
		return sizeStr;
	}
	
	public String getTtile() {
		if(title == null) {
			if(getType() == FileType.MRP) {
				title = MrpUtils.readMrpAppName(path + File.separatorChar + name);
				if(title == null)
					title = name;
			} else {
				title = name;
			}
		}
		
		return title;
	}

	@Override
	public int compareTo(MpFile another) {
		if(this.isDir && !another.isDir)
			return -1;
		else if(!this.isDir && another.isDir)
			return 1;
		
		return 0;
	}
	
	public File toFile() {
		return new File(path, name);
	}
	
	/**
	 * 获取完整路径
	 * 
	 * @return
	 */
	public String getPath() {
		return path + File.separatorChar + name;
	}
	
	public boolean isDir() {
		return isDir;
	}
	
	public String getName() {
		return name;
	}
	
	public boolean isFile() {
		return !isDir;
	}
	
	public long getLength() {
		return length;
	}
	
	public boolean isParent() {
		return isParent;
	}
	
	@Override
	public String toString() {
		return "[path=" + path + ", name=" + name + "]";
	}
}
