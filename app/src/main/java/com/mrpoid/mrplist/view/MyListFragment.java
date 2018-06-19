package com.mrpoid.mrplist.view;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.app.ListFragment;
import android.view.View;
import android.widget.ListView;

import com.mrpoid.mrpliset.R;

public class MyListFragment extends ListFragment {

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
	}
}
