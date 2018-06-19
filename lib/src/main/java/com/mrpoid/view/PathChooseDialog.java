package com.mrpoid.view;

import java.io.File;
import java.util.List;
import java.util.Stack;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.mrpoid.R;
import com.edroid.common.utils.FileUtils;
import com.edroid.common.utils.UIUtils;
import com.mrpoid.view.ListViewPathAdapter.OnPathOperateListener;

/**
 * 路径选择弹窗
 * 
 * @author yeguozhong@yeah.net
 * 
 */
public class PathChooseDialog extends Dialog implements android.view.View.OnClickListener {
	private ListView lv;
	private TextView tvCurPath;
	private Context ctx;

	private List<String> data;
	private ListAdapter listAdapter;

	private ChooseCompleteListener listener;
	private Stack<String> pathStack = new Stack<String>();

	private int firstIndex = 0;
	private boolean isBack = false;

	private View lastSelectItem; // 上一个长按操作的View
	private String root, dir;
	
	
	public static void ChoosePath(Context activity, ChooseCompleteListener l, String root, String dir) {
		new PathChooseDialog(activity, l, root, dir).show();
	}
	
	// 监听操作事件
	private OnPathOperateListener pListener = new OnPathOperateListener() {
		@Override
		public void onPathOperate(int type, final int position, final TextView pathName) {
			if (type == OnPathOperateListener.DEL) {
				String path = data.get(position);
				int rs = FileUtils.deleteBlankPath(path);
				if (rs == 0) {
					data.remove(position);
					refleshListView(data, firstIndex);
					UIUtils.toastMessage(ctx, "删除成功");
				} else if (rs == 1) {
					UIUtils.toastMessage(ctx, "没有权限");
				} else if (rs == 2) {
					UIUtils.toastMessage(ctx, "不能删除非空目录");
				}

			} else if (type == OnPathOperateListener.RENAME) {
				final EditText et = new EditText(ctx);
				et.setText(FileUtils.getPathName(data.get(position)));
				AlertDialog.Builder builder = new AlertDialog.Builder(ctx);
				builder.setTitle("重命名");
				builder.setView(et);
				builder.setCancelable(true);
				builder.setPositiveButton("确定", new OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						String input = et.getText().toString();
						if (input != null || input.length() == 0) {
							UIUtils.toastMessage(ctx, "输入不能为空");
						} else {
							String newPath = pathStack.peek() + File.separator + input;
							boolean rs = FileUtils.reNamePath(data.get(position), newPath);
							if (rs == true) {
								pathName.setText(input);
								data.set(position, newPath);
								UIUtils.toastMessage(ctx, "重命名成功");
							} else {
								UIUtils.toastMessage(ctx, "重命名失败");
							}
						}
						dialog.dismiss();
					}
				});
				builder.setNegativeButton("取消", new OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {
						dialog.dismiss();
					}
				});
				builder.create().show();
			}
		}
	};

	public interface ChooseCompleteListener {
		/**
		 * 
		 * @param fullPath 完整路径
		 * @param relativePath 相对于根目录
		 */
		void onComplete(String fullPath, String relativePath);
	}

	private PathChooseDialog(Context context, ChooseCompleteListener listener, String root, String dir) {
		super(context);
		this.ctx = context;
		this.listener = listener;
		this.root = root;
		this.dir = dir;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.file_path_chooser);
		setCanceledOnTouchOutside(true);
		init();
	}

	private void init() {
		lv = (ListView) findViewById(android.R.id.list);
		tvCurPath = (TextView) findViewById(R.id.tv_cur_path);

		findViewById(R.id.btn_comfirm).setOnClickListener(this);
		findViewById(R.id.btn_back).setOnClickListener(this);
		findViewById(R.id.btn_new).setOnClickListener(this);

		if(root.charAt(root.length()-1) != File.separatorChar){
			root.concat(File.separator);
		}
		
		String rootPath = root;//Environment.getExternalStorageDirectory().getAbsolutePath();
		pathStack.add(rootPath);
		if(dir!=null && dir.length()>0) {
			if(dir.indexOf(0) == File.separatorChar)
				dir.substring(1);
			
			pathStack.add(rootPath);
			rootPath.concat(dir);
		}

		data = FileUtils.listPath(rootPath);
		tvCurPath.setText(rootPath);

		refleshListView(data, 0);
		// 单击
		lv.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
				firstIndex = position;
				String currentPath = data.get(position);
				tvCurPath.setText(currentPath);
				data = FileUtils.listPath(currentPath);
				pathStack.add(currentPath);
				refleshListView(data, pathStack.size() - 1);
			}
		});
		// 长按
		lv.setOnItemLongClickListener(new OnItemLongClickListener() {
			@Override
			public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
				if (lastSelectItem != null && !lastSelectItem.equals(view)) {
					lastSelectItem.findViewById(R.id.ll_op).setVisibility(View.GONE);
				}
				LinearLayout llOp = (LinearLayout) view.findViewById(R.id.ll_op);
				int visible = llOp.getVisibility() == View.GONE ? View.VISIBLE : View.GONE;
				llOp.setVisibility(visible);
				lastSelectItem = view;
				return true;
			}
		});
	}

	/**
	 * 更新listView视图
	 * 
	 * @param data
	 */
	private void refleshListView(List<String> data, int firstItem) {
		String lost = FileUtils.getSDRoot() + "lost+found";
		data.remove(lost);
		listAdapter = new ListViewPathAdapter(ctx, data, R.layout.file_path_listitem, pListener);
		lv.setAdapter(listAdapter);
		lv.setSelection(firstItem);
	}

	@Override
	public void onClick(View v) {
		if (v.getId() == R.id.btn_back) {
			if (pathStack.size() > 1) {
				isBack = true;
				pathStack.pop();
				data = FileUtils.listPath(pathStack.peek());
				tvCurPath.setText(pathStack.peek());
				refleshListView(data, firstIndex);
			}
		} else if (v.getId() == R.id.btn_comfirm) {
			String full = pathStack.peek();
			String relative = full.substring(root.length());
			listener.onComplete(full, relative);
			dismiss();
		} else if (v.getId() == R.id.btn_new) {
			final EditText et = new EditText(ctx);
			et.setText("新建文件夹");
			AlertDialog.Builder builder = new AlertDialog.Builder(ctx);
			builder.setTitle("新建文件夹");
			builder.setView(et);
			builder.setCancelable(true);
			builder.setPositiveButton("确定", new OnClickListener() {

				@Override
				public void onClick(DialogInterface dialog, int which) {
					String rs = et.getText().toString();
					if (rs == null || rs.length() == 0) {
						UIUtils.toastMessage(ctx, "输入不能为空");
					} else {
						String newPath = pathStack.peek() + File.separator + rs;
						int ret = FileUtils.createDir(newPath);
						switch (ret) {
						case FileUtils.SUCCESS:
							data.add(newPath);
							refleshListView(data, data.size() - 1);
							UIUtils.toastMessage(ctx, "创建成功");
							break;
						case FileUtils.FAILED:
							UIUtils.toastMessage(ctx, "创建失败");
							break;
						}
					}
					dialog.dismiss();
				}
			});

			builder.setNegativeButton("取消", new OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					dialog.dismiss();
				}
			});
			builder.create().show();
		}
	}
}
