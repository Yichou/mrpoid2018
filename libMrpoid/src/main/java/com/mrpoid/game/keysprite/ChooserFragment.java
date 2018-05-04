package com.mrpoid.game.keysprite;

import java.io.File;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.app.DialogFragment;

import com.mrpoid.R;
import com.mrpoid.core.Emulator;

/**
 * 按键精灵选择界面
 * 
 * @author Yichou 2013-8-31
 *
 */
public class ChooserFragment extends DialogFragment {
	private OnChooseLitener mLitener;
	
	
	private void loadAndRun(File file) {
		KeySprite keySprite = new SampleKeySprite();
		try {
			keySprite.fromXml(file);
			if(mLitener != null)
				mLitener.onChoose(keySprite);
		} catch (Exception e) {
		}
	}
	
	public ChooserFragment setOnChooseLitener(OnChooseLitener l) {
		this.mLitener = l;
		return this;
	}
	
	@Override
	public Dialog onCreateDialog(Bundle savedInstanceState) {
		final File dir = Emulator.getPublicFilePath("keySprites");
		final String[] files = dir.list();

		return new AlertDialog.Builder(getActivity())
			.setTitle(R.string.choose_one)
			.setItems(files, new DialogInterface.OnClickListener() {
	
				@Override
				public void onClick(DialogInterface dialog, int which) {
					loadAndRun(new File(dir, files[which]));
				}
			})
			.setPositiveButton(R.string.create, new DialogInterface.OnClickListener() {
	
				@Override
				public void onClick(DialogInterface dialog, int which) {
					startActivity(new Intent(getActivity(), KeySpriteEditorActivity.class));
				}
			})
			.setOnCancelListener(new DialogInterface.OnCancelListener() {
				
				@Override
				public void onCancel(DialogInterface dialog) {
					if(mLitener != null)
						mLitener.onCancel();
				}
			})
			.create();
	}
}
