package com.mrpoid.mrplist.app;

import android.os.Bundle;
import android.support.v4.app.FragmentTransaction;

import com.mrpoid.mrpliset.R;
import com.mrpoid.mrplist.view.ExplorerFragment;

public class FileManagerActivity extends BaseActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
		ft.replace(android.R.id.content, new ExplorerFragment(), "main").commit();

		getSupportActionBar().setElevation(0);
		
//		getActionBar()
//		getWindow().setBackgroundDrawableResource(R.drawable.wp5);
//		getWindow().setBackgroundDrawable(new ColorDrawable(0xffffffff));
		
//		setTranslucentNavigation(true);
	}
}
