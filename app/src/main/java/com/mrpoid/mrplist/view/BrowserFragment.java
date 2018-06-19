package com.mrpoid.mrplist.view;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnCreateContextMenuListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.webkit.DownloadListener;
import android.webkit.JsResult;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebView.HitTestResult;
import android.webkit.WebViewClient;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.edroid.common.dl.DownloadManager;
import com.edroid.common.dl.Downloader;
import com.edroid.common.utils.ApkUtils;
import com.edroid.common.utils.ApkUtils.ApkResources;
import com.edroid.common.utils.Md5Utils;
import com.edroid.common.utils.TimeUtils;
import com.edroid.common.utils.UIUtils;
import com.mrpoid.MrpoidMain;
import com.mrpoid.mrpliset.R;
import com.mrpoid.mrplist.moduls.MyFavoriteManager;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

/**
 * 在线列表
 * 
 * @author Yichou 2013-12-19
 * 
 */
@SuppressLint("SetJavaScriptEnabled")
public class BrowserFragment extends Fragment implements DownloadListener, OnClickListener {
	static final int ID_MARK = 1001;
	static final int ID_COPY = 1002;
	static final int ID_SHARE = 1003;
	static final int ID_DOWNLOAD = 1004;
	
	WebView mWebView;
	ProgressBar mBar;
	PopupWindow markWindow;
	ListView markListView;
	final List<String> markList = new ArrayList<String>();
	static final String[] MARK_MENU = new String[]{"+收藏该页", "+新建书签"};
	
	static final String START_PAGE_URL = 
			"http://edroid.cn/app/";
	static final String HOME_PAGE_URL = 
			"http://edroid.cn/app/";
	
	 private ValueCallback<Uri> mUploadMessage;
	 private final static int FILECHOOSER_RESULTCODE = 1;
	
	
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		initMarkMenu();
		
		checkCache();
	}
	
	/**
	 * 先写下思路
	 * 
	 * 先指向E界某个固定页面，里面添加各大mrp站
	 * 
	 * 点击每个链接，进入他们的mrp下载专区
	 * 
	 * 监测mrp文件下载，下完提示运行
	 * 
	 * 同时运行 多个mrp 逻辑
	 */
	
	public void loadUrl(String url) {
		mWebView.loadUrl(url);
	}
	
	private void initMarkMenu() {
		LayoutInflater inflater = LayoutInflater.from(getActivity());
		View markView = inflater.inflate(R.layout.popu_mark, null);
		
		float base = getResources().getDimension(R.dimen.dp1);
		float h = getResources().getDimensionPixelSize(R.dimen.toobar_height);
		
		DisplayMetrics dm = getResources().getDisplayMetrics();
		
		markWindow  = new PopupWindow(markView, Math.round(dm.widthPixels - h - base*10), LayoutParams.WRAP_CONTENT);
		
		markWindow.setBackgroundDrawable(getResources().getDrawable(R.drawable.popu_mark_bg));
		markWindow.setOutsideTouchable(true);
		markWindow.setAnimationStyle(android.R.style.Animation_Dialog);
		markWindow.update();
		markWindow.setTouchable(true);
		markWindow.setFocusable(true);
		
		markListView = (ListView) markView.findViewById(R.id.listView1);
		markListView.setAdapter(new ArrayAdapter<String>(getActivity(), 
				android.R.layout.simple_list_item_1, 
				markList){
			@Override
			public View getView(int position, View convertView, ViewGroup parent) {
				View view = super.getView(position, convertView, parent);
				((TextView)view).setTextColor(0xfff0f0f0);
				return view;
			}
		});
		
		markListView.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
				SharedPreferences sp = getActivity().getSharedPreferences("bookmarks", 0);
				
				if(position == 0) {
					editMarket(mWebView.getTitle(), mWebView.getUrl());
				} else if (position == 1) {
					editMarket(null, null);
				} else {
					mWebView.loadUrl(sp.getString(markList.get(position), null));
				}
				
				markWindow.dismiss();
			}
		});
	}
	
	private void showMarkMenu() {
		SharedPreferences sp = getActivity().getSharedPreferences("bookmarks", 0);
		
		Map<String, ?> map = sp.getAll();
		
		markList.clear();
		for(String s : MARK_MENU) {
			markList.add(s);
		}
		for(Entry<String, ?> entry : map.entrySet()) {
			markList.add(entry.getKey());
		}
		
		((BaseAdapter)markListView.getAdapter()).notifyDataSetChanged();
		
		float base = getResources().getDimension(R.dimen.dp1);
		
		markWindow.showAtLocation(mWebView, Gravity.RIGHT|Gravity.BOTTOM, 
				Math.round(5 * base), 
				Math.round(5 * base) + 2*getResources().getDimensionPixelSize(R.dimen.toobar_height));
	}
	
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View root = inflater.inflate(R.layout.fragment_browser, null);
		onViewCreate(root);
		return root;
	}
	
	/**
	 * webview 链接长按操作
	 */
	final MenuItem.OnMenuItemClickListener webMenuItemClickListener = new MenuItem.OnMenuItemClickListener() {

		@Override
		public boolean onMenuItemClick(MenuItem item) {
			switch (item.getItemId()) {
			case ID_SHARE:
				Log.i("you click", "分享");
				break;
			case ID_DOWNLOAD:
				Log.i("you click", "下载");
				break;
			case ID_COPY:
				Log.i("you click", "复制");
				break;
				
			case ID_MARK:
				editMarket(null, item.getIntent().getDataString());
				break;
			}
			
			return true;
		}
	};
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == FILECHOOSER_RESULTCODE) {
			if (null == mUploadMessage)
				return;
			
			Uri result = (data == null || resultCode != Activity.RESULT_OK)? null : data.getData();
			mUploadMessage.onReceiveValue(result);
			mUploadMessage = null;
		}
	}
	
	private void initWebView() {
		mWebView.setWebChromeClient(new WebChromeClient() {

			@Override
			public void onProgressChanged(WebView view, int newProgress) {
				mBar.setProgress(newProgress);
				if(newProgress >= 100) {
					mBar.setProgress(0);
				}
			}

			@Override
			public void onReceivedTitle(WebView view, String title) {
				super.onReceivedTitle(view, title);
				
				if(getActivity() != null) //有空指针异常
					((AppCompatActivity)getActivity()).getSupportActionBar().setTitle(title);
			}

			@Override
			public boolean onJsAlert(WebView view, String url, String message, JsResult result) {
				return super.onJsAlert(view, url, message, result);
			}

			@Override
			public boolean onJsConfirm(WebView view, String url, String message, JsResult result) {
				return super.onJsConfirm(view, url, message, result);
			}
			
			// For Android > 4.1.1
			public void openFileChooser(ValueCallback<Uri> uploadMsg, String acceptType, String capture) {
				openFileChooser(uploadMsg, acceptType);
			}
	          
			// For Android 3.0+
            public void openFileChooser(ValueCallback<Uri> uploadMsg, String acceptType) {
                mUploadMessage = uploadMsg;
                Intent i = new Intent(Intent.ACTION_GET_CONTENT);
                i.addCategory(Intent.CATEGORY_OPENABLE);
                i.setType("image/*");
                
                startActivityForResult(Intent.createChooser(i, "File Chooser"),
                		FILECHOOSER_RESULTCODE);
            }

            // The undocumented magic method override
            // Eclipse will swear at you if you try to put @Override here
            public void openFileChooser(ValueCallback<Uri> uploadMsg) {
                mUploadMessage = uploadMsg;
                Intent i = new Intent(Intent.ACTION_GET_CONTENT);
                i.addCategory(Intent.CATEGORY_OPENABLE);
                i.setType("image/*");
                
                startActivityForResult(Intent.createChooser(i, "File Chooser"), 
                		FILECHOOSER_RESULTCODE);
            }
		});
		
		mWebView.setWebViewClient(new WebViewClient() {
			public boolean shouldOverrideUrlLoading(WebView view, String url) {
//				mLoadingView.setVisibility(View.VISIBLE);
				view.loadUrl(url);
				return true;
			}
		});
		
		//上下文菜单
		mWebView.setOnCreateContextMenuListener(new OnCreateContextMenuListener() {
			
			@Override
			public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
				Log.i("long click","true");
				
				HitTestResult result = ((WebView) v).getHitTestResult();
				int resultType = result.getType();
				
				if ((resultType == HitTestResult.ANCHOR_TYPE)
						|| (resultType == HitTestResult.IMAGE_ANCHOR_TYPE)
						|| (resultType == HitTestResult.SRC_ANCHOR_TYPE)
						|| (resultType == HitTestResult.SRC_IMAGE_ANCHOR_TYPE)) {
					Intent i = new Intent(Intent.ACTION_VIEW);
					i.setData(Uri.parse(result.getExtra()));
					
					Log.i("extrea", result.getExtra());

					//如果 不设置监听器，会发生到附属 Activity
					menu.add(0, ID_SHARE, 0, "分享")
						.setIntent(i)
						.setOnMenuItemClickListener(webMenuItemClickListener);
					
					menu.add(0, ID_COPY, 0, "复制")
						.setIntent(i)
						.setOnMenuItemClickListener(webMenuItemClickListener);

					menu.add(0, ID_DOWNLOAD, 0, "下载")
						.setIntent(i)
						.setOnMenuItemClickListener(webMenuItemClickListener);

					menu.add(0, ID_MARK, 0, "增加到书签")
						.setIntent(i)
						.setOnMenuItemClickListener(webMenuItemClickListener);
				} else if (resultType == HitTestResult.IMAGE_TYPE) { //图片处理
					Log.i("image type", "ture");
					Intent i = new Intent();
					MenuItem item = menu.add(0, 1, 0, "OPEN");
					item.setIntent(i);
					item = menu.add(0, 2, 0, "图片");
					item.setIntent(i);
					menu.setHeaderTitle(result.getExtra());
				}
			}
		});
		
		mWebView.setDownloadListener(this);
//		mWebView.setScrollBarStyle(0);
//		mWebView.getSettings().setSupportZoom(true);
//		mWebView.getSettings().setUseWideViewPort(true);
//		mWebView.getSettings().setUserAgentString();
	}
	
	private void checkCache() {
		SharedPreferences sp = getActivity().getSharedPreferences("popu_item_page", 0);
		int curDay = TimeUtils.getDayOfYear();
		int oldDay = sp.getInt("day", curDay-1);
		
		if(oldDay != curDay) {
			getActivity().deleteDatabase("webview.db");  
			getActivity().deleteDatabase("webviewCache.db");
			
			sp.edit().putInt("day", curDay).commit();
		}
	}
	
	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);
	}
	
	protected void onViewCreate(View root) {
		mWebView = (WebView) root.findViewById(R.id.webView1);
		mBar = (ProgressBar) root.findViewById(R.id.progressBar1);
		
		initWebView();
		
		mWebView.getSettings().setJavaScriptEnabled(true);
//		mWebView.getSettings().setCacheMode(WebSettings.LOAD_CACHE_ELSE_NETWORK);
		mWebView.getSettings().setCacheMode(WebSettings.LOAD_DEFAULT);

		mWebView.loadUrl(START_PAGE_URL);
		
		root.findViewById(R.id.iv_btn1).setOnClickListener(this);
		root.findViewById(R.id.iv_btn2).setOnClickListener(this);
		root.findViewById(R.id.iv_btn3).setOnClickListener(this);
		root.findViewById(R.id.iv_btn4).setOnClickListener(this);
		root.findViewById(R.id.iv_btnHome).setOnClickListener(this);
	}
	
	@SuppressLint("DefaultLocale")
	static boolean matchEnd(String src, String tag) {
		int i = src.lastIndexOf('.');
		if(i != -1) {
			String tmp = src.substring(i+1).toLowerCase();
			return tmp.endsWith(tag);
		}
		return false;
	}
	
	private static void handleFile(FragmentManager fm, String path) {
		FileDialogFragment fragment = new FileDialogFragment();
		Bundle bundle = new Bundle();
		bundle.putString("path", path);
		fragment.setArguments(bundle);
		fragment.show(fm, "fileDialog");
	}
	
	private void startDownload(String type, String url) {
		final String root = new File(Environment.getExternalStorageDirectory(), 
				"Download/" + type).getAbsolutePath();
		
		File path = new File(root, Md5Utils.md516(url.getBytes()) + "." + type);
		if(!path.exists()) {
			DownloaderDialogFragment fragment = new DownloaderDialogFragment();
			fragment.show(getFragmentManager(), "downloadDialog");
			fragment.start(getActivity(), url, path.getAbsolutePath());
		} else {
//			UIUtils.toastMessage(getActivity(), "已经下载！");
			handleFile(getFragmentManager(), path.getAbsolutePath());
		}
	}
	
	/**
	 * 编辑书签
	 * 
	 * @param title
	 * @param url
	 */
	public void editMarket(String title, String url) {
		MarkEditDialogFragment fragment = new MarkEditDialogFragment();
		
		Bundle bundle = new Bundle();
		bundle.putString("title", title);
		bundle.putString("url", url);
		
		fragment.setArguments(bundle);
		fragment.show(getFragmentManager(), "markDialog");
	}
	
	/**
	 * 文件处理对话框
	 * 
	 * @author YYichou
	 *
	 */
	public static class FileDialogFragment extends DialogFragment implements OnClickListener {
		View contentView;
		EditText editText;
		TextView textView1;
		ApkResources apkResources;
		String path;
		int fileType; //1apk 2mrp
		
		
		private void handle() {
			textView1.setText(path);
			
			int i = path.lastIndexOf('/');
			String tmp = path.substring(i + 1);
			editText.setText(tmp);
			
			if(matchEnd(path, "apk")) {
				apkResources = ApkUtils.makeApkResources(getActivity(), path);
				fileType = 1;
			} else if (matchEnd(path, "mrp")) {
				fileType = 2;
			}
		}
		
		@Override
		public void onClick(View v) {
			if(v.getId() == R.id.button1) {
				if(editText.getText().length() > 0) {
					String newName = editText.getText().toString();
					
					File oldFile = new File(path);
					File newFile = new File(oldFile.getParentFile(), newName);
					if(oldFile.renameTo(newFile)) {
						path = newFile.getAbsolutePath();
						textView1.setText(path);
						
						UIUtils.toastMessage(getActivity(), "success!");
					}
				}
			} else if (v.getId() == R.id.button2) {
				switch (fileType) {
				case 1: {
					startActivity(ApkUtils.getInstallIntent((getActivity().getApplicationContext()), new File(path)));
					break;
				}
				
				case 2: {
					MrpoidMain.runMrp(getActivity(), path);
					MyFavoriteManager.getInstance().add(path);
					break;
				}
				}
			}
		}
		
		@Override
		public void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			
			final LayoutInflater inflater = LayoutInflater.from(getActivity());
			contentView = inflater.inflate(R.layout.dialog_filehandle, null);
			
			contentView.findViewById(R.id.button1).setOnClickListener(this);
			contentView.findViewById(R.id.button2).setOnClickListener(this);
			editText = (EditText) contentView.findViewById(R.id.editText1);
			textView1 = (TextView) contentView.findViewById(R.id.textView2);
			
			path = getArguments().getString("path");
			handle();
		}
		
		@Override
		public Dialog onCreateDialog(Bundle savedInstanceState) {
			AlertDialog.Builder builder = new AlertDialog.Builder(getActivity())
				.setTitle(R.string.filehandle)
				.setView(contentView)
				.setCancelable(false)
				.setNegativeButton(R.string.cancel, null);
			
			if(apkResources != null) {
//				builder.setIcon(apkResources.getIcon());
			}
			if(apkResources != null) {
//				builder.setTitle(apkResources.getLabel());
			}
			
			return builder.create();
		}
		
		@Override
		public void onDestroy() {
			super.onDestroy();
			
			if(apkResources != null) {
				apkResources.recyle();
			}
		}
	}
	
	/**
	 * 书签编辑
	 * 
	 * @author YYichou
	 *
	 */
	public static class MarkEditDialogFragment extends DialogFragment {
		View contentView;
		EditText editText1, editText2;
		
		
		private void saveMark() {
			SharedPreferences sp = getActivity().getSharedPreferences("bookmarks", 0);
			
			String title = editText1.getText().toString();
			String url = editText2.getText().toString();
			sp.edit().putString(title, url).commit();
		}
		
		@Override
		public Dialog onCreateDialog(Bundle savedInstanceState) {
			LayoutInflater inflater = LayoutInflater.from(getActivity());
			contentView = inflater.inflate(R.layout.dialog_markedit, null);

			editText1 = (EditText) contentView.findViewById(R.id.editText1);
			editText2 = (EditText) contentView.findViewById(R.id.editText2);
			
			String title = getArguments().getString("title");
			String url = getArguments().getString("url");
			
			if(title == null && url != null)
				title = url.substring(0, 10);
			if(url == null)
				url = "http://";
			
			editText1.setText(title);
			editText2.setText(url);
			
			return new AlertDialog.Builder(getActivity())
				.setTitle("书签编辑")
				.setView(contentView)
				.setPositiveButton(R.string.save, new DialogInterface.OnClickListener() {
					
					@Override
					public void onClick(DialogInterface dialog, int which) {
						saveMark();
					}
				})
				.setNegativeButton(R.string.cancel, null)
				.create();
		}
	}
	
	public static class DownloaderDialogFragment extends DialogFragment {
		ProgressDialog mDialog;
		Downloader downloader;
		Handler mHandler;
		
		
		final DialogInterface.OnClickListener mClickListener = new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				if(which == ProgressDialog.BUTTON_NEGATIVE) {
					downloader.cancel();
//					mDialog.cancel();
				} else if (which == ProgressDialog.BUTTON_POSITIVE) {
					mDialog.dismiss();
				}
			}
		};
		
		private void toast(final String msg) {
			mHandler.post(new Runnable() {
				
				@Override
				public void run() {
					UIUtils.toastMessage(getActivity(), msg);
				}
			});
		}
		
		@Override
		public void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			
			mHandler = new Handler();
		}
		
		public void start(Activity activity, String url, final String savePath) {
			downloader = DownloadManager.startAsync(activity, savePath, url, mHandler, new com.edroid.common.dl.DownloadListener() {
				
				@Override
				public void onStart(long start, long total) {
					if(mDialog != null && mDialog.isShowing()) {
						mDialog.setMax((int) total);
						mDialog.setProgress((int) start);
					}
				}
				
				@Override
				public void onProgress(long cur, byte prog) {
					System.out.println("tid=" + Thread.currentThread().getId());

					if(mDialog != null && mDialog.isShowing()) {
						mDialog.setProgress((int) cur);
					}
				}
				
				@Override
				public void onCancel() {
					if(mDialog != null && mDialog.isShowing()) {
						mDialog.dismiss();
					}
				}
				
				@Override
				public void onFinish() {
					System.out.println("tid=" + Thread.currentThread().getId());
					
					toast("下载完成!");

					if(mDialog != null && mDialog.isShowing()) {
						mDialog.dismiss();
					}
					
					handleFile(getFragmentManager(), savePath);
				}
			});
		}
		
		@Override
		public Dialog onCreateDialog(Bundle savedInstanceState) {
			mDialog = new ProgressDialog(getActivity());
				
			mDialog.setTitle(R.string.downloading);
			mDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
			mDialog.setCancelable(false);
			mDialog.setButton(ProgressDialog.BUTTON_POSITIVE, getString(R.string.background), mClickListener);
			mDialog.setButton(ProgressDialog.BUTTON_NEGATIVE, getString(R.string.cancel), mClickListener);
			mDialog.setOnCancelListener(new DialogInterface.OnCancelListener() {
				
				@Override
				public void onCancel(DialogInterface dialog) {
					mDialog = null;
				}
			});
			
			return mDialog;
		}
	}

	@Override
	public void onDownloadStart(String url, 
			String userAgent, String contentDisposition, String mimetype,
			long contentLength) {

		System.out.println("------- dl --------");
		System.out.println(url);
		System.out.println(contentDisposition);
		System.out.println(mimetype);
		
		
		if("application/vnd.android.package-archive".equals(mimetype)) { //apk 
			startDownload("apk", url);
		} else if (matchEnd(url, "mrp")) {
			startDownload("mrp", url);
		} else {
			startActivity(new Intent(Intent.ACTION_VIEW)
				.setData(Uri.parse(url)));
		}
	}
	
	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.iv_btn1:
			mWebView.goBack();
			break;
		case R.id.iv_btn3:
			mWebView.reload();
			break;
		case R.id.iv_btn2:
			mWebView.goForward();
			break;
			
		case R.id.iv_btn4:
			showMarkMenu();
			break;

		case R.id.iv_btnHome:
			loadUrl(HOME_PAGE_URL);
			break;

		default:
			break;
		}
	}
}
