package com.mrpoid.mrplist.utils;

import java.io.File;

import android.app.Activity;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.Intent.ShortcutIconResource;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.database.Cursor;
import android.net.Uri;

public class ShortcutUtils {
	
	public static String getAppName(Context context) {
		String appName = "";
		try {
			PackageInfo info = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
			appName = context.getString(info.applicationInfo.labelRes);
		} catch (NameNotFoundException e) {
		}
		
		return appName;
	}
	
	public static int getAppIcon(Context context) {
		int appIcon = 0;
		try {
			PackageInfo info = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
			appIcon = info.applicationInfo.icon;
		} catch (NameNotFoundException e) {
		}
		
		return appIcon;
	}
	
	public static boolean hasShortcut(Context context, String title) {
		final ContentResolver cr = context.getContentResolver();
		final String AUTHORITY = "com.android.launcher.settings";
		final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/favorites?notify=true");
		
		Cursor c = cr.query(CONTENT_URI, new String[] { "title", "iconResource" }, "title=?", new String[] { title },
				null);
		if (c != null && c.getCount() > 0) {
			return true;
		}
		
		return false;
	}
	
	public boolean shortCutExist(Context context, String title) {
		boolean isInstallShortcut = false;
		final ContentResolver cr = context.getContentResolver();
		
		int versionLevel = android.os.Build.VERSION.SDK_INT;
		String AUTHORITY = "com.android.launcher2.settings";
		
		// 2.2以上的系统的文件文件名字是不一样的
		if (versionLevel >= 8) {
			AUTHORITY = "com.android.launcher2.settings";
		} else {
			AUTHORITY = "com.android.launcher.settings";
		}
		
		final Uri CONTENT_URI = Uri.parse("content://" + AUTHORITY + "/favorites?notify=true");
		Cursor c = cr.query(CONTENT_URI, new String[] { "title", "iconResource" }, "title=?", new String[] { title },
				null);
		
		if (c != null && c.getCount() > 0) {
			isInstallShortcut = true;
		}
		return isInstallShortcut;
	}
	
	public static void createShortCut(Activity app, String title, int icon, File path) {
		String componetName = "com.mrpoid.app.ExternActivity";
		// ComponentName comp = new ComponentName(this.getPackageName(),
		// this.getPackageName() + "." + this.getLocalClassName());
		// Intent shortcutIntent = new
		// Intent(Intent.ACTION_MAIN).setComponent(comp);
		// if(isAddShortCut(appName))
		// return;
		ComponentName comp = new ComponentName(app.getApplicationContext(), componetName);
		Intent shortcutIntent = new Intent(new Intent(Intent.ACTION_MAIN).setComponent(comp));
		
		// 加上这句能避免启动新的 Activity
		shortcutIntent.addCategory(Intent.CATEGORY_LAUNCHER);
		shortcutIntent.setData(Uri.fromFile(path));
		shortcutIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		
		Intent intent = new Intent();
		
		intent.putExtra(Intent.EXTRA_SHORTCUT_INTENT, shortcutIntent);
		intent.putExtra(Intent.EXTRA_SHORTCUT_NAME, title);
		ShortcutIconResource iconRes = Intent.ShortcutIconResource.fromContext(app, icon);
		intent.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE, iconRes);
		// 不创建重复快捷方式
		// intent.putExtra("duplicate", false);
		// 添加快捷方式
		intent.setAction("com.android.launcher.action.INSTALL_SHORTCUT");
		
		app.sendBroadcast(intent);
	}
}
