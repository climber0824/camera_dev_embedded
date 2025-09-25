#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jpeglib.h>
#include <png.h>

#define PI 3.14159265369

typedef struct {
    int width;
    int height;
    int channels;
    unsigned char *data;
} Image;

float **createGaussianKernel(int size, float sigma) {
    if (size & 1 == 0) {
        fprintf(stderr, "kernel size must be odd.\n");
        return NULL;
    }

    float **kernel = (float**)malloc(size * sizeof(float*));
    if (!kernel) return NULL;

    // allocate memory
    for (int i = 0; i < size; ++i) {
        kernel[i] = (float*)malloc(size * sizeof(float));
        if (!kernel[i]) {
            // clean up allocated rows
            for (int j = 0; j < i; ++j) free(kernel[j]);
            free(kernel);
            return NULL;
        }
    }

    float sum = 0.0;
    int center = size / 2;
    
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            float x = i - center;
            float y = j - center;
            kernel[i][j] = (1.0 / (2.0 * PI * sigma * sigma)) * exp(-(x*x + y*y) / (2.0 *                           sigma * sigma));
            sum += kernel[i][j];
        }
    }

    // normalize the kernel so the sum of its element is 1
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            kernel[i][j] /= sum;
        }
    }
}

// free the memory for the kernel
// todo: free all types of memory
void freeKernel(float **kernel, int size) {
    if (kernel) {
        for (int i = 0; i < size; ++i) {
            if (kernel[i]) {
                free(kernel[i]);
            }
        }
        free(kernel);
    }
}


// apply Gaussian filter on colored image
void applyGaussianfilter(Image input, Image output, float kernel, int kernel_size) {
    if (!input || !output || !kernel) {
        fprintf(stderr, "Invalid input\n");
        return;
    }

    if (input->channels != output->channels) {
        fprintf(stderr, "Input and output should have the same numbers of channels\n");
        return;
    }

    int center = kernel_size / 2;

    // iterate through each color channel
    for (int c = 0; c < input->channels; ++c) {
        for (int y = 0; y < input->height; ++y) {
            for (int x = 0; x < input->width; ++x) {
                float sum = 0.0;

                // convolution
                for (int ky = 0; ky < kernel_size; ++ky) {
                    for (int kx = 0; kx < kernel_size; ++kx) {
                        int imgX = x - center + kx;
                        int imgY = y - center + ky;

                        // boundary handling
                        if (imgX < 0) imgX = 0;
                        if (imgX >= input->width) imgX = input->width - 1;
                        if (imgY < 0) imgY = 0;
                        if (imgX >= input->height) imgY = input->height - 1;

                        unsigned char pixelVal = input->data[(imgY * input->width + imgX) *
                                                input->channels + c];
                        sum += pixelVal * kernel[kx][ky];
                    }
                }

                sum = sum > 255 ? 255 : (sum < 0 ? 0 : sum);
                output->data[(y * output->width + x) * output->channel + c] = (unsigned char)sum;
            }
        }
    }
}


// Save image in JPEG
void save_jpeg(const char* filename, Image* img, int quality) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;
    FILE                        *outfile;
    JSAMPROW                    row_pointer[1];

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Error: could not open %s\n", filename);
        return;
    }

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = img->width;
    cinfo.image_height = img->height;
    cinfo.input_components = img->channels;
    cinfo.in_color_space = (img->channels == 3) ? JCS_RGB : JCB_GRAYSCALE;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &img->data[cinfo.next_scanline * img->widh * img->channels];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    printf("Image saved to %s\n", filename);
}


// Save image to PNG
void save_png(const char* filename, Image* img) {
    FILE *fp;
    png_structp png_ptr = NULL;
    png_infop   info_ptr = NULL;

    fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error: could not open %s\n", filename);
        return;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, img->width, img->height, 8,
                (img->channels == 3) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, 
                PNG_FILTER_TYPE_DEFAULT);
    
    png_write_info(png_ptr, info_ptr);

    for (int y = 0; y < img->height; y++) {
        png_bytep row_pointer = (png_bytep)&img->data[y * img->width * img->channels];
        png_write_rows(png_ptr, &row_pointer, 1);
    }

    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    printf("Image saved to %s\n", filename);
}



int main() {

    return 0;

}



















