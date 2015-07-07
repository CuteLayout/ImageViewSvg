package com.imageviewsvg.controls;

import java.io.IOException;
import java.io.InputStream;

import org.apache.http.util.EncodingUtils;

import com.imageviewsvg.R;
import com.toolkits.libsvgandroid.SvgRaster;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.ViewGroup;
import android.widget.ImageView;

/**
 * 
 * @author kushnarev
 *
 */

/**
 * Image View class with SVG format support
 */
public class ImageViewSvg extends ImageView {

	// Bitmap to be set as a source
	private Bitmap mBitmap;
	// Content of the SVG file
	private String mSvgContent = "";
	// Is resource SVG file?
	private boolean mIsSvg = false;
	// Use Anti Grain Geometry
	private boolean mUseAgg = true;
	// libsvg-android rasterizer id
	private long mSvgId;

	// Load native libraries
	static {
		System.loadLibrary("svgandroid");
		System.loadLibrary("svgagg");
	}

	public ImageViewSvg(Context context) {
		super(context);
	}

	public ImageViewSvg(Context context, AttributeSet attrs) {
		this(context, attrs, 0);
	}

	public ImageViewSvg(Context context, AttributeSet attrs, int defStyle) {

		// Let's try load supported by ImageView formats
		super(context, attrs, defStyle);

		if (getDrawable() == null) {
			// Get defined attributes
			TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.ImageViewSvg, defStyle, 0);

			// Getting a file name
			CharSequence cs = a.getText(R.styleable.ImageViewSvg_android_src);
			String file = cs.toString();

			// Is it SVG file?
			if (file.endsWith(".svg")) {

				// Retrieve ID of the resource
				int id = a.getResourceId(R.styleable.ImageViewSvg_android_src, -1);
				if (id != -1) {
					try {
						// Get the input stream for the raw resource
						InputStream inStream = getResources().openRawResource(id);
						int size = inStream.available();

						// Read into the buffer
						byte[] buffer = new byte[size];
						inStream.read(buffer);
						inStream.close();

						// And make a string
						mSvgContent = EncodingUtils.getString(buffer, "UTF-8");

						// Parse it
						mSvgId = SvgRaster.svgAndroidCreate();
						SvgRaster.svgAndroidParseBuffer(mSvgId, mSvgContent);
						SvgRaster.svgAndroidSetAntialiasing(mSvgId, true);

						mIsSvg = true;

					} catch (IOException e) {
						mIsSvg = false;
						e.printStackTrace();
					}
				}
			}
		}
	}

	@Override
	public void setLayoutParams(ViewGroup.LayoutParams params) {
		if (mIsSvg) {
			// replace WRAP_CONTENT if needed
			if (params.width == ViewGroup.LayoutParams.WRAP_CONTENT && getSuggestedMinimumWidth() == 0)
				params.width = ViewGroup.LayoutParams.MATCH_PARENT;
			if (params.height == ViewGroup.LayoutParams.WRAP_CONTENT && getSuggestedMinimumHeight() == 0)
				params.height = ViewGroup.LayoutParams.MATCH_PARENT;
		}
		super.setLayoutParams(params);
	}

	@Override
	public void onSizeChanged(int w, int h, int ow, int oh) {
		if (mIsSvg) {
			if (!mUseAgg) {
				// Create the bitmap to raster svg to
				Canvas canvas = new Canvas();
				mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
				canvas.setBitmap(mBitmap);
				// Render SVG with use of libandroidsvg
				SvgRaster.svgAndroidRenderToArea(mSvgId, canvas, 0, 0, canvas.getWidth(), canvas.getHeight());
			} else {
				mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
				SvgRaster.RasterToBitmap(mBitmap, mSvgContent);
			}
			setImageBitmap(mBitmap);
		} else
			super.onSizeChanged(w, h, ow, oh);
	}

	public void setScale(int size) {

		if (mIsSvg) {
			size = size == 0 ? 1 : size;
			ViewGroup.LayoutParams p = getLayoutParams();
			if (p.height > 0 && p.width > 0) {
				p.height = size;
				p.width = size;
				setLayoutParams(p);
			}
		}
	}

	public void setUseAgg(boolean agg) {
		mUseAgg = agg;
		if (mIsSvg) {
			if (!mUseAgg) {
				Canvas canvas = new Canvas();
				mBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
				canvas.setBitmap(mBitmap);
				SvgRaster.svgAndroidRenderToArea(mSvgId, canvas, 0, 0, canvas.getWidth(), canvas.getHeight());
			} else {
				mBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
				SvgRaster.RasterToBitmap(mBitmap, mSvgContent);
			}
			setImageBitmap(mBitmap);
		}
	}

	@Override
	public void finalize() throws Throwable {
		SvgRaster.svgAndroidDestroy(mSvgId);
		super.finalize();
	}
}
