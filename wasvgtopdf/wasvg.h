#ifndef __WASVG_H__
#define __WASVG_H__

#define OPT_TO_PDF        'P'
#define OPT_TO_PNG        'p'
#define OPT_DPI_X         'x'
#define OPT_DPI_Y         'y'
#define OPT_ZOOM_X        'X'
#define OPT_ZOOM_Y        'Y'
#define OPT_WIDTH         'w'
#define OPT_HEIGHT        'h'
#define OPT_KEEP_ASPECT_RATIO        'a'
#define OPT_ZOOM          'z'
#define OPT_URI           'u'
#define OPT_INPUT         'i'
#define OPT_OUTPUT        'o'

#define OPT_LIST            "Ppx:y:X:Y:w:h:az:u:i:o:"

#define BUFFER_SIZE 8192

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <string.h>
#include <locale.h>

#include <rsvg.h>
#include <cairo-ps.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>

#ifdef __APPLE__
#include <cairo-quartz.h>
#endif

typedef enum {
    RSVG_SIZE_ZOOM,
    RSVG_SIZE_WH,
    RSVG_SIZE_WH_MAX,
    RSVG_SIZE_ZOOM_MAX
} RsvgSizeType;

struct RsvgSizeCallbackData {
    RsvgSizeType type;
    double x_zoom;
    double y_zoom;
    gint width;
    gint height;
    
    gboolean keep_aspect_ratio;
};

#ifndef __linux__
#ifndef __APPLE__
#define __WINDOWS__
#include <string>
#include "Shlwapi.h"
#include "WinInet.h"
void unescape_path(std::string &path);
FILE *_fopen(std::string &path, const wchar_t *mode);
#endif
#endif

static cairo_status_t rsvg_cairo_write_func(void *data, const unsigned char *bytes, unsigned int len);

#define PACK_RGBA(r,g,b,a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define PACK_RGB(r,g,b) PACK_RGBA(r, g, b, 255)

#endif