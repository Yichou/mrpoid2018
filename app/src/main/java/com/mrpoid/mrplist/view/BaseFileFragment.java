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

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.ListView;

import com.edroid.common.utils.TextUtils;
import com.edroid.common.utils.WorkThread;
import com.mrpoid.mrpliset.R;
import com.mrpoid.mrplist.moduls.MpFile;
import com.mrpoid.mrplist.moduls.MpListAdapter;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Stack;


/**
 * 文件浏览
 * 
 * @author Yichou 2013-12-19
 *
 */
public abstract class BaseFileFragment extends MyListFragment implements OnItemLongClickListener {
	static final String TAG = "BaseListFragment";
	private static final char PATH_SEP = File.separatorChar;


	ListView lv;
	MpListAdapter mAdapter;
	int mLongPressIndex;
	int mFocuseIndex;
	String curPath;
	int curP, curY;
	boolean isRootPath;
	ArrayList<MpFile> cacheList;

	private final Stack<String> mPathStack = new Stack<String>();

	
	protected ActionBar getSupportActionBar() {
		return ((AppCompatActivity)getActivity()).getSupportActionBar();
	}
	
	public BaseFileFragment() {
	}
	
	protected void pushPath(String path) {
        int p, y;
	    if(lv == null || lv.getChildCount() <= 0) {
            p = y = 0;
        } else {
            p = lv.getFirstVisiblePosition();
            y  = lv.getChildAt(0).getTop();
        }

		mPathStack.push(path + "," + p + "," + y);
	}

	void parsePath(String s) {
        String[] ss = s.split(",");
        curPath = ss[0];
        curP = Integer.parseInt(ss[1]);
        curY = Integer.parseInt(ss[2]);
    }

	private void initPathStack() {
		mPathStack.clear();
		
		pushPath("/"); //root目录

		File sdPath = Environment.getExternalStorageDirectory();
//		pushPath(sdPath.getParent()); //sd卡所在目录
        curPath = sdPath.getPath();
	}
	
	protected void initRootPath() {
    }
	
	public boolean isRootDir() { //根目录
		return mPathStack.size() == 1;
	}

	void inDir(String name, int position) {
        pushPath(curPath);
        curPath += "/" + name;
        curP = 0;
        curY = 0;
		reload();
	}

	//进入绝对路径
	public void toPath(String path) {
		curPath = path;
        curP = 0;
        curY = 0;
	    pushPath(path);
		reload();
	}

	void outDir() {
	    if(mPathStack.size() <= 1)
	        return;
		String s = mPathStack.pop();
        if (s == null || s.length() == 0) {
            curP = 0;
            curY = 0;
            curPath = "/";
        } else {
            parsePath(s);
        }
		reload();
	}
	
	public void reload() {
//        getSupportActionBar().setDisplayHomeAsUpEnabled(!isRootDir());
        getSupportActionBar().setSubtitle(curPath);

        load();
	}

	private WorkThread.ITask loadTask = new WorkThread.ITask() {

        @Override
        public Object onDo(Object... objects) {
            System.out.println("---------curPath=" + curPath);
            if(TextUtils.isEmpty(curPath))
                return null;

            if(cacheList == null)
                cacheList = new ArrayList<MpFile>();
            else
                cacheList.clear();
            if(!isRootPath)
                cacheList.add(new MpFile()); //..

            File[] files = new File(curPath).listFiles(getFileFilter());
            if (files != null && files.length > 0) {
                for (File f : files) {
                    cacheList.add(new MpFile(f));
                }
            }
            return cacheList;
        }

        @Override
        public void onResult(Object o) {
            /**
             * 真正返回给UI层的列表，不能在后台线程修改，
             * 我们不知道 加载器会不会对 这个 list 重用
             * 所有我们复制他的数据到一个安全的地方
             */
            mAdapter.setData(cacheList);
		    getListView().setSelectionFromTop(curP, curY);
        }
    };

	void load() {
		WorkThread.getDefault().postTask(loadTask);
	}

	/**
	 * 只调用一次
	 */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		initPathStack();
		mAdapter = new MpListAdapter(getActivity());
		initRootPath();
	}
	
	/**
	 * 每次销毁后调用
	 */
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		setListAdapter(mAdapter);

		lv = getListView();
		lv.setOnItemLongClickListener(this);
		lv.setFastScrollEnabled(true);
		registerForContextMenu(lv);

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
				reload();
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

		reload();
	}
	
	@Override
	public void onDestroyView() {
		unregisterForContextMenu(getListView());
		super.onDestroyView();
	}

	protected abstract FileFilter getFileFilter();

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