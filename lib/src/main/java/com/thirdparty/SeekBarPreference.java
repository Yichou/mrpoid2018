package com.thirdparty;

import android.content.Context;
import android.content.res.TypedArray;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.mrpoid.R;

public class SeekBarPreference extends DialogPreference
		implements SeekBar.OnSeekBarChangeListener {

//	private static final String LOG_TAG = "SeekBarPreference";
	private static final String NS = "http://androidemu.com/apk/res/android";

	private SeekBar seekBar;
	private TextView valueView;
	private int minValue, maxValue;
	private int oldValue, newValue;

	public SeekBarPreference(Context context, AttributeSet attrs) {
		super(context, attrs);

		minValue = attrs.getAttributeIntValue(NS, "minValue", 0);
		maxValue = attrs.getAttributeIntValue(NS, "maxValue", 100);

		setDialogLayoutResource(R.layout.seekbar_dialog);
		setPositiveButtonText(android.R.string.ok);
		setNegativeButtonText(android.R.string.cancel);
	}

	@Override
	protected void onBindDialogView(View view) {
		super.onBindDialogView(view);

		if (newValue < minValue)
			newValue = minValue;
		if (newValue > maxValue)
			newValue = maxValue;

		seekBar = (SeekBar) view.findViewById(R.id.seekbar);
		seekBar.setMax(maxValue - minValue);
		seekBar.setProgress(newValue - minValue);
		seekBar.setSecondaryProgress(newValue - minValue);
		seekBar.setOnSeekBarChangeListener(this);

		valueView = (TextView) view.findViewById(R.id.value);
		valueView.setText(Integer.toString(newValue));
	}

	public void onProgressChanged(SeekBar seekBar,
			int progress, boolean fromUser) {
		newValue = progress + minValue;
		valueView.setText(Integer.toString(newValue));
	}

	public void onStartTrackingTouch(SeekBar seekBar) {
	}

	public void onStopTrackingTouch(SeekBar seekBar) {
	}

	@Override
	protected void onDialogClosed(boolean positiveResult) {
		super.onDialogClosed(positiveResult);

		if (!positiveResult)
			newValue = oldValue;
		else {
			oldValue = newValue;
			persistInt(newValue);
		}
	}

	@Override
	protected Object onGetDefaultValue(TypedArray a, int index) {
		return a.getInteger(index, 0);
	}

	@Override
	protected void onSetInitialValue(
			boolean restoreValue, Object defaultValue) {
		oldValue = (restoreValue ?
				getPersistedInt(0) : ((Integer) defaultValue).intValue());
		newValue = oldValue;
	}
}
