package com.mrpoid.mrplist.view;

import java.io.FileFilter;

import com.mrpoid.mrplist.moduls.MpFile;

import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;



/**
 * 本地应用列表
 * 
 * @author Yichou 2013-12-19
 *
 */
public class ExplorerFragment extends BaseMrpListFragment {
	
	public ExplorerFragment() {
		super(2);
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		mAdapter.setColors(new int[]{0xfff0f0f0, 0x80f0f0f0, 0xa000f000});
	}
	
	@Override
	public void onViewCreated(View view, Bundle savedInstanceState) {
		super.onViewCreated(view, savedInstanceState);
		
		getListView().setFitsSystemWindows(true);
	}

	@Override
	protected void initRootPath() {
//		pushPath(Emulator.getInstance().getVmFullPath(), 0);	
	}
	
	@Override
	public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
		super.onCreateOptionsMenu(menu, inflater);
	}

	@Override
	protected FileFilter getFileFilter() {
		return super.getFileFilter();
	}

	
}
