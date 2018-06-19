package com.mrpoid.mrplist.moduls;

import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;

import com.edroid.common.utils.FileUtils;

import android.content.Context;

public class MyFavoriteManager {
	static MyFavoriteManager instance = new MyFavoriteManager();
	
	
	public static MyFavoriteManager getInstance() {
		return instance;
	}
	
	private Context mContext;
	private List<MpFile> list;
	
	public MyFavoriteManager() {
		list = new ArrayList<MpFile>();
	}
	
	public void init(Context context) {
		this.mContext = context.getApplicationContext();
	}
	
	public void remove(int i) {
		list.remove(i);
		save();
	}
	
	public void add(String path) {
		for(MpFile file : list) {
			if(file.getPath().equals(path))
				return;
		}
		
		list.add(new MpFile(path));
		
		save();
	}
	
	public List<MpFile> getAll() {
		return list;
	}
	
	public void read() {
		list.clear();
		try {
			JSONArray array = new JSONArray(
					FileUtils.fileToString(mContext.getFileStreamPath("favorate.list")));
			for(int i=0; i<array.length(); i++) {
				list.add(new MpFile(array.getString(i)));
			}
		} catch (Exception e) {
			Common.log.e("read faorate.list fail!" + e.getMessage());
		}
	}
	
	public void save() {
		try {
			JSONArray array = new JSONArray();
			for(MpFile file : list) {
				array.put(file.getPath());
			}
			
			FileUtils.stringToFile(mContext.getFileStreamPath("favorate.list"), 
					array.toString());
		} catch (Exception e) {
			Common.log.e("save faorate.list fail!" + e.getMessage());
		}
	}
}
