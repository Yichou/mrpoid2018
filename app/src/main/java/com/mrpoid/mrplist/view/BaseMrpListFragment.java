package com.mrpoid.mrplist.view;

import java.io.File;
import java.io.FileFilter;

import com.mrpoid.MrpoidMain;
import com.mrpoid.mrpliset.R;
import com.mrpoid.mrplist.app.HomeActivity;
import com.mrpoid.mrplist.moduls.FileType;
import com.mrpoid.mrplist.moduls.MpFile;
import com.mrpoid.mrplist.utils.ShortcutUtils;

import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.View;

public abstract class BaseMrpListFragment extends BaseFileFragment {
	
	@Override
	protected FileFilter getFileFilter() {
		return mrpFilter;
	}
	
	public static final FileFilter mrpFilter = new FileFilter() {
		
		@Override
		public boolean accept(File f) {
			if (f.isDirectory()) {
				return  true; // PreferencesProvider.Interface.General.getShowDir(true)? isContainMrp(f) : false;
			} else if (f.isFile()) {
				String name = f.getName();
				int ss = name.lastIndexOf('.'); //最后一个 .
				if(ss != -1) {
					return name.regionMatches(true, ss, ".mrp", 0, 4);
				}
			}

			return false;
		}
	};

	/**
	 * 检测一个文件夹是否包含MRP文件
	 * 
	 * @param path
	 * @return
	 */
	public boolean isContainMrp(File path) {
		return true;
	}
	
	public boolean isContainMrp1(File path) {
		File[] files = path.listFiles(mrpFilter);

		if (files == null || files.length == 0)
			return false;

		for (File f : files) {
			if (f.isFile()) // 是文件就是 MRP
				return true;

			if (isContainMrp(f)) // 递归子目录
				return true;
		}

		return false;
	}
	
	private void delete(int position) {
		MpFile file = mAdapter.getItem(position);
		
		Log.d(TAG, "delete file = " + file.getName());
		
		if (file.isFile()) {
			if (file.toFile().delete()) {
				Log.i(TAG, "remove file suc!");
				mAdapter.remove(position);
			}
		} else {
			// 设置该目录为排除
		}
	}
	
	@Override
	protected boolean onItemClick(int position, MpFile file) {
		if (file.getType() == FileType.MRP) {
//			MrpRunner.runMrp(getActivity(), file.getPath());
			
			MrpoidMain.runMrp(getActivity(), file.getPath());
			
			HomeActivity.addToFavorate(file.getPath());
		}
		
		return true;
	}

	@Override
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
		if(v == getListView()) {
			MpFile file = mAdapter.getItem(mLongPressIndex);
			
			if(file.isFile()) {
				menu.add(0, R.id.mi_run_mode, 0, R.string.run_mode);
				menu.add(0, R.id.mi_remove, 0, R.string.remove);
				menu.add(0, R.id.mi_add_favorite, 0, R.string.add_favorite);
				menu.add(0, R.id.mi_create_shortcut, 0, R.string.create_shortcut);
			}
		} else {
			super.onCreateContextMenu(menu, v, menuInfo);
		}
	}

	@Override
	public boolean onContextItemSelected(android.view.MenuItem item) {
		MpFile file = mAdapter.getItem(mLongPressIndex);
		
		if (item.getItemId() == R.id.mi_remove) {
			delete(mLongPressIndex);
		} else if (item.getItemId() == R.id.mi_create_shortcut) {
			ShortcutUtils.createShortCut(getActivity(), 
					file.getTtile(), 
					ShortcutUtils.getAppIcon(getActivity()),
					file.toFile());
		} else if (item.getItemId() == R.id.mi_add_favorite) {
			HomeActivity.addToFavorate(file.getPath());
		} else if (item.getItemId() == R.id.mi_run_mode) {
			HomeActivity.showRunMrpModeDialogFragment(getFragmentManager(), file.getPath());
		} else {
			return super.onContextItemSelected(item);
		}

		return true;
	}
}