#include <stdlib.h>
#include <stdio.h>
#include <src/image.h>
#include <stdint.h>

void read_gp_img(gpimg *img,char *filename) {
  FILE *fp = fopen(filename, "rb");
  if(!fp){
  fprintf(stderr,"Error opening img file\n");
  abort();
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png){
  fprintf(stderr,"Error reading img\n");
  abort();
  }

  png_infop info = png_create_info_struct(png);
  if(!info){
  fprintf(stderr,"Error reading img info\n");
  abort();
  }

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  img->width      = png_get_image_width(png, info);
  img->height     = png_get_image_height(png, info);
  img->color_type = png_get_color_type(png, info);
  img->bit_depth  = png_get_bit_depth(png, info);

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(img->bit_depth == 16)
    png_set_strip_16(png);

  if(img->color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(img->color_type == PNG_COLOR_TYPE_GRAY && img->bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  if(img->color_type == PNG_COLOR_TYPE_RGB ||
     img->color_type == PNG_COLOR_TYPE_GRAY ||
     img->color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(img->color_type == PNG_COLOR_TYPE_GRAY ||
     img->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  if (img->row_pointers){
      img->row_pointers= NULL;
  }

  img->row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * img->height);
  for(int y = 0; y < img->height; y++) {
    img->row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, img->row_pointers);

  fclose(fp);
  png_destroy_read_struct(&png, &info, NULL);
}

void draw_gplayoutwoffset(uint32_t * argb_w,gpimg *img,int offset_x,int offset_y,int width,int scale){
int offset = offset_y*width + offset_x;

int new_height = img->height * scale;
int new_width = img->width * scale;

for (int i=0;i<new_height;i++){
       png_bytep row = img->row_pointers[i/scale];
       for(int j=0;j<new_width;j++){
           png_bytep px = &(row[j/scale * 4]);
         argb_w[offset] =  px[3]<<24 | px[0]<<16 | px[1]<<8 | px[2];
         offset++;
       }
offset = offset - img->width + width;
}
for(int y = 0; y < img->height; y++) {
    free(img->row_pointers[y]);
  }
  free(img->row_pointers);
}

void draw_gplayout(uint32_t * argb_w,gpimg *img,int *width,int *height,int scale){
int cnt=0;

int new_height = img->height * scale;
int new_width = img->width * scale;
// *size = new_width;

*width=new_width;
*height=new_height;

if(new_width>UINT8_MAX || new_width>UINT8_MAX){
fprintf(stderr,"Error invalid image size shouldn't be larger than %d\n",UINT8_MAX);
abort();
}

for (int i=0;i<new_height;i++){
       png_bytep row = img->row_pointers[i/scale];
       for(int j=0;j<new_width;j++){
         png_bytep px = &(row[j/scale * 4]);

         argb_w[cnt] = px[3]<<24 | px[0]<<16 | px[1]<<8 | px[2];
//         printf("%2X  %d  %d  %d\n",argb_w[cnt],cnt,i,j);
         cnt++;
       }
}

 for(int y = 0; y < img->height; y++) {
    free(img->row_pointers[y]);
  }
  free(img->row_pointers);
}
