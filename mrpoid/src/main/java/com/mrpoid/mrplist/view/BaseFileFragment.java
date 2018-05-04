/*
 * Copyright (C) 2013 The Mrpoid Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.mrpoid.mrplist.view;

import java.io.File;
import java.io.FileFilter;
import java.util.List;
import java.util.Stack;

import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.LoaderManager.LoaderCallbacks;
import android.support.v4.content.Loader;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.ListView;

import com.mrpoid.mrpliset.R;
import com.mrpoid.mrplist.moduls.FileListLoader;
import com.mrpoid.mrplist.moduls.MpFile;
import com.mrpoid.mrplist.moduls.MpListAdapter;


/**
 * 文件浏览
 * 
 * @author Yichou 2013-12-19
 *
 */
public abstract class BaseFileFragment extends MyListFragment 
	implements LoaderCallbacks<List<MpFile>>, OnItemLongClickListener {
	static final String TAG = "BaseListFragment";
	static final char PATH_SEP = File.separatorChar;

	MpListAdapter mAdapter;
	int mLongPressIndex;
	int mFocuseIndex;
	FileListLoader mLoader;
	
	
	
	private final Stack<String> mPathStack = new Stack<String>();
	private final Stack<Integer> mFocuseStack = new Stack<Integer>();
	protected final int mLoaderId;
	
	
	protected ActionBar getSupportActionBar() {
		return ((AppCompatActivity)getActivity()).getSupportActionBar();
	}
	
	public BaseFileFragment(int loaderId) {
		this.mLoaderId = loaderId;
	}
	
	/**
	 * 添加一个路径到堆栈，
	 * 
	 * @param path
	 * @param focuseIndex
	 */
	protected void pushPath(String path, int focuseIndex) {
		mPathStack.push(path);
		mFocuseStack.push(focuseIndex);
	}

	protected void pushPath(String path) {
		pushPath(path, getListView().getFirstVisiblePosition());
	}
	
	private void initPathStack() {
		mPathStack.clear();
		
		pushPath("/", 0); //root目录
		
		File sdPath = Environment.getExternalStorageDirectory();
		pushPath(sdPath.getParent() + PATH_SEP, 0); //sd卡所在目录
		pushPath(sdPath.getAbsolutePath() + PATH_SEP, 0); //sd卡根目录
	}
	
	protected abstract void initRootPath();
	
	public boolean isRootDir() { //根目录
		return mPathStack.size() == 1;
	}

	/**
	 * 进入下级目录
	 * 
	 * @param name
	 * @param position
	 * @return
	 */
	public String inDir(String name, int position) {
		String cur = mPathStack.peek() + name + PATH_SEP;
		
		pushPath(cur, position);
		
		refreshListInner(0); //新进入的页面 焦点在 0

		enableHomeUpBtn();
		
		return cur;
	}
	
	public void toPath(String path) {
		pushPath(path, 0);
		refreshListInner(0); //新进入的页面 焦点在 0
		enableHomeUpBtn();
	}

	/**
	 * 返回上级目录
	 * 
	 * @return
	 */
	public String outDir() {
		if (isRootDir()) {
			disableHomeUpBtn();
			return mPathStack.peek();
		}
		
		mPathStack.pop();
		refreshListInner(mFocuseStack.pop());
		
		return mPathStack.peek();
	}
	
	public String getCurrentPath() {
		return mPathStack.peek();
	}
	
	public void refreshList() {
		refreshListInner(getListView().getFirstVisiblePosition());
	}
	
	/**
	 * 重新加载列表
	 * 
	 * @param position
	 */
	private void refreshListInner(int position) {
		mFocuseIndex = position;
		
		setListShownNoAnimation(false); //没有动画隐藏 listView，显示旋转进度

		mLoader.reload(mPathStack.peek(), isRootDir());
		
		getSupportActionBar().setSubtitle(getCurrentPath());
	}
	
	private void enableHomeUpBtn() {
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);
	}
	
	private void disableHomeUpBtn() {
		getSupportActionBar().setDisplayHomeAsUpEnabled(false);
	}
	
	/**
	 * 只调用一次
	 */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		initPathStack();

		mAdapter = new MpListAdapter(getActivity());
		mLoader = (FileListLoader) getLoaderManager().initLoader(mLoaderId, null, this);
		
		initRootPath();
	}
	
	/**
	 * 每次销毁后调用
	 */
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		setListAdapter(mAdapter);
		
		ListView listView = getListView();
		listView.setOnItemLongClickListener(this);
		listView.setFastScrollEnabled(true);
		
		
		registerForContextMenu(listView);
		
		setListShown(false);
		setHasOptionsMenu(true);
	}
	
	@Override
	public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
//		SubMenu subOptions = menu.addSubMenu(0, R.id.mi_menu, 1, R.string.menu);
//		subOptions.setIcon(getHomeActivity().isLightTheme() ? R.drawable.ic_menu : R.drawable.ic_menu_dark);
//		subOptions.getItem().setShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);

		SubMenu subOptions = menu.addSubMenu(R.id.mi_group_mrplist, R.id.mi_refresh, 1, R.string.refresh);
		subOptions.setIcon(R.drawable.ic_refresh);
		subOptions.getItem().setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
		
		subOptions = menu.addSubMenu(R.id.mi_group_mrplist, R.id.mi_download, 1, R.string.download);
		subOptions.getItem().setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
		subOptions.setIcon(android.R.drawable.stat_sys_download_done);
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if(item.getGroupId() == R.id.mi_group_mrplist) {
			if(item.getItemId() == R.id.mi_refresh) {
				refreshList();
				return true;
			} else if (item.getItemId() == R.id.mi_download) {
				toPath(Environment.getExternalStorageDirectory().getAbsolutePath() + "/mythroad/");
				return true;
			}
		}
		
		return super.onOptionsItemSelected(item);
	}
	
	@Override
	public void onViewCreated(View view, Bundle savedInstanceState) {
		super.onViewCreated(view, savedInstanceState);

		refreshList();
	}
	
	@Override
	public void onResume() {
		super.onResume();
	}
	
	@Override
	public void onDestroyView() {
		unregisterForContextMenu(getListView());
		
		super.onDestroyView();
	}

	@Override
	public Loader<List<MpFile>> onCreateLoader(int id, Bundle data) {
		return new FileListLoader(getActivity(), 
				mPathStack.peek(), 
				isRootDir(),
				getFileFilter());
	}
	
	protected abstract FileFilter getFileFilter();

	@Override
	public void onLoadFinished(Loader<List<MpFile>> loader, List<MpFile> list) {
		/**
		 * 真正返回给UI层的列表，不能在后台线程修改，
		 * 我们不知道 加载器会不会对 这个 list 重用
		 * 所有我们复制他的数据到一个安全的地方
		 */
		mAdapter.setData(list);

		if (isResumed()) {
			setListShown(true);
		} else {
			setListShownNoAnimation(true);
		}
		getListView().setSelection(mFocuseIndex);
	}

	@Override
	public void onLoaderReset(Loader<List<MpFile>> loader) {
		if(!getActivity().isFinishing() && isVisible()) {
			mAdapter.setData(null);
			
			getListView().setSelection(mFocuseIndex);
		}
	}
	
	@Override
	public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
		mLongPressIndex = position;
		return false;
	}
	
	@Override
	public final void onListItemClick(ListView l, View v, int position, long id) {
		MpFile file = mAdapter.getItem(position);

		if (file.isParent()) { //返回上级标记
			outDir();
		} else if (file.isDir()) {
			inDir(file.getName(), position);
		} else {
			onItemClick(position, file);
		}
	}
	
	protected abstract boolean onItemClick(int position, MpFile file); 
}