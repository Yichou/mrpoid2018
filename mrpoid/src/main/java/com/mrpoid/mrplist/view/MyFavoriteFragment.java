package com.mrpoid.mrplist.view;

import java.util.List;

import com.mrpoid.MrpoidMain;
import com.mrpoid.mrpliset.R;
import com.mrpoid.mrplist.app.HomeActivity;
import com.mrpoid.mrplist.moduls.MpFile;
import com.mrpoid.mrplist.moduls.MpListAdapter;
import com.mrpoid.mrplist.moduls.MyFavoriteManager;

import android.graphics.Color;
import android.os.Bundle;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SubMenu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.ListView;


/**
 * 
 * @author yichou 2014-07-30
 *
 */
public class MyFavoriteFragment extends MyListFragment implements 
	OnItemLongClickListener {
	MpListAdapter mAdapter;
	int pressedPosition;
	
	
	protected HomeActivity getHomeActivity() {
		return (HomeActivity)getActivity();
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		mAdapter = new MpListAdapter(getActivity());
		setListAdapter(mAdapter);
		
		MyFavoriteManager.getInstance().read();
		
		setHasOptionsMenu(true);
	}
	
	@Override
	public void onResume() {
		super.onResume();
		
		refreshList();
	}
	
	void refreshList() {
		List<MpFile> list = MyFavoriteManager.getInstance().getAll();
		mAdapter.setData(list);
	}
	
	@Override
	public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
		pressedPosition = position;
		return false;
	}
	
	@Override
	public void onViewCreated(View view, Bundle savedInstanceState) {
		super.onViewCreated(view, savedInstanceState);
		
		ListView listView = getListView();
		listView.setOnItemLongClickListener(this);
		listView.setFastScrollEnabled(true);
		listView.setCacheColorHint(Color.TRANSPARENT);

		registerForContextMenu(getListView());
	}
	
	@Override
	public void onDestroyView() {
		unregisterForContextMenu(getListView());

		super.onDestroyView();
	}
	
	@Override
	public void onListItemClick(ListView l, View v, int position, long id) {
		MrpoidMain.runMrp(getActivity(), mAdapter.getItem(position).getPath());
	}
	
	@Override
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
		if(v == getListView()) {
			menu.add(5, R.id.mi_remove_from_list, 1, R.string.remove_from_list);
			menu.add(5, R.id.mi_run_mode, 2, R.string.run_mode);
			menu.add(5, R.id.mi_show_mrpinfo, 3, R.string.show_mrpinfo);
		} else {
			super.onCreateContextMenu(menu, v, menuInfo);
		}
	}
	
	@Override
	public boolean onContextItemSelected(MenuItem item) {
		if(item.getGroupId() == 5) {
			int id = item.getItemId();
			MpFile file = mAdapter.getItem(pressedPosition);
			
			if(id == R.id.mi_remove_from_list) {
				mAdapter.remove(pressedPosition);
				MyFavoriteManager.getInstance().remove(pressedPosition);
			} else if (id ==  R.id.mi_run_mode) {
				HomeActivity.showRunMrpModeDialogFragment(getActivity(), file.getPath());
			} else if (id ==  R.id.mi_show_mrpinfo) {
				getHomeActivity().showMrpInfoDialog(file.getPath());
			}
			
			return true;
		}
		
		return super.onContextItemSelected(item);
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
				refreshList();
				return true;
			}
		}
		
		return super.onOptionsItemSelected(item);
	}
}
