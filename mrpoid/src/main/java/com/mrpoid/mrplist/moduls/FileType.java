package com.mrpoid.mrplist.moduls;

import java.lang.ref.WeakReference;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.mrpoid.mrpliset.R;

/**
 * 
 * @author Yichou 
 *
 */
public enum FileType {
	FOLDER(R.drawable.ic_fso_folder, null),	//文件夹
	
	TEXT(R.drawable.fso_type_document,
			new String[]{".txt"}),		//文本文件
			
	ARCHIVE(R.drawable.fso_type_compress,
			new String[]{".zip", ".7z", ".gz", ".tar"}),	//压缩文件
			
	AUDIO(R.drawable.fso_type_audio,
			new String[]{".mid", ".mp3", ".wav"}),		//声音文件
			
	VIDEO(R.drawable.fso_type_video,
			new String[]{".mp4", ".3gp", ".rm"}),		//文件
			
	IMAGE(R.drawable.fso_type_image,
			new String[]{".png", ".jpg", ".bmp", ".gif"}),		//图像文件
	
	APK(R.drawable.fso_type_app,
			new String[]{".apk"}),		//图像文件
			
	MRP(R.drawable.fso_type_mrp,
			new String[]{".mrp"}),		//图像文件
			
	NOTYPE(R.drawable.ic_fso_default,
			null);		//无类型文件
	
	
	private final int icon; 
	private final String[] regExts;
	private WeakReference<Bitmap> mBitmapIcon;
	
	private FileType(int icon, String[] regExts) {
		this.icon = icon;
		this.regExts = regExts;
	}
	
	public int getIconRes() {
		return icon;
	}
	
	public String[] getRegExts() {
		return regExts;
	}
	
	public Bitmap getIconBitmap(Resources r) {
		Bitmap bmp = mBitmapIcon==null? null : mBitmapIcon.get();
		if(bmp == null || bmp.isRecycled()) {
			bmp = BitmapFactory.decodeResource(r, icon);
			mBitmapIcon = new WeakReference<Bitmap>(bmp);
		}
		
		return bmp;
	}
	
	public static FileType getTypeByName(String name) {
		if(name == null || name.indexOf('.') == -1)
			return NOTYPE;
		
		int i = name.lastIndexOf('.'); //最后一个 .

		for(FileType type : FileType.values()) {
			String[] exts = type.regExts;
			
			if(exts == null) continue;
			
			for(String ext : exts) {
				if(name.regionMatches(true, i, ext, 0, ext.length())) {
					return type;
				}
			}
		}
		
		return NOTYPE;
	}
	
	public static void loadIcons(Resources r) {
		for(FileType type : FileType.values()) {
			type.getIconBitmap(r);
		}
	}

	public static void unloadIcons() {
		for(FileType type : FileType.values()) {
			Bitmap bmp = type.mBitmapIcon==null? null : type.mBitmapIcon.get();
			if(bmp != null && !bmp.isRecycled()) {
				bmp.recycle();
			}
			
			type.mBitmapIcon = null;
		}
	}
}
