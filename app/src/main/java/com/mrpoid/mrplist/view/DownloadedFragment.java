package com.mrpoid.mrplist.view;

import android.os.Environment;

/**
 * 已下载
 * 
 * @author YYichou 2014-06-21
 *
 */
public class DownloadedFragment extends BaseMrpListFragment {

	@Override
	protected void initRootPath() {
		String sd = Environment.getExternalStorageDirectory().getAbsolutePath();
		
		pushPath(sd + "/Download");
	}
}
