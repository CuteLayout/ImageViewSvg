package com.imageviewsvg;

import com.imageviewsvg.controls.ImageViewSvg;

import android.app.Activity;
import android.os.Bundle;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;

public class MainActivity extends Activity {
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		final SeekBar scale = (SeekBar) findViewById(R.id.ScaleSeek);
		final ImageViewSvg svgView = (ImageViewSvg) findViewById(R.id.svgvie);

		final int width = svgView.getLayoutParams().width;
		final int height = svgView.getLayoutParams().height;

		scale.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

			public void onStopTrackingTouch(SeekBar seekBar) {
			}

			public void onStartTrackingTouch(SeekBar seekBar) {
			}

			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				if (width > 0 && height > 0) {
					int size = (int) (((float) progress / (float) 50) * width);
					svgView.setScale(size);
				}
			}
		});

		RadioGroup g = (RadioGroup) findViewById(R.id.sizegroup);
		g.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {

			public void onCheckedChanged(RadioGroup group, int checkedId) {
				RadioButton r = (RadioButton) findViewById(R.id.RadioButton01);
				if (r.isChecked())
					svgView.setUseAgg(false);
				else
					svgView.setUseAgg(true);
				scale.setProgress(50);
			}
		});
	}
}