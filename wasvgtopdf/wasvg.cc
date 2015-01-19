#include "wasvg.h"

#include <string>
#include <iostream>
#include <vector>

extern char *optarg;
extern int optind, opterr;

static void usage(void){
    fprintf(stderr, "usage: wasvg [options]\n");
    fprintf(stderr, " -%c: %s\n", OPT_TO_PDF, "to pdf"); 
    fprintf(stderr, " -%c: %s\n", OPT_TO_PNG, "to png"); 
    fprintf(stderr, " -%c number: %s\n", OPT_DPI_X, "dpi x");
    fprintf(stderr, " -%c number: %s\n", OPT_DPI_Y, "dpi y");
    fprintf(stderr, " -%c number: %s\n", OPT_ZOOM_X, "zoom x");
    fprintf(stderr, " -%c number: %s\n", OPT_ZOOM_Y, "zoom y");
    fprintf(stderr, " -%c number: %s\n", OPT_WIDTH, "width");    
    fprintf(stderr, " -%c number: %s\n", OPT_HEIGHT, "height");        
    fprintf(stderr, " -%c number: %s\n", OPT_KEEP_ASPECT_RATIO, "keep aspect ratio"); 
    fprintf(stderr, " -%c number: %s\n", OPT_ZOOM , "zoom");       
    fprintf(stderr, " -%c string: %s\n", OPT_URI , "uri");    
    fprintf(stderr, " -%c string: %s\n", OPT_INPUT , "input"); 
    fprintf(stderr, " -%c string: %s\n", OPT_OUTPUT , "output"); 
    
    exit(1);
}

int main(int argc, char *argv[])
{
    if(argc == 1) usage();
    
    int ch;
        
    double dpi_x = -1, dpi_y = -1;
    int width = -1, height = -1;
    double x_zoom = 1, y_zoom = 1, zoom = 0;
    int keep_aspect_ratio = 1;
    
    std::string uri, input, output;
    
    int mode = 0;
    
    while ((ch = getopt(argc, argv, OPT_LIST)) != -1){
                
        switch (ch){
            case OPT_TO_PDF:
                mode = OPT_TO_PDF;
                break;
            case OPT_TO_PNG:
                mode = OPT_TO_PNG;
                break;
            case OPT_DPI_X:
                dpi_x = atof(optarg);
                break; 
            case OPT_DPI_Y:
                dpi_y = atof(optarg);
                break;
            case OPT_ZOOM_X:
                x_zoom = atoi(optarg);
                break; 
            case OPT_ZOOM_Y:
                y_zoom = atoi(optarg);
                break; 
            case OPT_WIDTH:
                width = atoi(optarg);
            break;                 
            case OPT_HEIGHT:
                height = atoi(optarg);
                break;  
            case OPT_KEEP_ASPECT_RATIO:
                keep_aspect_ratio = atoi(optarg);
                break;  
            case OPT_ZOOM:
                zoom = atoi(optarg);
                break;  
            case OPT_URI:
                uri = std::string(optarg);
                break;                  
            case OPT_INPUT:
#ifdef __WINDOWS__
                input = std::string(optarg);
                unescape_path(input);
#else                
                input = std::string(optarg);
#endif                
                break;
            case OPT_OUTPUT:
#ifdef __WINDOWS__
                output = std::string(optarg);
                unescape_path(output);                
#else
                output = std::string(optarg);
#endif                
                break;                    
            default:
                usage();
        }
    } 
    
    bool isReady = false;
    
    if(mode){
    
        unsigned int size = BUFFER_SIZE;
        
        char *buf = (char *)calloc(size, sizeof(char));
        
        if(!buf){
            exit(1);
        }
        
        unsigned int ret, i = 0;
        
        if(input.length()){
#ifdef __WINDOWS__
            FILE *fp = _fopen(input, L"rb");
#else
            FILE *fp = fopen(input.c_str(), "rb");    
#endif            
            if(fp){
                while((ret = (unsigned int)fread(&buf[i], sizeof(char), BUFFER_SIZE, fp)) > 0){
                    i += ret;
                    size += BUFFER_SIZE;
                    buf = (char *)realloc(buf, size);
                    if(!buf){
                        exit(1);
                    }
                }   
                fclose(fp);
                isReady = true;
            }
        }
        
        if(!isReady){
            while((ret = (unsigned int)fread(&buf[i], sizeof(char), BUFFER_SIZE, stdin)) > 0){
                i += ret;
                size += BUFFER_SIZE;
                buf = (char *)realloc(buf, size);
                if(!buf){
                    exit(1);
                }
            }
            isReady = true;
        }
        
        if(!isReady){
            usage();
        }
         
        RsvgHandle *rsvg;
        cairo_surface_t *surface = NULL;
        
        FILE *out = NULL;
        
        if(output.length()){
#ifdef __WINDOWS__
            out = _fopen(output, L"wb");
#else
            out = fopen(output.c_str(), "wb");
#endif            
            if(!out){
                exit(1);
            }
        }else{
            out = stdout;
        }
        
        RsvgDimensionData dimensions;
        GError *error = NULL;
        
        const guint8 *p = (guint8 *)buf;
        
        if(i){
                        
            rsvg_set_default_dpi_x_y (dpi_x, dpi_y);
            
            rsvg = rsvg_handle_new_from_data(p, i, &error);
            
            if(rsvg){
                
                if(uri.size())
                    rsvg_handle_set_base_uri (rsvg, (const char *)uri.c_str());
                
                struct RsvgSizeCallbackData size_data;
                rsvg_handle_get_dimensions (rsvg, &dimensions);
                
                if (width == -1 && height == -1) {
                    size_data.type = RSVG_SIZE_ZOOM;
                    size_data.x_zoom = x_zoom;
                    size_data.y_zoom = y_zoom;
                    size_data.keep_aspect_ratio = keep_aspect_ratio;
                }
                else if (x_zoom == 1.0 && y_zoom == 1.0) {
                    if (width == -1 || height == -1) {
                        size_data.type = RSVG_SIZE_WH_MAX;
                        size_data.width = width;
                        size_data.height = height;
                        size_data.keep_aspect_ratio = keep_aspect_ratio;
                    } else {
                        size_data.type = RSVG_SIZE_WH;
                        size_data.width = width;
                        size_data.height = height;
                        size_data.keep_aspect_ratio = keep_aspect_ratio;
                    }
                } else {
                    size_data.type = RSVG_SIZE_ZOOM_MAX;
                    size_data.x_zoom = x_zoom;
                    size_data.y_zoom = y_zoom;
                    size_data.width = width;
                    size_data.height = height;
                    size_data.keep_aspect_ratio = keep_aspect_ratio;
                }
                                
                switch (mode){
                    case OPT_TO_PNG:
                        surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, dimensions.width, dimensions.height);
                        break;                        
                    case OPT_TO_PDF:
                        surface = cairo_pdf_surface_create_for_stream(rsvg_cairo_write_func, 
                                                                       (void *)out,
                                                                       dimensions.width, dimensions.height);
                        break;
                }
                
                if(surface){
                    
                    cairo_t *cr = NULL;	
                    cr = cairo_create(surface);    
                    
                    rsvg_handle_render_cairo (rsvg, cr);
                    
                    switch (mode){
                        case OPT_TO_PNG:
                            cairo_surface_write_to_png_stream (surface, rsvg_cairo_write_func, (void *)out);
                            break;	
                            
                        case OPT_TO_PDF:
                            cairo_show_page(cr);
                            break;
                    }
                    
                    cairo_destroy(cr);	
                    cairo_surface_destroy(surface);	
                }
                rsvg_handle_close(rsvg, &error);
                g_clear_object(&rsvg);
            }
            
        }
        free(buf);    
    }
    
	return 0;
}

static cairo_status_t rsvg_cairo_write_func(void *data, const unsigned char *bytes, unsigned int len)
{	    
    if(len){  
        fwrite(bytes, 1, len, (FILE *)data);  
    }    
    
    return CAIRO_STATUS_SUCCESS;
}

