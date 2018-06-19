package com.mrpoid.app;

import java.io.File;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.Uri;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.view.MenuItem;
import android.widget.Toast;

import com.mrpoid.R;
import com.mrpoid.core.Emulator;
import com.mrpoid.core.MrpoidSettings;
import com.mrpoid.Res;
import com.mrpoid.game.keypad.KeypadEditActivity;
import com.edroid.common.utils.FileUtils;
import com.edroid.common.utils.SdkUtils;
import com.mrpoid.view.PathPreference;

/**
 * 
 * @author Yichou
 *
 */
public class MrpoidSettingsActivity extends Activity {
	static final String TAG = MrpoidSettingsActivity.class.getSimpleName();
	
	public static class MyPrefsFragement extends PreferenceFragment implements OnPreferenceChangeListener,
			 OnSharedPreferenceChangeListener {
		
		private PathPreference epMythroad, epSD;
		private CheckBoxPreference chkpMulti;
		private CheckBoxPreference chkPrivate;
		private ListPreference lpScnSize;
		private String oldScnSize;
		private SharedPreferences sp;
		
//		Context getContext() {
//			return getActivity();
//		}
		
		@Override
		public void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			
			addPreferencesFromResource(R.xml.emu_preferences);
			
			sp = PreferenceManager.getDefaultSharedPreferences(getActivity());
			sp.registerOnSharedPreferenceChangeListener(this);
			
			// findPreference("version").setOnPreferenceClickListener(this);
			// findPreference("advanced").setOnPreferenceClickListener(this);
			// findPreference("checkUpdate").setOnPreferenceClickListener(this);
			
			try {
				findPreference("keypadLayout").setIntent(new Intent(getActivity(), KeypadEditActivity.class));
			} catch (Exception e) {
			}
			
			Intent intent = new Intent(getActivity(), HelpActivity.class);
			intent.setData(Uri.parse(getString(R.string.setup_wizard_uri)));
			findPreference("setupWizard").setIntent(intent);
			
			Intent intent2 = new Intent(getActivity(), HelpActivity.class);
			intent2.setData(Res.CHANGELOG_URI_ASSET);
			findPreference("version").setIntent(intent2);
			

			chkpMulti = (CheckBoxPreference) findPreference(MrpoidSettings.kMultiPath);
			chkpMulti.setOnPreferenceChangeListener(this);
			
			Emulator emulator = Emulator.getInstance();
			
			epSD = (PathPreference) findPreference(MrpoidSettings.kSdcardPath);
			epSD.setSummary(emulator.getVmRootPath());
			epSD.setOnPreferenceChangeListener(this);
			epSD.setEnabled(FileUtils.isSDMounted());
			epSD.setDefaultValue(Emulator.SDCARD_ROOT);
			
			{
				String sd = Emulator.SDCARD_ROOT;
				
				int l = sd.length();
				if (sd.charAt(l - 1) == File.separatorChar) {
					sd = sd.substring(0, l - 1);
				}
				
				String path = sd;
				String dir = null;
				int i = sd.lastIndexOf(File.separatorChar);
				if (i != -1) {
					path = sd.substring(0, i + 1); // 保留 /结尾
					dir = sd.substring(i + 1);
					dir.concat(File.separator);
				}
				
				epSD.setDefRoot(path);
				epSD.setDefDir(dir);
			}
			
			epMythroad = (PathPreference) findPreference(MrpoidSettings.kMythroadPath);
			epMythroad.setSummary(emulator.getVmWorkPath());
			epMythroad.setOnPreferenceChangeListener(this);
			epMythroad.setEnabled(!chkpMulti.isChecked());
			epMythroad.setDefaultValue(Emulator.DEF_WORK_PATH);
			// if(!chkpMulti.isChecked())
			// epMythroad.setPath(emuPath.getMythroadPath());
			{
				epMythroad.setDefRoot(emulator.getVmRootPath());
				epMythroad.setDefDir(emulator.getVmWorkPath());
			}
			
			chkPrivate = (CheckBoxPreference) findPreference(MrpoidSettings.kUsePrivateDir);
			chkPrivate.setOnPreferenceChangeListener(this);
			
			// if(VERSION.SDK_INT >= VERSION_CODES.HONEYCOMB) {
			// gets().setDisplayHomeAsUpEnabled(true);
			// }
			
			// Supp
			
		}
		
		@Override
		public void onDestroy() {
			super.onDestroy();
			
			sp.unregisterOnSharedPreferenceChangeListener(this);
		}
		
		@Override
		public boolean onPreferenceChange(Preference p, Object v) {
			// EmuLog.i(TAG, "changed("+p+", "+v+")");
			
			/*
			 * if (p == epMythroad) { epMythroad.setSummary((CharSequence) v); }
			 * else if (p == epSD) { epSD.setSummary((CharSequence) v);
			 * epMythroad.setDefRoot(emuPath.getSDPath()); } else
			 */
			if (MrpoidSettings.kVmMem.equals(p.getKey())) {
				p.setSummary((CharSequence) v + " M");
				
				return true;
			} else if (MrpoidSettings.kScreenSize.equals(p.getKey())) {
				p.setSummary((CharSequence) v);
			} else if (MrpoidSettings.kMultiPath.equals(p.getKey())) {
				epMythroad.setEnabled(!(Boolean) v);
				// String path = "";
				//
				// if((Boolean)v){//选中后
				// path = emuPath.getMythroadPath() + EmuScreen.getSizeTag();
				// }else {
				// path = EmuPath.DEF_MYTHROAD_DIR;
				// }
				//
				// epMythroad.setPath(path);
				// epMythroad.setSummary(path);
				return true;
			}
			
			return true;
		}
		
		@Override
		public boolean onPreferenceTreeClick(PreferenceScreen ps, Preference p) {
			if ("advanced".equals(p.getKey())) {
				if (!MrpoidSettings.notHintAdvSet) {
					new AlertDialog.Builder(getActivity()).setTitle(R.string.warn).setMessage("如果你不知道自己在干什么请离开这里！")
							.setNegativeButton(R.string.ok, null)
							.setPositiveButton("不在提醒", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									MrpoidSettings.notHintAdvSet = true;
								}
							}).create().show();
					;
				}
				
				return true;
			} else if (p == epMythroad) {
				if (MrpoidSettings.differentPath) {
					Toast.makeText(getActivity(), "请取消勾选 " + getString(R.string.run_under_multi_path) + " 选项!",
							Toast.LENGTH_SHORT).show();
				}
				
				return true;
			} else if ("restartMrpoid".equals(p.getKey())) {
				ActivityManager am = (ActivityManager) getActivity().getSystemService(ACTIVITY_SERVICE);
				am.killBackgroundProcesses(getActivity().getPackageName());
				
				// final Intent intent =
				// getActivity().getPackageManager().getLaunchIntentForPackage(getActivity().getPackageName());
				// intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
				// startActivity(intent);
			}
			
			return super.onPreferenceTreeClick(ps, p);
		}
		

		@Override
		public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
			
		}
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		getFragmentManager().beginTransaction().replace(android.R.id.content, new MyPrefsFragement()).commit();
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
	}
	
	// @SuppressWarnings("deprecation")
	// @Override
	// public void onBackPressed() {
	// if (chkpMulti.isChecked() && (oldScnSize != null &&
	// !oldScnSize.equals(lpScnSize.getValue()))) {
	// showDialog(2);
	// } else {
	// super.onBackPressed();
	// }
	// }
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			finish();
			return true;
			
		default:
			return super.onOptionsItemSelected(item);
		}
	}
	
	@SuppressWarnings("deprecation")
	@Override
	protected Dialog onCreateDialog(int id) {
		if (id == 1) {
			
		} else if (id == 2) {
			return new AlertDialog.Builder(this).setTitle(R.string.hint)
					.setMessage("您修改了屏幕尺寸，并且选择了 \"不同分辨率在不同目录下运行\"意味着之前下载过的冒泡游戏都需要重新下载！")
					.setNegativeButton(R.string.ok, new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							finish();
						}
					}).create();
		} else if (id == 3) {
			try {
				PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(),
						PackageManager.GET_ACTIVITIES);
				
				return new AlertDialog.Builder(this).setTitle(R.string.app_name)
						.setMessage("版本 " + packageInfo.versionName + "\n\n" + getString(R.string.cpoy_right))
						.setPositiveButton("确定", new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which) {
								// /...
							}
						}).create();
				
			} catch (NameNotFoundException e) {
				e.printStackTrace();
			}
			
		}
		
		return super.onCreateDialog(id);
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		
		SdkUtils.onPause(this);
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		
		SdkUtils.onResume(this);
	}
}
