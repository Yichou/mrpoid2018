package com.mrpoid.mrplist.view;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import com.mrpoid.MrpoidMain;
import com.mrpoid.core.Emulator;
import com.mrpoid.mrpliset.R;
import com.mrpoid.mrplist.app.HomeActivity;
import com.mrpoid.mrplist.moduls.MpFile;
import com.mrpoid.mrplist.moduls.MpListAdapter;
import com.mrpoid.mrplist.moduls.MyFavoriteManager;
import com.mrpoid.mrplist.utils.ShortcutUtils;

import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.LoaderManager.LoaderCallbacks;
import android.support.v4.content.AsyncTaskLoader;
import android.support.v4.content.Loader;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.ListView;
import android.widget.TextView;

public class LocalmrpFragment extends Fragment 
	implements OnItemLongClickListener, OnItemClickListener, LoaderCallbacks<List<MpFile>>, OnClickListener {
	MpListAdapter mAdapter;
	int pressedPosition;
	MyLoader mLoader;
	ListView mListView;
	View mProgressContainer;
	TextView tvPath;
	int mLongPressIndex;

	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		mAdapter = new MpListAdapter(getActivity());
		mLoader = (MyLoader) getLoaderManager().initLoader(2000, null, this);
		
		setHasOptionsMenu(true);
	}
	
	public ListView getListView() {
		return mListView;
	}
	
	/**
	 * 每次销毁后调用
	 */
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);
		
		mListView.setAdapter(mAdapter);
		
		ListView listView = getListView();
		listView.setOnItemClickListener(this);
		listView.setOnItemLongClickListener(this);
		listView.setFastScrollEnabled(true);
		
		registerForContextMenu(listView);
		
		setListShown(false, true);
		setHasOptionsMenu(true);
		
//		mLoader.load();
	}
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View view = inflater.inflate(R.layout.fragment_localmrp, null);
		
//		view.findViewById(R.id.textView1).setOnClickListener(this);
		mListView = (ListView) view.findViewById(R.id.list);
		mProgressContainer = view.findViewById(R.id.progressContainer);
		tvPath = (TextView) view.findViewById(R.id.tvPath);
		
		return view;
	}
	
	@Override
	public void onViewCreated(View view, Bundle savedInstanceState) {
		super.onViewCreated(view, savedInstanceState);
		
//		LayoutParams p = view.getLayoutParams();
		int pad = getResources().getDimensionPixelSize(R.dimen.default_padding);
		view.setPadding(pad, 0, pad, 0);
		
		ListView listView = getListView();
		listView.setCacheColorHint(Color.TRANSPARENT);
		listView.setDivider(new ColorDrawable(Color.TRANSPARENT));
		listView.setDividerHeight(0);
		
		tvPath.setText(Emulator.getInstance().getVmFullPath());
		
		mLoader.load();
	}

	void setListShown(boolean shown, boolean animate) {
//		if (animate) {
//            mProgressContainer.startAnimation(AnimationUtils.loadAnimation(
//                    getActivity(), android.R.anim.fade_out));
//            mListView.startAnimation(AnimationUtils.loadAnimation(
//                    getActivity(), android.R.anim.fade_in));
//        } else {
//            mProgressContainer.clearAnimation();
//            mListView.clearAnimation();
//        }
//		
//		mListView.setVisibility(shown? View.VISIBLE : View.GONE);
//		mProgressContainer.setVisibility(!shown? View.VISIBLE : View.GONE);
	}

	private static class MyLoader extends AsyncTaskLoader<List<MpFile>> {

		public MyLoader(Context context) {
			super(context);
		}

		@Override
		public List<MpFile> loadInBackground() {
			List<MpFile> list = new ArrayList<MpFile>();
			findMrpFiles(new File(Emulator.getInstance().getVmFullPath()), list);
			return list;
		}
		
		public void load() {
			onContentChanged();
		}
		
		public void findMrpFiles(File path, List<MpFile> mCacheList) {
			File[] files = path.listFiles(BaseMrpListFragment.mrpFilter);

			if (files != null && files.length > 0) {
				for (File f : files) {
					if(f.isFile()) {
						mCacheList.add(new MpFile(f));
					} else if (f.isDirectory()) {
//						findMrpFiles(path, mCacheList);
					}
				}
			}
		}
	}

	@Override
	public Loader<List<MpFile>> onCreateLoader(int arg0, Bundle arg1) {
		return new MyLoader(getActivity());
	}

	@Override
	public void onLoadFinished(Loader<List<MpFile>> arg0, List<MpFile> arg1) {
		/**
		 * 真正返回给UI层的列表，不能在后台线程修改，
		 * 我们不知道 加载器会不会对 这个 list 重用
		 * 所有我们复制他的数据到一个安全的地方
		 */
		mAdapter.setData(arg1);
		
		if (isResumed()) {
			setListShown(true, true);
		} else {
			setListShown(true, false);
		}
	}

	@Override
	public void onLoaderReset(Loader<List<MpFile>> arg0) {
	}

	@Override
	public void onClick(View v) {
		if(v.getId() == R.id.textView1) {
			mLoader.forceLoad();
		}
	}
	
	protected HomeActivity getHomeActivity() {
		return (HomeActivity)getActivity();
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
		MpFile file = mAdapter.getItem(position);
		
		MrpoidMain.runMrp(getActivity(), file.getPath());
		
		MyFavoriteManager.getInstance().add(file.getPath());
	}
	
	@Override
	public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
		mLongPressIndex = position;
		return false;
	}
	
	@Override
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
		if(v == getListView()) {
			menu.add(0, R.id.mi_run_mode, 0, R.string.run_mode);
			menu.add(0, R.id.mi_add_favorite, 0, R.string.add_favorite);
			menu.add(0, R.id.mi_create_shortcut, 0, R.string.create_shortcut);
		} else {
			super.onCreateContextMenu(menu, v, menuInfo);
		}
	}
	
	@Override
	public boolean onContextItemSelected(android.view.MenuItem item) {
		MpFile file = mAdapter.getItem(mLongPressIndex);
		
		if (item.getItemId() == R.id.mi_create_shortcut) {
			ShortcutUtils.createShortCut(getActivity(), 
					file.getTtile(), 
					ShortcutUtils.getAppIcon(getActivity()),
					file.toFile());
		} else if (item.getItemId() == R.id.mi_add_favorite) {
			MyFavoriteManager.getInstance().add(file.getPath());
		} else if (item.getItemId() == R.id.mi_run_mode) {
			HomeActivity.showRunMrpModeDialogFragment(getActivity(), file.getPath());
		} else {
			return super.onContextItemSelected(item);
		}

		return true;
	}
	
	@Override
	public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
		SubMenu subOptions = menu.addSubMenu(R.id.mi_group_mrplist, R.id.mi_refresh, 1, R.string.refresh);
		subOptions.setIcon(getHomeActivity().isLightTheme() ? R.drawable.ic_refresh_drak : R.drawable.ic_refresh);
		subOptions.getItem().setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if(item.getGroupId() == R.id.mi_group_mrplist) {
			if(item.getItemId() == R.id.mi_refresh) {
				mLoader.load();
				return true;
			}
		}
		
		return super.onOptionsItemSelected(item);
	}
}
