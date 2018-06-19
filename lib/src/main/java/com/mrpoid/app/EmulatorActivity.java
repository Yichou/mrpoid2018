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
package com.mrpoid.app;

import java.util.ArrayList;
import java.util.List;

import com.mrpoid.MrpoidMain;
import com.mrpoid.R;
import com.mrpoid.core.EmuConfig;
import com.mrpoid.core.EmuLog;
import com.mrpoid.core.EmuSmsManager;
import com.mrpoid.core.EmuView;
import com.mrpoid.core.Emulator;
import com.mrpoid.core.MrDefines;
import com.mrpoid.core.MrpoidSettings;
import com.mrpoid.game.keypad.Keypad;
import com.mrpoid.game.keypad.Keypad.OnKeyEventListener;
import com.mrpoid.game.keypad.KeypadView;
import com.mrpoid.game.keysprite.ChooserFragment;
import com.mrpoid.game.keysprite.KeyEventListener;
import com.mrpoid.game.keysprite.KeySprite;
import com.mrpoid.game.keysprite.OnChooseLitener;
import com.mrpoid.game.keysprite.Sprite;

import android.Manifest.permission;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Process;
import android.os.Vibrator;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.text.InputFilter;
import android.view.ContextMenu;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.TextView;
import android.widget.Toast;

/**
 * @author JianbinZhu
 *
 * 创建日期：2012/10/9
 * 
 * 最后修改：2013-3-14 20:06:44
 */
public class EmulatorActivity extends BaseActivity implements Handler.Callback, MrDefines, OnClickListener {
	static final String TAG = "EmulatorActivity";
	
	public static boolean SMS_DEL_MODE = false;
	
	
	public static String APP_ACTIVITY_NAME = "com.mrpoid.apps.AppActivity0";
	public static String APP_SERVICE_NAME = "com.mrpoid.apps.AppService0";
	
//	private static final int MSG_ID_SHOWEDIT = 1001;
	private static final int MSG_ID_UPDATE = 1002;
	private static final int MSG_ID_KEY_DOWN = 1012;
	private static final int MSG_ID_KEY_UP = 1003;
	private static final int MSG_ID_UPDATE_INFO_TEXT = 1004;
	
	private static final int INFO_TYPE_KEY_SPRITE = 1001;
	private static final int REQ_SHOWEDIT = 1001;
	private static final int REQ_GET_IMAGE = 1002;
	
	private TextView tvInfo;
	private EmuView emulatorView;
	private Emulator emulator;
	private EmuConfig cfg;
//	private SmsReceiver mSmsReceiver;
	private ViewGroup continer;
	public Handler handler;
	private LayoutInflater inflater;
	private Keypad keypad;
	private boolean mPaused;
	private Vibrator vibrator;
	
	String mEntryActivity, mEntryMrp;

	@Override
	public boolean handleMessage(Message msg) {
		switch (msg.what) {
		case MSG_ID_UPDATE: {
			if(mPaused)
				break;
			
			handler.sendEmptyMessageDelayed(MSG_ID_UPDATE, 1000);
			break;
		}
		
		case MSG_ID_UPDATE_INFO_TEXT: {
			String text = msg.obj==null? null : msg.obj.toString();
			if(text == null)
				tvInfo.setVisibility(View.INVISIBLE);
			else {
				if(tvInfo.getVisibility() != View.VISIBLE)
					tvInfo.setVisibility(View.VISIBLE);
				tvInfo.setText(text);
			}
			
			break;
		}
			
		default:
			return false;
		}

		return true;
	}
	
	@Override
	public void onClick(View v) {
		if(v.getId() == R.id.tv_info) {
			if(v.getTag().equals(INFO_TYPE_KEY_SPRITE)){
				stopKeySprite();
			}
		}
	}
	
	private void initEntrys() {
		String path = getIntent().getStringExtra(MrpoidMain.INTENT_KEY_ENTRY_MRP);
		String activity = getIntent().getStringExtra(MrpoidMain.INTENT_KEY_ENTRY_ACTIVITY);

		SharedPreferences appsp = getSharedPreferences(APP_ACTIVITY_NAME, 0);
		if(path == null) {
			path = appsp.getString(MrpoidMain.INTENT_KEY_ENTRY_MRP, null);
		} else {
			//保存路径，下次启动时候丢失也可以拿到
			appsp.edit().putString(MrpoidMain.INTENT_KEY_ENTRY_MRP, path).commit();
		}
		
		if(activity == null) {
			activity = appsp.getString(MrpoidMain.INTENT_KEY_ENTRY_ACTIVITY, null);
		} else {
			appsp.edit().putString(MrpoidMain.INTENT_KEY_ENTRY_ACTIVITY, activity).commit();
		}
		
		if(path == null || activity == null) {
			finish();
			Toast.makeText(this, "启动文件丢失！", Toast.LENGTH_SHORT).show();
			return;
		}
		
		mEntryMrp = path;
		mEntryActivity = activity;
	}
	
	private void setStatusBar() {
//		if(!MrpoidSettings.showStatusBar)
//			getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
//		else
//			getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
	}
	
	private void switchStatusBar() {
		MrpoidSettings.showStatusBar = !MrpoidSettings.showStatusBar;
		setStatusBar();
	}
	
	private EmulatorActivity getActivity() {
		return this;
	}
	
	void initPad() {
		keypad = new Keypad(this, false);
		
		View padView = new KeypadView(this, keypad);
		continer.addView(padView, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
		
		keypad.attachView(padView); //依赖一个 view 来展示
		keypad.setOnKeyEventListener(mKeyEventListener);
		keypad.setMode(2);
		keypad.setOpacity(EmuConfig.getInstance().padAlpha);
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_emulator);

		initEntrys();
//		getWindow().requestFeature(Window.FEATURE_NO_TITLE);//去掉标题栏
		
		handler = new Handler(this);
		inflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		vibrator = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
		
		emulator = Emulator.getInstance();
		cfg = EmuConfig.getInstance();

		emulatorView = new EmuView(this);
		emulatorView.setBackgroundColor(Color.TRANSPARENT);
		continer =  (ViewGroup) findViewById(R.id.contener);
		continer.addView(emulatorView, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
		registerForContextMenu(emulatorView);
		
		initPad();

		//内存显示
		{
			tvInfo = new TextView(this);
			tvInfo.setBackgroundColor(0x80000000);
			tvInfo.setTextColor(0xfff0f0f0);
			tvInfo.setTextSize(16);
			tvInfo.setGravity(Gravity.CENTER_VERTICAL|Gravity.CENTER_HORIZONTAL);
			tvInfo.setSingleLine(true);
			tvInfo.setVisibility(View.INVISIBLE);
			tvInfo.setId(R.id.tv_info);
			tvInfo.setOnClickListener(this);
			
			int padding = getResources().getDimensionPixelSize(R.dimen.dp5);
			tvInfo.setPadding(padding, padding, padding, padding);
			continer.addView(tvInfo, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
		}
		
		emulator.init(this, emulatorView);
		emulator.startMrp(mEntryMrp);
	}

	@Override
	protected void onPause() {
		EmuLog.i(TAG, "onPause");

		mPaused = true;
		emulator.pause();
		
		if (!isFinishing()) {
			entryBackground();
		} else {
			keypad.setOnKeyEventListener(null);
		}
		
		overridePendingTransition(0, 0);
		
//		MrpoidSettings.getInstance().tempSave();
		
		super.onPause();
	}
	
	@Override
	protected void onResume() {
		EmuLog.i(TAG, "onResume");
		
		mPaused = false;
		backFroground();
//		MrpoidSettings.getInstance().tempRead();
		emulator.resume();
		
		if(MrpoidSettings.showMemInfo)
			handler.sendEmptyMessageDelayed(MSG_ID_UPDATE, 1000);
		
		overridePendingTransition(0, 0);
		
		super.onResume();
		
		setStatusBar();
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();

		EmuLog.i(TAG, "onDestroy");

		if(emulator.isRunning()) { //说明在后台运行被杀了
			EmuLog.e(TAG, "后台运行被杀！");
		}

		emulator.onActivityDestroy();
	}
	
	@Override
	protected void onStop() {
		EmuLog.i(TAG, "onStop");
		
		super.onStop();
	}
	
	@Override
	protected void onRestart() {
		EmuLog.i(TAG, "onRestart");

		super.onRestart();
	}
	
	@Override
	protected void onStart() {
		EmuLog.i(TAG, "onStart");
		super.onStart();
	}
	
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		EmuLog.i(TAG, "onSaveInstanceState:" + outState);
		
		outState.putBoolean("hasSaved", true);
		outState.putString("curMrpPath", emulator.getRunningMrpPath());
		
		super.onSaveInstanceState(outState);
	}
	
	/**
	 * 被杀后恢复
	 */
	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState) {
		EmuLog.i(TAG, "onRestoreInstanceState:" + savedInstanceState);
		
		if(savedInstanceState.getBoolean("hasSaved", false)) {
			String curMrpPath = savedInstanceState.getString("curMrpPath");
			if(curMrpPath != null){
				EmuLog.i(TAG, "异常恢复成功");
//				Emulator.getInstance().setRunMrp(curMrpPath);
			} else {
				finish();
			}
		} else {
			finish();
		}

		super.onRestoreInstanceState(savedInstanceState);
	}
	
	public void postUIRunable(Runnable r) {
		handler.post(r);
	}
	
	public Handler getHandler() {
		return handler;
	}
	
	private void backgroundRun() {
		Intent intent = new Intent();
		intent.setClassName(this, mEntryActivity);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK
				| Intent.FLAG_ACTIVITY_SINGLE_TOP
				| Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
		intent.addCategory(Intent.CATEGORY_LAUNCHER);
		intent.setAction(Intent.ACTION_MAIN);
		
		startActivity(intent);
	}
	
	@Override
    public boolean onContextItemSelected(MenuItem item) {
		if (item.getItemId() == R.id.mi_close) {
            emulator.stop();
            finish();
		} else if (item.getItemId() == R.id.mi_scnshot) {
            emulator.getScreen().screenShot(this);
		} else if (item.getItemId() == R.id.mi_switch_keypad) {
            keypad.switchMode();
		} else if (item.getItemId() == R.id.mi_scale_mode) {
			showScaleDialog();
		} else if (item.getItemId() == R.id.mi_tools) {
			showToolListDialog();
		}
		else if (item.getItemId() == R.id.mi_switch_anti) {
			emulator.getScreen().switchAnt();
		}
		else if (item.getItemId() == R.id.mi_exit_all) {
			Process.killProcess(Process.myPid());
		}
		
		return true;
	}

    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo menuInfo) {
        if(v == emulatorView) {
            getMenuInflater().inflate(R.menu.emu_main, menu);
        }
        super.onCreateContextMenu(menu, v, menuInfo);
    }

    void showScaleDialog() {
		final String[] items = getResources().getStringArray(R.array.scaling_mode_entryvalues);
		int choice = 0;

		for(String s : items){
//			if(s.equals(EmuScreen.getScaleModeTag()))
//				break;
			choice++;
		}
		if(choice > items.length - 1)
			choice = items.length - 1;

		new AlertDialog.Builder(this)
            .setTitle(R.string.scaling_mode)
            .setSingleChoiceItems(R.array.scaling_mode_entries, choice, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    /* User clicked on a radio button do some stuff */
                	tmpChoice = which;
                }
            })
            .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
//                	EmuScreen.parseScaleMode(items[tmpChoice]);
                	emulator.getScreen().setScale(whichButton);
                	emulator.getScreen().flush();
                }
            })
            .setNegativeButton(R.string.cancel, null)
           .create()
           .show();
	}
	
	void showToolListDialog() {
		new AlertDialog.Builder(this)
				.setTitle(R.string.tools)
				.setItems(TOOLS, new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, int which) {
						runTool(which);
					}
				})
				.create()
				.show();
	}

	private Dialog mrpInputDialog;
	private EditText mrpInputEdit;

	/**
	 * 显示MRP输入框
	 *
	 * @param title
	 * @param content
	 * @param type
	 * @param max
	 */
	public void showMrpInputer(String title, String content, int type, int max) {
		if(mrpInputDialog == null) {
			AlertDialog.Builder builder = new AlertDialog.Builder(this);

			/**
			 * view 不能重用
			 * 2013-3-24 23:26:03
			 */
			View editDlgView = inflater.inflate(R.layout.dialog_input, null);
			mrpInputEdit = (EditText) editDlgView.findViewById(R.id.editText1);
			mrpInputDialog = builder.setTitle(title)
					.setView(editDlgView)
					.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							emulator.setEditInputContent(mrpInputEdit.getText().toString());
							emulator.postMrpEvent(MrDefines.MR_DIALOG_EVENT, MrDefines.MR_DIALOG_KEY_OK, 0);
						}
					})
					.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							emulator.setEditInputContent(null);
							emulator.postMrpEvent(MrDefines.MR_DIALOG_EVENT, MrDefines.MR_DIALOG_KEY_CANCEL, 0);
						}
					})
					.setCancelable(false)
					.create();
		}

		mrpInputDialog.setTitle(title);
		mrpInputEdit.setText(content);
		//字数限制
//		if(ar.limitInputLength)
			mrpInputEdit.setFilters(new  InputFilter[]{ new  InputFilter.LengthFilter(max)});

		int newType;

		if (type == MrDefines.MR_EDIT_ALPHA)
			newType = EditorInfo.TYPE_CLASS_TEXT | EditorInfo.TYPE_TEXT_FLAG_CAP_CHARACTERS | EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE;
		else if (type == MrDefines.MR_EDIT_NUMERIC)
			newType = EditorInfo.TYPE_CLASS_NUMBER | EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE;
		else if (type == MrDefines.MR_EDIT_PASSWORD)
			newType = EditorInfo.TYPE_CLASS_TEXT | EditorInfo.TYPE_TEXT_VARIATION_PASSWORD;
		else
			newType = EditorInfo.TYPE_CLASS_TEXT | EditorInfo.TYPE_TEXT_FLAG_MULTI_LINE;

		mrpInputEdit.setInputType(newType);

		mrpInputDialog.show();
	}
	

	private int tmpChoice;
	

	void showFragmentDialog(DialogFragment fragment) {
        fragment.show(getSupportFragmentManager(), "dialog");
    }
	
	private static final String[] TOOLS = new String[]{
		"按键精灵"
	};
	
	private KeySprite mKeySprite;
	
	private void stopKeySprite() {
		if(mKeySprite != null){
			mKeySprite.stop();
			mKeySprite = null;
		}
		handler.sendEmptyMessageDelayed(MSG_ID_UPDATE_INFO_TEXT, 100);
		
		EmuLog.i(TAG, "stopKeySprite");
	}
	
	private void runKeySprite(KeySprite sprite) {
		mKeySprite = sprite;
		tvInfo.setTag(INFO_TYPE_KEY_SPRITE);
		
		sprite.run(new KeyEventListener() {
			
			@Override
			public void onKeyUp(int key, Sprite sprite) {
				if(mKeySprite == null)
					return;
				handler.obtainMessage(MSG_ID_KEY_UP, key, 0).sendToTarget();
				handler.obtainMessage(MSG_ID_UPDATE_INFO_TEXT, sprite.toString() + " 点此停止").sendToTarget();
			}
			
			@Override
			public void onKeyDown(int key, Sprite sprite) {
				if(mKeySprite == null)
					return;
				handler.obtainMessage(MSG_ID_KEY_DOWN, key, 0).sendToTarget();
				handler.obtainMessage(MSG_ID_UPDATE_INFO_TEXT, sprite.toString() + " 点此停止").sendToTarget();
			}
		});
	}
	
	private void showKeySpriteDialog() {
		showFragmentDialog(new ChooserFragment().setOnChooseLitener(new OnChooseLitener() {
			
			@Override
			public void onChoose(Object object) {
				runKeySprite((KeySprite) object);
			}
			
			@Override
			public void onCancel() {
			}
		}));
	}
	
	private void runTool(int index) {
		switch (index) {
		case 0: {
			showKeySpriteDialog();
			break;
		}
		}
	}
	
	public void startAppService(String action) {
		startService(new Intent(action).setClassName(this, APP_SERVICE_NAME));
	}
	
	private void entryBackground() {
		startAppService(EmulatorService.ACTION_FOREGROUND);
	}
	
	private void backFroground() {
		startAppService(EmulatorService.ACTION_BACKGROUND);
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if(requestCode == REQ_SHOWEDIT) {
			if(resultCode == 1 && data != null){
				emulator.setEditInputContent(data.getStringExtra("input"));
				emulator.postMrpEvent(MrDefines.MR_DIALOG_EVENT, MrDefines.MR_DIALOG_KEY_OK, 0);
			}else {
				emulator.setEditInputContent(null);
				emulator.postMrpEvent(MrDefines.MR_DIALOG_EVENT, MrDefines.MR_DIALOG_KEY_CANCEL, 0);
			}
		} else if (requestCode == REQ_GET_IMAGE) { //选择图片
			if (resultCode == RESULT_OK && null != data) {
//				Uri selectedImage = data.getData();
//				String[] filePathColumn = { MediaStore.Images.Media.DATA };
//
//				Cursor cursor = getContentResolver().query(selectedImage, filePathColumn, null, null, null);
//				cursor.moveToFirst();
//
//				int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
//				String picturePath = cursor.getString(columnIndex);
//				cursor.close();
//				
//				bitmapBg = BitmapFactory.decodeFile(picturePath);
			}
		}
		
		super.onActivityResult(requestCode, resultCode, data);
	}
	
	/**
	 * 发送短信提示
	 * 
	 * @param text
	 * @param addr
	 */
	public void reqSendSms(final String text, final String addr) {
		AlertDialog dialog = new AlertDialog.Builder(this)
			.setTitle(R.string.hint)
			.setMessage(emulator.getCurMrpAppName()
					+ "请求发送短信：\n"
					+ "地址：" + addr + "\n"
					+ "内容：" + text + "\n")
			.setPositiveButton(R.string.accept, new DialogInterface.OnClickListener() {
				
				@Override
				public void onClick(DialogInterface dialog, int which) {
					EmuSmsManager.getDefault().sendSms(text, addr);
				}
			})
			.setNegativeButton(R.string.refused, new DialogInterface.OnClickListener() {
				
				@Override
				public void onClick(DialogInterface dialog, int which) {
					//直接通知底层失败
	                emulator.postMrpEvent(MrDefines.MR_SMS_RESULT, MrDefines.MR_FAILED, 0);
				}
			})
			.setOnCancelListener(new DialogInterface.OnCancelListener() {
				
				@Override
				public void onCancel(DialogInterface dialog) {
					emulator.postMrpEvent(MrDefines.MR_SMS_RESULT,  MrDefines.MR_SUCCESS, 0);
				}
			})
			.create();
		dialog.setCancelable(false);
		dialog.setCanceledOnTouchOutside(!SMS_DEL_MODE);
		dialog.show();
	}
	
	/**
	 * 打电话提示
	 * 
	 * @param number
	 */
	public void reqCallPhone(final String number) {
		if(PackageManager.PERMISSION_DENIED == checkPermission(permission.CALL_PHONE, Process.myPid(), Process.myUid()))
			return;
		
		new AlertDialog.Builder(this)
			.setTitle(R.string.hint)
			.setMessage(emulator.getCurMrpAppName()
					+ "请求拨打：\n" 
					+ number)
			.setPositiveButton(R.string.accept, new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					Intent intent = new Intent(Intent.ACTION_CALL, Uri.parse("tel:" + number));
//					EmulatorActivity.this.startActivity(intent);
				}
			})
			.setNegativeButton(R.string.refused, null)
			.create()
			.show();
	}
	
	//---------- 对话框区 ----------------------------------------
	private AlertDialog dsmDialog = null;
	private AlertDialog.Builder dsmDialogBuilder = null;
	private static final DialogInterface.OnClickListener dsmDialogClickListener = new DialogInterface.OnClickListener() {

		@Override
		public void onClick(DialogInterface dialog, int which) {
			Emulator.getInstance().postMrpEvent(MR_DIALOG_EVENT, which==DialogInterface.BUTTON_NEGATIVE? MR_DIALOG_KEY_OK : MR_DIALOG_KEY_CANCEL, 0);
		}
	};
	
	public void dsmDialogShow(String title, String content, int type) {
		dsmDialogBuilder = new AlertDialog.Builder(getActivity());
		dsmDialogBuilder.setTitle(title).setMessage(content);
		if(type == MR_DIALOG_OK || type == MR_DIALOG_OK_CANCEL) 
			dsmDialogBuilder.setPositiveButton(R.string.ok, dsmDialogClickListener);
		if(type == MR_DIALOG_CANCEL || type == MR_DIALOG_OK_CANCEL) 
			dsmDialogBuilder.setNegativeButton(R.string.cancel, dsmDialogClickListener);
		
		dsmDialog = dsmDialogBuilder.create();
		dsmDialog.show();
	}
	
	public void dsmDialogReShow(String title, String content, int type) {
		if(dsmDialog == null || !dsmDialog.isShowing())
			dsmDialogShow(title, content, type);
		
		dsmDialog.setTitle(title);
		dsmDialog.setMessage(content);
		if(type == MR_DIALOG_OK || type == MR_DIALOG_OK_CANCEL) 
			dsmDialog.setButton(DialogInterface.BUTTON_POSITIVE, getResources().getString(R.string.ok), dsmDialogClickListener);
		else
			dsmDialog.setButton(DialogInterface.BUTTON_POSITIVE, null, (DialogInterface.OnClickListener)null);

		if(type == MR_DIALOG_CANCEL || type == MR_DIALOG_OK_CANCEL) 
			dsmDialog.setButton(DialogInterface.BUTTON_NEGATIVE, getResources().getString(R.string.cancel), dsmDialogClickListener);
		else
			dsmDialog.setButton(DialogInterface.BUTTON_NEGATIVE, null, (DialogInterface.OnClickListener)null);
	}
	
	public void dsmDialogReClose() {
		if(dsmDialog == null)
			dsmDialog.dismiss();
		dsmDialog = null;
		dsmDialogBuilder = null;
	}
	
	//---------- 菜单区 ----------------------------------------
	private AlertDialog dsmMenuDialog = null;
	private AlertDialog.Builder dsmMenuBuilder = null;
	private List<String> dsmMenuItems = null;
	private int dsmMenuItemCount = 0;
	private static final DialogInterface.OnClickListener dsmMenuItemClickListener = new DialogInterface.OnClickListener() {

		@Override
		public void onClick(DialogInterface dialog, int which) {
			Emulator.getInstance().postMrpEvent(MR_MENU_SELECT, which, 0);
		}
	};
	
	public void dsmMenuCreate(String title, int itemCount) {
		dsmMenuItemCount = itemCount;
		if(dsmMenuItemCount <= 0)
			return;
		
		dsmMenuBuilder = new AlertDialog.Builder(getActivity());
		dsmMenuBuilder.setTitle(title);
		//MTK 返回
		dsmMenuBuilder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				Emulator.getInstance().postMrpEvent(MR_MENU_RETURN, 0, 0);
			}
		});
		
		dsmMenuItems = new ArrayList<String>(itemCount);
		for(int i=0; i<itemCount; i++) {
			dsmMenuItems.add("null");
		}
	}
	
	public void dsmMenuSetItem(String title, int position) {
		if(dsmMenuItems == null || position < 0 || position >= dsmMenuItemCount)
			return;
		
		dsmMenuItems.set(position, title);
	}
	
	public void dsmMenuShow() {
		if(dsmMenuItems == null || dsmMenuItemCount <= 0)
			return;
		
		dsmMenuBuilder.setItems(dsmMenuItems.toArray(new CharSequence[]{}), dsmMenuItemClickListener);
		dsmMenuDialog = dsmMenuBuilder.create();
		dsmMenuDialog.show();
	}
	
	public void dsmMenuReShow() {
		if(dsmMenuDialog != null) {
			dsmMenuDialog.dismiss();
			dsmMenuDialog = null;
		}
		dsmMenuShow();
	}
	
	public void dsmMenuClose() {
		if(dsmMenuDialog != null) {
			dsmMenuDialog.dismiss();
			dsmMenuDialog = null;
		}
		if(dsmMenuItems != null) {
			dsmMenuItems.clear();
			dsmMenuItems = null;
		}
		dsmMenuItemCount = 0;
		dsmMenuBuilder = null;
	}
	
	private final OnKeyEventListener mKeyEventListener = new OnKeyEventListener() {
		
		@Override
		public boolean onKeyUp(int key) {
			if(key == 1025) { //打开菜单
				openContextMenu(emulatorView);
			} else {
				emulator.postMrpEvent(MrDefines.MR_KEY_RELEASE, key, 0);
			}
			
			return true;
		}
		
		@Override
		public boolean onKeyDown(int key) {
			if(cfg.enableKeyVirb)
				vibrator.vibrate(20);
			
			emulator.postMrpEvent(MrDefines.MR_KEY_PRESS, key, 0);
			
			return false;
		}
	};
}
