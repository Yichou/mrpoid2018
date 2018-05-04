package com.mrpoid.mrplist.app;

import android.os.Bundle;
import android.support.v4.app.FragmentTransaction;
import android.view.View;

public class BrowserActivity extends BaseActivity {

	@Override
	protected void onCreate(Bundle arg0) {
		super.onCreate(arg0);
		
		FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
//		ft.replace(android.R.id.content, new BrowserFragment(), "main").commit();

		setTranslucentNavigation(true);
//		if(VERSION.SDK_INT >= VERSION_CODES.ICE_CREAM_SANDWICH) {
			View decorView = getWindow().getDecorView();
			// Hide the status bar.
			int uiOptions = View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION|View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
			decorView.setSystemUiVisibility(uiOptions);
//		}
	}
}
