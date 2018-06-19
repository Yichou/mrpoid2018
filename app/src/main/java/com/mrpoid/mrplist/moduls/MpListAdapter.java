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
package com.mrpoid.mrplist.moduls;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.mrpoid.mrpliset.R;


/**
 * 列表适配器
 * 
 * @author Yichou 2013-11-23
 *
 */
public final class MpListAdapter extends BaseAdapter {
	private Context context;
	private LayoutInflater mInflater;
	private final List<MpFile> mList = new ArrayList<MpFile>(32);
	private int[] colors;

	
	public MpListAdapter(Activity activity) {
		this.context = activity;
		mInflater = (LayoutInflater) activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}
	
	public void setColors(int[] colors) {
		this.colors = colors;
	}
	
	public void setData(List<MpFile> newData) {
		mList.clear();
		if(newData != null) {
			mList.addAll(newData);
		}
		notifyDataSetChanged();
	}
	
	public void addData(MpFile file) {
		mList.add(file);
		notifyDataSetChanged();
	}

	public List<MpFile> getData() {
		return mList;
	}
	
	/**
	 * 移除条目
	 * 
	 * @param position
	 */
	public void remove(int position) {
		mList.remove(position);
		
		notifyDataSetChanged();
	}
	
	@Override
	public int getCount() {
		return mList.size();
	}

	@Override
	public MpFile getItem(int position) {
		return mList.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHoder hoder;

		MpFile file = mList.get(position);

		if (convertView == null) {
			convertView = mInflater.inflate(R.layout.list_item, null);
			hoder = new ViewHoder();

			hoder.icon = (ImageView) convertView.findViewById(R.id.imageView1);
			hoder.viewDiv = convertView.findViewById(R.id.viewDiv);
			
			hoder.tv_title = (TextView) convertView.findViewById(R.id.textView1);
			hoder.tv_msg = (TextView) convertView.findViewById(R.id.textView2);
			hoder.tv_size = (TextView) convertView.findViewById(R.id.textView3);
			
			convertView.setTag(hoder);
		} else {
			hoder = (ViewHoder) convertView.getTag();
		}

		hoder.tv_title.setText(file.getTtile());
		hoder.tv_msg.setText(file.getMsg());
		hoder.tv_size.setText(file.getSizeString());
		hoder.icon.setImageResource(file.getType().getIconRes());
		
//		if(colors != null) {
//			hoder.tv_title.setTextColor(colors[0]);
//			hoder.tv_msg.setTextColor(colors[1]);
//			hoder.tv_size.setTextColor(colors[2]);
//		}
		
		if(position == getCount()-1) {
			hoder.viewDiv.setVisibility(View.GONE);
		} else {
			hoder.viewDiv.setVisibility(View.VISIBLE);
		}

		return convertView;
	}

	// 保存视图
	static final class ViewHoder {
		TextView tv_title, tv_msg, tv_size;
		ImageView icon;
		View viewDiv;
	}
}