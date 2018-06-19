package com.mrpoid;

import android.net.Uri;

/**
 * 通用资源管理类
 * 
 * @author Yichou
 *
 */
public final class Res {
	public static final Uri FAQ_URI = Uri.parse("http://mrpej.com/YichouAngle/mrpoid/faq.html");
	public static final Uri CHANGELOG_URI = Uri.parse("http://mrpej.com/YichouAngle/mrpoid/changelog.html");

	public static final Uri FAQ_URI_ASSET = Uri.parse("file:///android_asset/faq.html");
	public static final Uri CHANGELOG_URI_ASSET = Uri.parse("file:///android_asset/changelog.html");
	public static final Uri ABOUT_URI_ASSET = Uri.parse("file:///android_asset/about.html");
	public static final Uri HELP_URI_ASSET = Uri.parse("file:///android_asset/help.html");
}
