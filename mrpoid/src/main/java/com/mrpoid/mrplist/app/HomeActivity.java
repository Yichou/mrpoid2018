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
package com.mrpoid.mrplist.app;

import com.mrpoid.MrpoidMain;
import com.mrpoid.app.HelpActivity;
import com.mrpoid.app.MrpoidSettingsActivity;
import com.mrpoid.mrpliset.R;
import com.mrpoid.mrplist.moduls.MrpInfo;
import com.mrpoid.mrplist.moduls.MyFavoriteManager;
import com.mrpoid.mrplist.moduls.PreferencesProvider;
import com.mrpoid.mrplist.utils.MrpUtils;
import com.mrpoid.mrplist.view.BaseFileFragment;
import com.mrpoid.mrplist.view.DownloadedFragment;
import com.mrpoid.mrplist.view.ExplorerFragment;
import com.mrpoid.mrplist.view.LocalmrpFragment;
import com.mrpoid.mrplist.view.MyFavoriteFragment;
import com.mrpoid.mrplist.view.SlidingTabLayout;
import com.mrpoid.utils.UmengUtils;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Build.VERSION;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;


/**
 * 文件列表 2014-7-15
 */
public class HomeActivity extends AppCompatActivity implements OnClickListener,
	OnPageChangeListener {
	public static final String TAG = "HomeActivity";
	
	public static final String SHARE_URL = 
			"http://mrpej.com/wapindex.aspx?siteid=1000&classid=36&sid=[sid]";
	public static final String FADEBACK_URL = 
			"http://mrpej.com/bbs/book_view_add.aspx?page=1&classid=847&siteid=1000";

	private static final int DEFAULT_BACKGROUND_INDEX = 5;
	private static final int[] BACKGROUND_IMGS = {
		android.R.color.transparent,
		R.drawable.wp1,
		R.drawable.wp2,
		R.drawable.wp3,
		R.drawable.wp4,
		R.drawable.wp5
	};
	
	private static final String[] PAGE_TITLES = {
			"最近打开", "本地浏览"
	};
	
	private MyFavoriteFragment favoriteFmg;
	private BaseFileFragment listFmg;
	private DownloadedFragment downloadedFragment;
	private LocalmrpFragment localmrpFragment;
	private boolean needRefresh = false;
	private ViewPager mPager;
	private View mStartDrawer;
	private ListView mMenuListView;
	private DrawerLayout mDrawerLayout;
	private ActionBarHelper mActionBar;
	private ActionBarDrawerToggle mDrawerToggle;
	
	
    
    @Override
	protected void onCreate(Bundle arg0) {
//		int themeColor = PreferencesProvider.Interface.General.getThemeColor(0);
//		if(themeColor == 1) {
			setTheme(R.style.AppTheme);
//		} else {
//			setTheme(R.style.Theme_AppCompat);
//		}
    	super.onCreate(arg0);
		
		setContentView(R.layout.activity_home);
		
//		Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
//		if(toolbar != null)
//			setSupportActionBar(toolbar);
		
		getSupportActionBar().setDisplayUseLogoEnabled(true);
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);
//		getSupportActionBar().setBackgroundDrawable(new ColorDrawable(0x80222222));
		
//		PAGE_TITLES = getResources().getStringArray(R.array.page_titles);
		
		mPager = (ViewPager) findViewById(R.id.pager);
		mPager.setAdapter(new MyFragmentPagerAdapter(getSupportFragmentManager()));
		
		
		SlidingTabLayout slidingTabLayout = (SlidingTabLayout) findViewById(R.id.sliding_tabs);
		slidingTabLayout.setCustomTabView(R.layout.tab_indicator, android.R.id.text1);
		slidingTabLayout.setSelectedIndicatorColors(getResources().getColor(R.color.nliveo_teal_colorAccent));
		slidingTabLayout.setDistributeEvenly(true);
		slidingTabLayout.setViewPager(mPager);
		
//		mPageIndicator = (TitlePageIndicator)findViewById(R.id.indicator);
//		mPageIndicator.setViewPager(mPager);
//		mPageIndicator.setOnPageChangeListener(this);
//		mPageIndicator.setFooterIndicatorStyle(IndicatorStyle.Underline);
//		mPageIndicator.setOnCenterItemClickListener(this);
//		mPageIndicator.setCurrentItem(0);
//		mPageIndicator.setFooterColor(0xf0e1e5ee);
 
		//创建
		favoriteFmg = new MyFavoriteFragment();
		listFmg = new ExplorerFragment();
		downloadedFragment = new DownloadedFragment();
		localmrpFragment = new LocalmrpFragment();
		
		//drawlayout
		initLeftDrawer();
		initBackground(); 
		
//		runOnUiThread(new Runnable() {
//
//			@Override
//			public void run() {
//				File file = new File(Environment.getExternalStorageDirectory(), "mythroad/240x320/gwy.mrp");
//				MrpoidMain.runMrp(getActivity(), file.getPath());
//			}
//		});
	}
    
    private void initLeftDrawer() {
    	mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
		mDrawerLayout.setDrawerListener(new DemoDrawerListener());
		
		mStartDrawer = findViewById(R.id.start_drawer);
		mMenuListView = (ListView) findViewById(R.id.listView1);
		mMenuListView.setAdapter(new ArrayAdapter<String>(this, 
				R.layout.drawer_list_item, R.id.title, 
				getResources().getStringArray(R.array.main_menu_items)));
		mMenuListView.setOnItemClickListener(new DrawerItemClickListener());
		mActionBar = createActionBarHelper();
		mActionBar.init();
		mDrawerToggle = new ActionBarDrawerToggle(getActivity(), mDrawerLayout, R.string.drawer_open, R.string.drawer_close);
		mDrawerToggle.syncState();
		
		try {
			PackageInfo info;
			info = getPackageManager().getPackageInfo(getPackageName(), 0);
			((TextView)findViewById(R.id.tvVersion)).setText("版本: " + info.versionName);
		} catch (Exception e) {
		}
	}
    
    public void setCurrentPage(int page) {
		mPager.setCurrentItem(page);
	}
	
	private void initBackground() {
		int img = PreferencesProvider.Interface.General.getThemeImage(DEFAULT_BACKGROUND_INDEX);
//		if(img != 0)
//			setBackground(img, true);
	}
	
	private void setBackground(int img) {
		setBackground(img, false);
	}
	private void setBackground(int img, boolean foce) {
		if(!foce) {
			int oldimg = PreferencesProvider.Interface.General.getThemeImage(DEFAULT_BACKGROUND_INDEX);
			if(oldimg == img)
				return;
		}
		
		if(img < 0 || img >= BACKGROUND_IMGS.length)
			img = DEFAULT_BACKGROUND_INDEX;

		PreferencesProvider.Interface.General.setThemeImage(this, img);
		if(img == 0) {
			reStartSelf();
		} else {
			getWindow().setBackgroundDrawableResource(BACKGROUND_IMGS[img]);
		}
	}
	
	@Override
	public void onClick(View v) {
	}
	
	
	@Override
	public void onPageScrollStateChanged(int arg0) {
	}

	@Override
	public void onPageScrolled(int arg0, float arg1, int arg2) {
	}

	@Override
	public void onPageSelected(int page) {
		setSubTitle(PAGE_TITLES[page]);
	}
	
	public final class MyFragmentPagerAdapter extends FragmentPagerAdapter {
		public MyFragmentPagerAdapter(FragmentManager fm) {
			super(fm);
		}

		@Override
		public Fragment getItem(int arg0) {
			/*if(arg0 == 0) {
				return localmrpFragment;
			} else */if(arg0 == 0) {
				return favoriteFmg;
			} else if(arg0 == 1) {
				return listFmg;
			} /*else if(arg0 == 3) {
				return downloadedFragment;
			}*/
			
			return null;
		}

		@Override
		public int getCount() {
			return PAGE_TITLES.length;
		}

		@Override
		public CharSequence getPageTitle(int position) {
			return PAGE_TITLES[position % PAGE_TITLES.length];
		}
	}
	
	@SuppressLint("NewApi")
	private void reStartSelf() {
		if(VERSION.SDK_INT >= 11) {
			recreate();
		} else {
			finish();
			
			startActivity(new Intent(this, getClass())
					.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION)
					.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP));
		}
	}
	
	private void showShare() {
		Intent intent = new Intent(Intent.ACTION_SEND);
		intent.setType("text/plain");
		intent.putExtra(Intent.EXTRA_SUBJECT, getString(R.string.share));
		intent.putExtra(Intent.EXTRA_TEXT, getString(R.string.share_msg) + "\n" + SHARE_URL);
		startActivity(Intent.createChooser(intent, getTitle()));
	}
	
    private class DrawerItemClickListener implements ListView.OnItemClickListener {
       
    	@Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
    		switch (position) {
			case 0: {
				startActivity(new Intent(getActivity(), MrpoidSettingsActivity.class));
				break;
			}
			case 1: {
				HelpActivity.show(getActivity(), Uri.parse("http://edroid.sinaapp.com/pages/mrpoid-about.php"));
				break;
			}
			case 2: {
				showShare();
				break;
			}
			case 3: {
				break;
			}
			case 4: {
				break;
			}
			case 5: {
				startActivity(new Intent(getActivity(), BrowserActivity.class).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
				break;
			}
			case 6: {
				startActivity(new Intent(getActivity(), FileManagerActivity.class).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));
				break;
			}

			default:
				break;
			}
    		
            mDrawerLayout.closeDrawer(mStartDrawer);
        }
    }
	
	@Override
	protected void onPause() {
		UmengUtils.onPause(getActivity());

		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		Log.i(TAG, "onResume");

		UmengUtils.onResume(getActivity());

		againToExit = false;
		
		if (needRefresh) {
			// 刷新列表
			if (listFmg != null)
				listFmg.refreshList();
			needRefresh = false;
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	private boolean againToExit;

	@Override
	public void onBackPressed() {
		if (!againToExit) {
			againToExit = true;
			Toast.makeText(this, R.string.hint_again_to_exit, Toast.LENGTH_SHORT).show();
		} else {
			finish();
		}
	}

	public boolean isLightTheme() {
		return false;
	}
	
	public static void addToFavorate(String path) {
		MyFavoriteManager.getInstance().add(path);
	}
	
	public void runMrp(String path) {
		MrpoidMain.runMrp(this, path);
		addToFavorate(path);
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		SubMenu subSkin = menu.addSubMenu(0, R.id.mi_theme, 0, R.string.theme);
		subSkin.setIcon(isLightTheme() ? R.drawable.ic_theme : R.drawable.ic_theme_drak);
		subSkin.getItem().setShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
		
		SubMenu subColor = subSkin.addSubMenu(0, 0, 0, R.string.color);
		subColor.add(R.id.mi_skin_color, R.id.mi_theme_black, 0, R.string.dark);
		subColor.add(R.id.mi_skin_color, R.id.mi_theme_light, 1, R.string.light);
		
		String[] ss = getResources().getStringArray(R.array.bg_imgs);
		SubMenu subBg = subSkin.addSubMenu(0, 1, 1, R.string.image);
		for(int i=0; i<BACKGROUND_IMGS.length; ++i){
			subBg.add(R.id.mi_skin_bg, 1000 + i, i, ss[i]);
		}

		return true;
	}
	
	Activity getActivity() {
		return this;
	}
	
	public void setSubTitle(String subTitle) {
		getSupportActionBar().setSubtitle(subTitle);
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if (mDrawerToggle.onOptionsItemSelected(item)) {
			return true;
		}

		if (item.getGroupId() == R.id.mi_skin_color) {
			if (item.getItemId() == R.id.mi_theme_black) {
				PreferencesProvider.Interface.General.setThemeColor(this, 0);
				reStartSelf();
				return true;
			} else if (item.getItemId() == R.id.mi_theme_light) {
				PreferencesProvider.Interface.General.setThemeColor(this, 1);
				reStartSelf();
				return true;
			}
		} else if (item.getGroupId() == R.id.mi_skin_bg) {
			setBackground(item.getItemId() - 1000);
			return true;
		}

		return false;
	}
	
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		
		mDrawerToggle.onConfigurationChanged(newConfig);
	}
	
	private ActionBarHelper createActionBarHelper() {
		return new ActionBarHelper();
	}
	
	private class DemoDrawerListener implements DrawerLayout.DrawerListener {
	       
    	@Override
        public void onDrawerOpened(View drawerView) {
            mDrawerToggle.onDrawerOpened(drawerView);
            mActionBar.onDrawerOpened();
        }

        @Override
        public void onDrawerClosed(View drawerView) {
            mDrawerToggle.onDrawerClosed(drawerView);
            mActionBar.onDrawerClosed();
        }

        @Override
        public void onDrawerSlide(View drawerView, float slideOffset) {
            mDrawerToggle.onDrawerSlide(drawerView, slideOffset);
        }

        @Override
        public void onDrawerStateChanged(int newState) {
            mDrawerToggle.onDrawerStateChanged(newState);
        }
    }
	
	private class ActionBarHelper {
		private final ActionBar mActionBar;
		private CharSequence mDrawerTitle;
		private CharSequence mTitle;

		private ActionBarHelper() {
			mActionBar = getSupportActionBar();
		}

		public void init() {
			mActionBar.setDisplayHomeAsUpEnabled(true);
			mActionBar.setHomeButtonEnabled(true);
			mTitle = mDrawerTitle = getActivity().getTitle();
		}

		/**
		 * When the drawer is closed we restore the action bar state reflecting
		 * the specific contents in view.
		 */
		public void onDrawerClosed() {
			mActionBar.setTitle(mTitle);
		}

		/**
		 * When the drawer is open we set the action bar to a generic title. The
		 * action bar should only contain data relevant at the top level of the
		 * nav hierarchy represented by the drawer, as the rest of your content
		 * will be dimmed down and non-interactive.
		 */
		public void onDrawerOpened() {
			mActionBar.setTitle(mDrawerTitle);
		}

		public void setTitle(CharSequence title) {
			mTitle = title;
		}
	}
	
	////////////////////////////////////////////////////////////////////////////
	public static void showRunMrpModeDialogFragment(FragmentActivity activity, String path) {
		RunMrpModeDialogFragment fragment = new RunMrpModeDialogFragment();
		
		Bundle bundle = new Bundle();
		bundle.putString("path", path);
		fragment.setArguments(bundle);
		fragment.show(activity.getSupportFragmentManager(), "RunMrpModeDialog");
	}
	
	public static class RunMrpModeDialogFragment extends DialogFragment {
		
		private void runMrp(int m) {
			String path = getArguments().getString("path");
			MrpoidMain.runMrp(getActivity(), path, m, true);
		}
		
		@Override
		public Dialog onCreateDialog(Bundle savedInstanceState) {
			return new AlertDialog.Builder(getActivity())
				.setItems(R.array.run_mode_entries, new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, int which) {
						runMrp(which);
					}
				})
				.create();
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////
	public void showMrpInfoDialog(String path) {
		MrpInfoDialogFragment fragment = new MrpInfoDialogFragment();
		
		Bundle bundle = new Bundle();
		bundle.putString("path", path);
		fragment.setArguments(bundle);
		fragment.show(getSupportFragmentManager(), "MrpInfoDialog");
	}
	
	public static class MrpInfoDialogFragment extends DialogFragment {
		
		private void read(View v) {
			String path = getArguments().getString("path");
			MrpInfo info = MrpUtils.readMrpInfo(path);
			
			((EditText)v.findViewById(R.id.editText1)).setText(info.label);
			((EditText)v.findViewById(R.id.editText2)).setText(info.name);
			((EditText)v.findViewById(R.id.editText3)).setText(info.vendor);
			((EditText)v.findViewById(R.id.editText4)).setText(String.valueOf(info.version));
			((EditText)v.findViewById(R.id.editText5)).setText(String.valueOf(info.appid));
			((EditText)v.findViewById(R.id.editText6)).setText(info.detail);
		}
		
		@Override
		public Dialog onCreateDialog(Bundle savedInstanceState) {
			LayoutInflater inflater = LayoutInflater.from(getActivity());
			View view = inflater.inflate(R.layout.dialog_mrpinfo, null);
			read(view);
			
			return new AlertDialog.Builder(getActivity())
				.setTitle(R.string.mrpinfo)
				.setView(view)
				.create();
		}
	}
}
