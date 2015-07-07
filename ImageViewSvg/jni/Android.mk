# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := svgandroid

LOCAL_CFLAGS += \
	-DLIBSVG_EXPAT \
	-Ijni/libexpat/ \
	-Ijni/libexpat/expat/ \
	-Ijni/libpng/ \
	-Ijni/libjpeg/ \
	-Ijni/libsvg/ \
	-DHAVE_CONFIG_H \
	-DHAVE_EXPAT_CONFIG_H \
	-Wall

# debugging
LOCAL_CFLAGS += -DDEBUG_LIBSVG_ANDROID

# libjpeg stuff
LIBJPEG_SOURCES = \
	libjpeg/jaricom.c libjpeg/jcapimin.c \
	libjpeg/jcapistd.c libjpeg/jcarith.c \
	libjpeg/jccoefct.c libjpeg/jccolor.c \
	libjpeg/jcdctmgr.c libjpeg/jchuff.c \
	libjpeg/jcinit.c libjpeg/jcmainct.c \
	libjpeg/jcmarker.c libjpeg/jcmaster.c \
	libjpeg/jcomapi.c libjpeg/jcparam.c \
	libjpeg/jcprepct.c libjpeg/jcsample.c \
	libjpeg/jctrans.c libjpeg/jdapimin.c \
	libjpeg/jdapistd.c libjpeg/jdarith.c \
	libjpeg/jdatadst.c libjpeg/jdatasrc.c \
	libjpeg/jdcoefct.c libjpeg/jdcolor.c \
	libjpeg/jddctmgr.c libjpeg/jdhuff.c \
	libjpeg/jdinput.c libjpeg/jdmainct.c \
	libjpeg/jdmarker.c libjpeg/jdmaster.c \
	libjpeg/jdmerge.c libjpeg/jdpostct.c \
	libjpeg/jdsample.c libjpeg/jdtrans.c \
	libjpeg/jerror.c libjpeg/jfdctflt.c \
	libjpeg/jfdctfst.c libjpeg/jfdctint.c \
	libjpeg/jidctflt.c libjpeg/jidctfst.c \
	libjpeg/jidctint.c libjpeg/jquant1.c \
	libjpeg/jquant2.c libjpeg/jutils.c \
	libjpeg/jmemmgr.c libjpeg/jmemnobs.c

# zlib stuff
ZLIB_SOURCES =  \
	zlib/adler32.c zlib/compress.c \
	zlib/crc32.c zlib/deflate.c zlib/gzclose.c \
	zlib/gzlib.c zlib/gzread.c \
	zlib/gzwrite.c zlib/infback.c zlib/inffast.c \
	zlib/inflate.c zlib/inftrees.c \
	zlib/trees.c zlib/uncompr.c zlib/zutil.c

# libpng stuff
LIBPNG_SOURCES = \
	libpng/png.c libpng/pngset.c \
	libpng/pngget.c libpng/pngrutil.c \
	libpng/pngtrans.c libpng/pngwutil.c \
	libpng/pngread.c libpng/pngrio.c libpng/pngwio.c \
	libpng/pngwrite.c libpng/pngrtran.c \
	libpng/pngwtran.c libpng/pngmem.c \
	libpng/pngerror.c libpng/pngpread.c \
	libpng/png.h libpng/pngconf.h libpng/pngpriv.h

# libexpat stuff
LIBEXPAT_SOURCES = libexpat/expat/xmlparse.c \
	libexpat/expat/xmlrole.c libexpat/expat/xmltok.c \
	libexpat/expat/xmltok_impl.c libexpat/expat/xmltok_ns.c


# libsvg stuff
LIBSVG_SOURCES = \
	libsvg/svg.c \
	libsvg/svg.h \
	libsvg/svgint.h \
	libsvg/svg_ascii.h \
	libsvg/svg_ascii.c \
	libsvg/svg_attribute.c \
	libsvg/svg_color.c \
	libsvg/svg_element.c \
	libsvg/svg_gradient.c \
	libsvg/svg_group.c \
	libsvg/svg_length.c \
	libsvg/svg_paint.c \
	libsvg/svg_parser.c \
	libsvg/svg_pattern.c \
	libsvg/svg_image.c \
	libsvg/svg_path.c \
	libsvg/svg_str.c \
	libsvg/svg_style.c \
	libsvg/svg_text.c \
	libsvg/svg_transform.c \
	libsvg/svg_version.h \
	libsvg/svg_parser_expat.c \
	libsvg/svg_hash.c \
	libsvg/svg_hash.h

# libsvg-android stuff
LIBSVG_ANDROID_SOURCES = \
	libsvg-android/svg_android.c \
	libsvg-android/svg_android_render.c \
	libsvg-android/svg_android_render_helper.c \
	libsvg-android/svg_android_state.c \
	libsvg-android/svg-android.h \
	libsvg-android/svg-android-internal.h

# package it

LOCAL_SRC_FILES := \
	$(LIBJPEG_SOURCES) \
	$(LIBPNG_SOURCES) \
	$(ZLIB_SOURCES) \
	$(LIBEXPAT_SOURCES) \
	$(LIBSVG_SOURCES) \
	$(LIBSVG_ANDROID_SOURCES)
	
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)



# libsvgagg

include $(CLEAR_VARS)

LOCAL_MODULE    := svgagg
LOCAL_CFLAGS +=\
	-Ijni/libagg/agg/include/ \
	-Ijni/libagg/agg/gpc/ \
	-Ijni/libagg/aggsvg \
	-Ijni/libexpat/ \
	-Ijni/libexpat/expat/ \
	-DHAVE_CONFIG_H \
	-DHAVE_EXPAT_CONFIG_H \
	-Wall \
	-fexceptions

# debugging
LOCAL_CFLAGS += -DDEBUG_LIBSVG_ANDROID

# libjpeg stuff
LIBAGG_SOURCES = \
	libagg/agg/src/agg_arc.cpp \
	libagg/agg/src/agg_image_filters.cpp \
	libagg/agg/src/agg_line_aa_basics.cpp \
	libagg/agg/src/agg_line_profile_aa.cpp \
	libagg/agg/src/agg_trans_warp_magnifier.cpp \
	libagg/agg/src/agg_vcgen_contour.cpp \
	libagg/agg/src/agg_vcgen_markers_term.cpp \
	libagg/agg/src/agg_vpgen_clip_polygon.cpp \
	libagg/agg/src/agg_vpgen_clip_polyline.cpp \
	libagg/agg/src/agg_vpgen_segmentator.cpp \
	libagg/agg/src/agg_rounded_rect.cpp \
	libagg/agg/src/agg_trans_affine.cpp \
	libagg/agg/src/agg_trans_single_path.cpp \
	libagg/agg/src/agg_bspline.cpp \
	libagg/agg/src/agg_vcgen_bspline.cpp \
	libagg/agg/src/agg_vcgen_stroke.cpp \
	libagg/agg/src/agg_vcgen_smooth_poly1.cpp \
	libagg/agg/src/agg_vcgen_dash.cpp \
	libagg/agg/src/agg_trans_double_path.cpp \
	libagg/agg/src/agg_sqrt_tables.cpp \
	libagg/agg/src/agg_bezier_arc.cpp \
	libagg/agg/src/agg_curves.cpp \
	libagg/agg/src/agg_gsv_text.cpp \
	libagg/agg/src/agg_embedded_raster_fonts.cpp \
	libagg/agg/gpc/gpc.c \
	libagg/aggsvg/agg_svg_parser.cpp \
	libagg/aggsvg/agg_svg_path_renderer.cpp \
	libagg/aggsvg/agg_svg_path_tokenizer.cpp \
	libagg/aggsvg-android/aggsvgandroid.cpp

# libexpat stuff
LIBEXPAT_SOURCES = \
	libexpat/expat/xmlparse.c \
	libexpat/expat/xmlrole.c \
	libexpat/expat/xmltok.c \
	libexpat/expat/xmltok_impl.c \
	libexpat/expat/xmltok_ns.c

# package it
LOCAL_SRC_FILES := \
	$(LIBEXPAT_SOURCES) \
	$(LIBAGG_SOURCES)
	
LOCAL_LDLIBS := -lm \
	-llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
