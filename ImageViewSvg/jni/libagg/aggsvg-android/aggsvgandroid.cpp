#include <string.h>
#include <jni.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <math.h>

#include "../agg/include/agg_basics.h"
#include "../agg/include/agg_rendering_buffer.h"
#include "../agg/include/agg_rasterizer_scanline_aa.h"
#include "../agg/include/agg_scanline_p.h"
#include "../agg/include/agg_renderer_scanline.h"
#include "../agg/include/agg_pixfmt_rgba.h"
#include "../aggsvg/agg_svg_parser.h"


#ifdef DEBUG_LIBSVG_ANDROID
#define  LOG_TAG    "aggsvg"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define  LOGI(...)  do{}while(0)
#define  LOGE(...)  do{}while(0)
#endif


enum pix_format_e
{
    pix_format_undefined = 0,  // By default. No conversions are applied
    pix_format_bw,             // 1 bit per color B/W
    pix_format_gray8,          // Simple 256 level grayscale
    pix_format_gray16,         // Simple 65535 level grayscale
    pix_format_rgb555,         // 15 bit rgb. Depends on the byte ordering!
    pix_format_rgb565,         // 16 bit rgb. Depends on the byte ordering!
    pix_format_rgbAAA,         // 30 bit rgb. Depends on the byte ordering!
    pix_format_rgbBBA,         // 32 bit rgb. Depends on the byte ordering!
    pix_format_bgrAAA,         // 30 bit bgr. Depends on the byte ordering!
    pix_format_bgrABB,         // 32 bit bgr. Depends on the byte ordering!
    pix_format_rgb24,          // R-G-B, one byte per color component
    pix_format_bgr24,          // B-G-R, native win32 BMP format.
    pix_format_rgba32,         // R-G-B-A, one byte per color component
    pix_format_argb32,         // A-R-G-B, native MAC format
    pix_format_abgr32,         // A-B-G-R, one byte per color component
    pix_format_bgra32,         // B-G-R-A, native win32 BMP format
    pix_format_rgb48,          // R-G-B, 16 bits per color component
    pix_format_bgr48,          // B-G-R, native win32 BMP format.
    pix_format_rgba64,         // R-G-B-A, 16 bits byte per color component
    pix_format_argb64,         // A-R-G-B, native MAC format
    pix_format_abgr64,         // A-B-G-R, one byte per color component
    pix_format_bgra64,         // B-G-R-A, native win32 BMP format

    end_of_pix_formats
};

class SvgRasterizer{
	agg::svg::path_renderer m_path;
    double m_min_x;
    double m_min_y;
    double m_max_x;
    double m_max_y;
    double m_x;
    double m_y;
    pix_format_e pixformat;
	agg::rendering_buffer m_rbuf_window;

public:
	SvgRasterizer(pix_format_e format, uint32_t width, uint32_t height, void *pixels) : \
		m_path(), \
		m_min_x(0.0), \
		m_min_y(0.0), \
		m_max_x(0.0), \
		m_max_y(0.0), \
		pixformat(format)
	{
		m_rbuf_window.attach((unsigned char*)pixels, width, height, 4*width);
	}

	void parse_svg(const char* svg, int length){
		// Create parser
		agg::svg::parser p(m_path);
		// Parse SVG
		p.parse(svg, length);
		// Make all polygons CCW-oriented
		m_path.arrange_orientations();
		// Get bounds of the image defined in SVG
        m_path.bounding_rect(&m_min_x, &m_min_y, &m_max_x, &m_max_y);
	}


	void rasterize_svg()
	{
		typedef agg::pixfmt_rgba32 pixfmt;
		typedef agg::renderer_base<pixfmt> renderer_base;
		typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

        pixfmt pixf(m_rbuf_window);
        renderer_base rb(pixf);
        renderer_solid ren(rb);

        agg::rasterizer_scanline_aa<> ras;
        agg::scanline_p8 sl;
        agg::trans_affine mtx;

        double scl;
        // Calculate the scale the image to fit given bitmap
        if(m_max_y > m_max_x)
        	scl = pixf.height()/m_max_y;
        else
        	scl = pixf.width()/m_max_x;

        // Default gamma as is
        ras.gamma(agg::gamma_power(1.0));
        mtx *= agg::trans_affine_scaling(scl);

        m_path.expand(0.0);

        // Render image
        m_path.render(ras, sl, ren, mtx, rb.clip_box(), 1.0);

        ras.gamma(agg::gamma_none());
	}
};

extern "C" {
    JNIEXPORT void JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_RasterToBitmap(JNIEnv * env, jobject obj,  jobject bitmap, jstring svg);
};


JNIEXPORT void JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_RasterToBitmap(JNIEnv * env, jobject obj,  jobject bitmap, jstring svg)
{
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }


    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return;
    }

    const char* svgstring = env->GetStringUTFChars(svg, 0);
    int length = env->GetStringLength(svg);

    // It should be pure char array so expat coud process it
    char* strr = new char[length+1];
    memset(strr, 0, length + 1);
    strncpy(strr, svgstring, length);
    env->ReleaseStringUTFChars(svg, svgstring);

    SvgRasterizer rst(pix_format_argb32, info.height, info.height, pixels);
    rst.parse_svg(strr, length);
    rst.rasterize_svg();

    AndroidBitmap_unlockPixels(env, bitmap);
}

