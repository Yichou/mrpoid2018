package com.mrpoid.app;

import java.io.File;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import com.mrpoid.MrpoidMain;
import com.mrpoid.core.Emulator;
import com.edroid.common.utils.FileUtils;

/**
 * 外部通过此Activity 启动模拟器运行mrp
 * 
 * @author Yichou
 * 
 */
public class ExternActivity extends BaseActivity {
	private static final String TAG = "MrpoidExtern";

	private void showError(String msg) {
		Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
		finish();
	}

	@Override
	protected void onCreate(Bundle arg0) {
		super.onCreate(arg0);

		Intent intent = getIntent();
		// {act=android.intent.action.VIEW
		// dat=file:///storage/emulated/0/mythroad/dsm_gm.mrp
		// cmp=com.mrpej.mrpoid/.EmulatorActivity}
		Log.d(TAG, intent.toString());

		Uri dat = intent.getData();
		if (dat == null) {
			showError("参数非法！");
			return;
		}

		String inputPath = dat.getPath();
		File inputFile = new File(inputPath);

		if (!inputFile.exists()) {
			showError("无法读取该文件！");
			return;
		}
		
		Emulator emulator = Emulator.getInstance();
		String str = inputPath;
		int i = str.indexOf(emulator.getVmWorkPath()); //是不是在当前模拟器工作目录下
		if (i == -1) { // 需要复制
			File tempFile = emulator.getVmFullFilePath(inputFile.getName());
			if (FileUtils.FAILED == FileUtils.copyTo(tempFile, inputFile)) {
				showError("非 mythroad 下，复制文件失败！");
				return;
			}
			
			str = inputPath = tempFile.getAbsolutePath();
			inputFile = tempFile;
			i = str.indexOf(emulator.getVmWorkPath()); //是不是在当前模拟器工作目录下
		}

		str = str.substring(i);
		i = str.indexOf(File.separator);
		str = str.substring(i + 1);

		MrpoidMain.runMrp(this, str);
	}

	@Override
	protected void onRestart() {
		super.onRestart();

		// 运行mrp关闭后，确定会调用此方法么？
		finish();
	}
}
