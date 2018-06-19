package com.mrpoid.view;

import java.io.File;

import android.content.Context;
import android.preference.Preference;
import android.util.AttributeSet;

import com.mrpoid.core.MrpoidSettings;

public class PathPreference extends Preference {
	public PathPreference(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public PathPreference(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public PathPreference(Context context) {
		super(context);
	}
	
	private String defRoot, defDir;
	
	public void setDefRoot(String defRoot) {
		if(defRoot.indexOf(defRoot.length()-1) != File.separatorChar)
			defRoot.concat(File.separator);
		this.defRoot = defRoot;
	}
	
	public void setDefDir(String defDir) {
		this.defDir = defDir;
	}
	
	@Override
	protected void onClick() {
		PathChooseDialog.ChoosePath(getContext(), new PathChooseDialog.ChooseCompleteListener() {
			@Override
			public void onComplete(String fullPath, String relativePath) {
				if(getKey().equals(MrpoidSettings.kMythroadPath))
					setPath(relativePath);
				else
					setPath(fullPath);
			}
		}, defRoot, defDir);
	}
	
	public void setPath(String finalPath) {
		persistString(finalPath);
		//通知监听器
		callChangeListener(finalPath);
	}
}
