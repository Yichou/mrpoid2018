package com.mrpoid.game.keysprite;

import java.io.File;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.mrpoid.R;
import com.mrpoid.core.Emulator;
import com.edroid.common.utils.FileUtils;


/**
 * 按键精灵编辑界面
 * 
 * @author Yichou 2013-8-31
 *
 */
public class KeySpriteEditorActivity extends Activity implements OnItemSelectedListener, OnClickListener {
	int selectedKey;
	EditText mEditText;
	String[] mKeyTitles;
	KeySprite mKeySprite;
	MyAdapter mAdapter;

	public KeySpriteEditorActivity getActivity() {
		return this;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_key_sprite);

		mKeySprite = new SampleKeySprite();
		mKeyTitles = getResources().getStringArray(R.array.key_titles);

		Spinner spinner = (Spinner) findViewById(R.id.spinner1);

		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, 
				android.R.layout.simple_spinner_item, 
				mKeyTitles);

		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		spinner.setAdapter(adapter);
		spinner.setOnItemSelectedListener(this);

		ListView listView = (ListView) findViewById(R.id.listView1);
		mAdapter = new MyAdapter();
		listView.setAdapter(mAdapter);

		findViewById(R.id.ibtn_add).setOnClickListener(this);
		mEditText = (EditText) findViewById(R.id.edit_time);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add(0, 1, 0, "保存");
		menu.add(0, 2, 0, "运行");

		return true;
	}

	@SuppressWarnings("deprecation")
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if (item.getItemId() == 1) {
			showDialog(1001);
		} else if (item.getItemId() == 2) {
			
		} else {
			return super.onOptionsItemSelected(item);
		}

		return true;
	}
	
	private void save(String name) {
		File dir = Emulator.getPublicFilePath("keySprites");
		FileUtils.createDir(dir);
		
		File file = new File(dir, name + ".xml");
		try {
			mKeySprite.toXml(file);
			Toast.makeText(getActivity(), "保存成功！", Toast.LENGTH_SHORT).show();
		} catch (Exception e) {
			Toast.makeText(getActivity(), "保存失败！", Toast.LENGTH_SHORT).show();
		}
	}

	@SuppressWarnings("deprecation")
	@Override
	protected Dialog onCreateDialog(int id) {
		if (id == 1001) {
			LayoutInflater inflater = LayoutInflater.from(this);
			View view = inflater.inflate(R.layout.dialog_single_edit, null);
			final EditText editText = (EditText) view.findViewById(R.id.editText1);

			Dialog dialog = new AlertDialog.Builder(this)
				.setTitle("请输入文件名：")
				.setView(view)
				.setPositiveButton("保存", new DialogInterface.OnClickListener() {

				@Override
				public void onClick(DialogInterface dialog, int which) {
					String name = editText.getText().toString();
					if (name.length() == 0)
						name = String.valueOf(System.currentTimeMillis());

					save(name);
				}
			}).setNegativeButton("取消", null).create();

			return dialog;
		}

		return super.onCreateDialog(id);
	}

	@Override
	public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
		selectedKey = position;
	}

	@Override
	public void onNothingSelected(AdapterView<?> parent) {
	}

	private void addSprite() {
		int time = 1000;
		try {
			time = Integer.valueOf(mEditText.getText().toString());
		} catch (Exception e) {
		}

		Sprite sprite = new Sprite(mKeyTitles[selectedKey], selectedKey, time);
		mKeySprite.add(sprite);
		mAdapter.notifyDataSetChanged();
	}

	@Override
	public void onClick(View v) {
		addSprite();
	}

	private class MyAdapter extends BaseAdapter implements OnClickListener {
		LayoutInflater mInflater;

		public MyAdapter() {
			mInflater = LayoutInflater.from(KeySpriteEditorActivity.this);
		}

		@Override
		public int getCount() {
			return mKeySprite.count();
		}

		@Override
		public Object getItem(int position) {
			return mKeySprite.get(position);
		}

		@Override
		public long getItemId(int position) {
			return mKeySprite.get(position).value;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			ViewHolder holder;

			if (convertView == null) {
				convertView = mInflater.inflate(R.layout.key_sprite_item, null);
				holder = new ViewHolder();
				holder.mTextView = (TextView) convertView.findViewById(R.id.textView1);
				holder.mButton = (ImageButton) convertView.findViewById(R.id.ibtn_remove);
				holder.mButton.setOnClickListener(this);

				convertView.setTag(holder);
			} else {
				holder = (ViewHolder) convertView.getTag();
			}

			Sprite sprite = mKeySprite.get(position);

			holder.mButton.setTag(position);
			holder.mTextView.setText(String.valueOf(position) + ". " + sprite.toString());

			return convertView;
		}

		final class ViewHolder {
			TextView mTextView;
			ImageButton mButton;
		}

		@Override
		public void onClick(View v) {
			mKeySprite.remove((Integer) v.getTag());
			notifyDataSetChanged();
		}
	}
}
