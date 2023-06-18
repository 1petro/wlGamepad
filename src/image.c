
#define _POSIX_C_SOURCE 200809L
#ifndef DEBUG
#define NDEBUG
#endif

#include <src/image.h>


int padding(int read){
        if( read % 4 != 0 ) {
        read = 4 - (read%4);
        return read;
        }
    return 0;
}


void BMPImgread(BMPImg *img,char *imgname){
   FILE *file = fopen(imgname, "rb");
   fseek(file, 0, SEEK_END);
   unsigned long fileLen=ftell(file);
   img->imgheader=malloc(54*sizeof(BMPHeader));
   img->imgdata=malloc((fileLen+1)*sizeof(char));
   rewind(file);
   if (img->imgdata == NULL){
    printf("Memory error");
    exit (2);
    }

     fread(&img->imgheader->type, 1, sizeof(img->imgheader->type), file);
     fread(&img->imgheader->size, 1, sizeof(img->imgheader->size), file);
     fread(&img->imgheader->reserved1, 1, sizeof(img->imgheader->reserved1), file);
     fread(&img->imgheader->reserved2, 1, sizeof(img->imgheader->reserved2), file);
     fread(&img->imgheader->offset, 1, sizeof(img->imgheader->offset), file);
     fread(&img->imgheader->dib_header_size, 1, sizeof(img->imgheader->dib_header_size), file);
     fread(&img->imgheader->width_px, 1, sizeof(img->imgheader->width_px), file);
     fread(&img->imgheader->height_px, 1, sizeof(img->imgheader->height_px), file);

     fseek(file, 54, SEEK_SET);
     fread(img->imgdata,1,fileLen-54,file);

     fclose(file);

     int rgb=3;
     uint32_t **pixels = malloc(rgb * sizeof(uint32_t * ));
     img->pixel=malloc(img->imgheader->height_px * img->imgheader->width_px * sizeof(uint32_t));

       int cnt=0;
       int pad=padding(3*img->imgheader->width_px);
       int row=img->imgheader->width_px;
       int bak=row;
        for(int i=0; i<=img->imgheader->height_px * img->imgheader->width_px; i++){
          for(int j=0;j<rgb; j++){
         if(i==0){pixels[j]=malloc(img->imgheader->height_px * img->imgheader->width_px * sizeof(uint32_t));}
         if(i==row){pixels[j][i]=img->imgdata[cnt+pad];
         if(j==rgb-1){cnt+=pad;row+=bak;}}
         else{pixels[j][i]=(uint32_t) img->imgdata[cnt];}
         cnt++;
         if(j==rgb-1){img->pixel[i]= pixels[j][i]<<16 | pixels[j-1][i] << 8 | pixels[j-2][i];}
   }
}

  for (int i=0; i<rgb; i++) {
    free(pixels[i]);
    }
free(pixels);
}


void print_pixels(BMPImg *img,int transparency ){//debugging only
   int size=img->imgheader->height_px * img->imgheader->width_px;
   int row = img->imgheader->width_px;
   for (int i=0;i<size;i++){
   if(i==row){printf("\n");
   row += img->imgheader->width_px;}
   int alpha = transparency << 24;
   printf("%02X \n", alpha | img->pixel[i]);
   }
   free(img->imgheader);
   free(img->imgdata);
   free(img->pixel);
}


void draw_gplayout(uint32_t * argb,BMPImg *img,int transparency ){
int cnt=0;
int alpha = transparency << 24;
for (int i=img->imgheader->height_px-1;i>=0;i--){
       for(int j=0;j<img->imgheader->width_px;j++){
         argb[cnt] = alpha | img->pixel[j+i*img->imgheader->width_px];
         cnt++;
       }
}
}

void draw_gplayoutwoffset(uint32_t * argb,BMPImg *img,int offset_x,int offset_y,int width,int transparency ){
int offset = offset_y*width + offset_x;
int alpha = transparency << 24;
if(width<img->imgheader->width_px || img->imgheader->width_px+offset>width){
fprintf(stderr,"Error invalid area size\n");
exit(EXIT_FAILURE);
}
for (int i=img->imgheader->height_px-1;i>=0;i--){
       for(int j=0;j<img->imgheader->width_px;j++){
         argb[offset] = alpha | img->pixel[j+i*img->imgheader->width_px];
         offset++;
       }
offset = offset - img->imgheader->width_px + width;
}
}
