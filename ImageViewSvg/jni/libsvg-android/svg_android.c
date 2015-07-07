/* libsvg-android - Render SVG documents to an Android canvas
 *
 * Copyright © 2002 University of Southern California
 * Copyright © 2010 Anton Persson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy (COPYING.LESSER) of the
 * GNU Lesser General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Original Cairo-version:
 * Author: Carl D. Worth <cworth@isi.edu>
 *
 * Android modification:
 * Author: Anton Persson {don d0t juanton 4t gmail d0t com}
 *
 */

#include <stdlib.h>
#include <string.h>

#include "svg-android-internal.h"
#include "math.h"

#include <android/log.h>

svg_render_engine_t SVG_ANDROID_RENDER_ENGINE = {
	/* hierarchy */
	_svg_android_begin_group,
	_svg_android_begin_element,
	_svg_android_end_element,
	_svg_android_end_group,
	/* path creation */
	_svg_android_move_to,
	_svg_android_line_to,
	_svg_android_curve_to,
	_svg_android_quadratic_curve_to,
	_svg_android_arc_to,
	_svg_android_close_path,
	/* style */
	_svg_android_set_color,
	_svg_android_set_fill_opacity,
	_svg_android_set_fill_paint,
	_svg_android_set_fill_rule,
	_svg_android_set_font_family,
	_svg_android_set_font_size,
	_svg_android_set_font_style,
	_svg_android_set_font_weight,
	_svg_android_set_opacity,
	_svg_android_set_stroke_dash_array,
	_svg_android_set_stroke_dash_offset,
	_svg_android_set_stroke_line_cap,
	_svg_android_set_stroke_line_join,
	_svg_android_set_stroke_miter_limit,
	_svg_android_set_stroke_opacity,
	_svg_android_set_stroke_paint,
	_svg_android_set_stroke_width,
	_svg_android_set_text_anchor,
	/* transform */
	_svg_android_transform,
	_svg_android_apply_view_box,
	_svg_android_set_viewport_dimension,
	/* drawing */
	_svg_android_render_line,
	_svg_android_render_path,
	_svg_android_render_ellipse,
	_svg_android_render_rect,
	_svg_android_render_text,
	_svg_android_render_image
};

svg_android_status_t svgAndroidDestroy(svg_android_t *svg_android) {
	svg_android_status_t status;

	_svg_android_pop_state (svg_android);

	status = svg_destroy (svg_android->svg);

	free (svg_android);

	return status;
}

svg_android_t *svgAndroidCreate(void) {
	svg_android_t *svg_android;	
	
	svg_android = (svg_android_t *)malloc (sizeof (svg_android_t));

	if (svg_android != NULL) {
		svg_android->do_antialias = JNI_FALSE;
		
		svg_android->canvas = NULL;
		svg_android->state = NULL;
				
		if(svg_create (&(svg_android)->svg)) {
			free(svg_android);
			svg_android = NULL;
		}
	}

	return svg_android;
}

JNIEXPORT jlong JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidCreate
(JNIEnv * env, jclass jc)
{	
	return (jlong)svgAndroidCreate();
}

JNIEXPORT jint JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidDestroy
(JNIEnv *env, jclass jc, jlong _svg_android_r)
{
	svg_android_t *svg_android = (svg_android_t *)_svg_android_r;

	return svgAndroidDestroy(svg_android);
}

JNIEXPORT jint JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidParseBuffer
(JNIEnv *env, jclass jc, jlong _svg_android_r, jstring _bfr)
{
	svg_android_t *svg_android = (svg_android_t *)_svg_android_r;

	
	const char *buf = (*env)->GetStringUTFChars(env, _bfr, JNI_FALSE);

	svg_android_status_t status = svg_parse_buffer (svg_android->svg, buf, strlen(buf));
	
	(*env)->ReleaseStringUTFChars(env, _bfr, buf); 

	return status;
}

JNIEXPORT jint JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidParseChunkBegin
(JNIEnv *env, jclass jc, jlong _svg_android_r)
{
	svg_android_t *svg_android = (svg_android_t *)_svg_android_r;
	return svg_parse_chunk_begin (svg_android->svg);
}

JNIEXPORT jint JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidParseChunk
(JNIEnv *env, jclass jc, jlong _svg_android_r, jstring _bfr)
{
	svg_android_t *svg_android = (svg_android_t *)_svg_android_r;

	const char *buf = (*env)->GetStringUTFChars(env, _bfr, JNI_FALSE);

	svg_android_status_t status = svg_parse_chunk (svg_android->svg, buf, strlen(buf));
	
	(*env)->ReleaseStringUTFChars(env, _bfr, buf); 

	return status;
}

JNIEXPORT jint JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidParseChunkEnd
(JNIEnv *env, jclass jc, jlong _svg_android_r)
{
	svg_android_t *svg_android = (svg_android_t *)_svg_android_r;
	return svg_parse_chunk_end (svg_android->svg);
}

void __prepare_android_interface(svg_android_t *svg_android, JNIEnv *env, jobject *android_canvas) {
	svg_android->env = env;
	
	svg_android->canvas = android_canvas;
	
	// prepare canvas class/methods
//	svg_android->canvas_clazz = (*env)->GetObjectClass(env, android_canvas);
	svg_android->canvas_clazz = (*env)->FindClass(env, "android/graphics/Canvas");

	svg_android->canvas_constructor = (*env)->GetMethodID(env,
		svg_android->canvas_clazz, "<init>", "(Landroid/graphics/Bitmap;)V");
	svg_android->canvas_save = (*env)->GetMethodID(env,svg_android->canvas_clazz, "save", "()I");
	svg_android->canvas_restore = (*env)->GetMethodID(env,svg_android->canvas_clazz, "restore", "()V");
	svg_android->canvas_draw_bitmap = (*env)->GetMethodID(env,
		svg_android->canvas_clazz, "drawBitmap",
		"(Landroid/graphics/Bitmap;Landroid/graphics/Matrix;Landroid/graphics/Paint;)V");	
	svg_android->canvas_draw_path = (*env)->GetMethodID(env,
		svg_android->canvas_clazz, "drawPath",
		"(Landroid/graphics/Path;Landroid/graphics/Paint;)V");
	svg_android->canvas_draw_text = (*env)->GetMethodID(env,
		svg_android->canvas_clazz, "drawText",
		"(Ljava/lang/String;FFLandroid/graphics/Paint;)V");
	svg_android->canvas_drawRGB =  (*env)->GetMethodID(env,
		svg_android->canvas_clazz, "drawRGB",
		"(III)V");
	svg_android->canvas_getWidth =  (*env)->GetMethodID(env,
		svg_android->canvas_clazz, "getWidth",
		"()I");
	svg_android->canvas_getHeight =  (*env)->GetMethodID(env,
		svg_android->canvas_clazz, "getHeight",
		"()I");
	
	// prepare raster
	svg_android->raster_clazz = (*env)->FindClass(env, "com/toolkits/libsvgandroid/SvgRaster");

	svg_android->raster_setTypeface = (*env)->GetStaticMethodID(
		env,
		svg_android->raster_clazz, "setTypeface",
		"(Landroid/graphics/Paint;Ljava/lang/String;IFI)V"
		);
	svg_android->raster_getBounds = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "getBounds",
		"(Landroid/graphics/Path;)[F"
		);
	svg_android->raster_matrixInit = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "matrixInit",
		"(FFFFFF)Landroid/graphics/Matrix;"
		);
	svg_android->raster_createBitmap = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "createBitmap",
		"(II)Landroid/graphics/Bitmap;"
		);
	svg_android->raster_data2bitmap = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "data2bitmap",
		"(II[I)Landroid/graphics/Bitmap;"
		);
	svg_android->raster_setFillRule = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "setFillRule",
		"(Landroid/graphics/Path;Z)V"
		);
	svg_android->raster_setPaintStyle = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "setPaintStyle",
		"(Landroid/graphics/Paint;Z)V"
		);
	svg_android->raster_setStrokeCap = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "setStrokeCap",
		"(Landroid/graphics/Paint;I)V"
		);
	svg_android->raster_setStrokeJoin = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "setStrokeJoin",
		"(Landroid/graphics/Paint;I)V"
		);
	svg_android->raster_createBitmapShader = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "createBitmapShader",
		"(Landroid/graphics/Bitmap;)Landroid/graphics/Shader;"
		);
	svg_android->raster_createLinearGradient = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "createLinearGradient",
		"(FFFF[I[FI)Landroid/graphics/Shader;"
		);
	svg_android->raster_createRadialGradient = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "createRadialGradient",
		"(FFF[I[FI)Landroid/graphics/Shader;"
		);
	svg_android->raster_matrixInvert = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "matrixInvert", "(Landroid/graphics/Matrix;)Landroid/graphics/Matrix;");
	svg_android->raster_drawEllipse = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "drawEllipse", "(Landroid/graphics/Canvas;Landroid/graphics/Paint;FFFF)V");
	svg_android->raster_debugMatrix = (*env)->GetStaticMethodID(env,
		svg_android->raster_clazz, "debugMatrix", "(Landroid/graphics/Matrix;)V");
	
	// prepare bitmap class/methods
	svg_android->bitmap_clazz = (*env)->FindClass(env, "android/graphics/Bitmap");

	svg_android->bitmap_erase_color = (*env)->GetMethodID(env,
		svg_android->bitmap_clazz, "eraseColor",
		"(I)V"
		);

	// prepare matrix class/methods
	svg_android->matrix_clazz = (*env)->FindClass(env, "android/graphics/Matrix");

	svg_android->matrix_constructor = (*env)->GetMethodID(env,
		svg_android->matrix_clazz, "<init>", "()V");
	svg_android->matrix_copy_constructor = (*env)->GetMethodID(env,
		svg_android->matrix_clazz, "<init>", "(Landroid/graphics/Matrix;)V");
	svg_android->matrix_postTranslate = (*env)->GetMethodID(env,
		svg_android->matrix_clazz, "postTranslate", "(FF)Z");
	svg_android->matrix_postScale = (*env)->GetMethodID(env,
		svg_android->matrix_clazz, "postScale", "(FF)Z");
	svg_android->matrix_postConcat = (*env)->GetMethodID(env,
		svg_android->matrix_clazz, "postConcat", "(Landroid/graphics/Matrix;)Z");

	// prepare shader class/methods
	svg_android->shader_clazz = (*env)->FindClass(env, "android/graphics/Shader");

	svg_android->shader_setLocalMatrix = (*env)->GetMethodID(env,
		svg_android->shader_clazz, "setLocalMatrix", "(Landroid/graphics/Matrix;)V");

	// prepare path class/methods
	svg_android->path_clazz = (*env)->FindClass(env, "android/graphics/Path");

	svg_android->path_constructor = (*env)->GetMethodID(env,
		svg_android->path_clazz, "<init>", "()V");	
	svg_android->path_transform = (*env)->GetMethodID(env,
		svg_android->path_clazz, "transform", "(Landroid/graphics/Matrix;)V");
	svg_android->path_moveTo = (*env)->GetMethodID(env,
		svg_android->path_clazz, "moveTo", "(FF)V");
	svg_android->path_lineTo = (*env)->GetMethodID(env,
		svg_android->path_clazz, "lineTo", "(FF)V");
	svg_android->path_cubicTo = (*env)->GetMethodID(env,
		svg_android->path_clazz, "cubicTo", "(FFFFFF)V");
	svg_android->path_quadTo = (*env)->GetMethodID(env,
		svg_android->path_clazz, "quadTo", "(FFFF)V");
	svg_android->path_close = (*env)->GetMethodID(env,
		svg_android->path_clazz, "close", "()V");
	svg_android->path_reset = (*env)->GetMethodID(env,
		svg_android->path_clazz, "reset", "()V");

	// prepare paint class/methods
	svg_android->paint_clazz = (*env)->FindClass(env, "android/graphics/Paint");

	svg_android->paint_constructor = (*env)->GetMethodID(env,
		svg_android->paint_clazz, "<init>", "()V");	
	svg_android->paint_copy_constructor = (*env)->GetMethodID(env,
		svg_android->paint_clazz, "<init>", "(Landroid/graphics/Paint;)V");	
	svg_android->paint_setPathEffect = (*env)->GetMethodID(
		env,
		svg_android->paint_clazz, "setPathEffect",
		"(Landroid/graphics/PathEffect;)Landroid/graphics/PathEffect;");
	svg_android->paint_setARGB = (*env)->GetMethodID(env,
		svg_android->paint_clazz, "setARGB", "(IIII)V");
	svg_android->paint_setShader = (*env)->GetMethodID(env,
		svg_android->paint_clazz, "setShader", "(Landroid/graphics/Shader;)Landroid/graphics/Shader;");
	svg_android->paint_setStrokeMiter = (*env)->GetMethodID(env,
		svg_android->paint_clazz, "setStrokeMiter", "(F)V");
	svg_android->paint_setStrokeWidth = (*env)->GetMethodID(env,
		svg_android->paint_clazz, "setStrokeWidth", "(F)V");
	svg_android->paint_getTextPath = (*env)->GetMethodID(
		env,
		svg_android->paint_clazz, "getTextPath",
		"(Ljava/lang/String;IIFFLandroid/graphics/Path;)V");
	svg_android->paint_setAntialias = (*env)->GetMethodID(env,
		svg_android->paint_clazz, "setAntiAlias", "(Z)V");

	// prepare dash path effect class/methods
	svg_android->dashPathEffect_clazz =
		(*env)->FindClass(env, "android/graphics/DashPathEffect");

	svg_android->dashPathEffect_constructor = (*env)->GetMethodID(env,
		svg_android->dashPathEffect_clazz, "<init>", "([FF)V");	
}

void svgAndroidSetAntialiasing(svg_android_t *svg_android, jboolean doAntiAlias) {
	svg_android->do_antialias = doAntiAlias;
}

JNIEXPORT int JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidSetAntialiasing
(JNIEnv *env, jclass jc, jlong _svg_android_r, jboolean doAntiAlias) {
	svg_android_t *svg_android = (svg_android_t *)_svg_android_r;
	svg_android->do_antialias = doAntiAlias;
	return 0;	
}

svg_status_t svgAndroidRender
(JNIEnv *env, svg_android_t *svg_android, jobject android_canvas)
{
	__prepare_android_interface(svg_android, env, android_canvas); 

	_svg_android_push_state (svg_android, NULL);
	svg_android->state->viewport_width = ANDROID_GET_WIDTH(svg_android);
	svg_android->state->viewport_height = ANDROID_GET_HEIGHT(svg_android);

	svg_android->fit_to_area = 0;
	return svg_render (svg_android->svg, &SVG_ANDROID_RENDER_ENGINE, svg_android);
}

JNIEXPORT jint JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidRender
(JNIEnv *env, jclass jc, jlong _svg_android_r, jobject android_canvas)
{
	svg_android_t *svg_android = (svg_android_t *)_svg_android_r;
	return svgAndroidRender(env, svg_android, android_canvas);
}

svg_status_t svgAndroidRenderToArea(JNIEnv *env, svg_android_t *svg_android, jobject android_canvas, int x, int y, int w, int h) {
	__prepare_android_interface(svg_android, env, android_canvas); 

	_svg_android_push_state (svg_android, NULL);

	svg_android->state->viewport_width = w;
	svg_android->state->viewport_height = h;
	
	svg_android->fit_to_area = -1;
	svg_android->fit_to_x = x;
	svg_android->fit_to_y = y;
	svg_android->fit_to_w = w;
	svg_android->fit_to_h = h;	
	svg_android->fit_to_MATRIX = NULL;
	
	return svg_render (svg_android->svg, &SVG_ANDROID_RENDER_ENGINE, svg_android);
}

JNIEXPORT jint JNICALL Java_com_toolkits_libsvgandroid_SvgRaster_svgAndroidRenderToArea
(JNIEnv *env, jclass jc, jlong _svg_android_r, jobject android_canvas,
 jint x, jint y, jint w, jint h)
{
	svg_android_t *svg_android = (svg_android_t *)_svg_android_r;
	return svgAndroidRenderToArea(env, svg_android, android_canvas, x, y, w, h);
}
