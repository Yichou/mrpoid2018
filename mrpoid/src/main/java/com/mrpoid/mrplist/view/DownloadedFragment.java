package com.mrpoid.mrplist.view;

import android.os.Environment;

/**
 * 已下载
 * 
 * @author YYichou 2014-06-21
 *
 */
public class DownloadedFragment extends BaseMrpListFragment {

	public DownloadedFragment() {
		super(3);
	}

	@Override
	protected void initRootPath() {
		String sd = Environment.getExternalStorageDirectory().getAbsolutePath();
		
		pushPath(sd + "/Download", 0);
		pushPath(sd + "/Download/mrp", 0);
	}
}
