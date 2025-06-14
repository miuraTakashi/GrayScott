#include <stdio.h>
#include <complex.h> // complex.h は fftw3.h より先に include する
#include "/opt/homebrew/opt/fftw/include/fftw3.h"
#include <stdlib.h>
#include <time.h>
// #include "MT.h" // This file isn't present, commenting out for now
// Will use stdlib.h rand() instead
#include "/opt/homebrew/opt/giflib/include/gif_lib.h" // Include the GIF library with absolute path

#define _USE_MATH_DEFINES

#include <math.h>

// Define TRUE/FALSE if not already defined by gif_lib.h
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define lattice_number_x 128
#define lattice_number_y 128
#define t1 5120  // Reduce from 10000 to 500 iterations
#define FRAME_DELAY 5 // Delay between frames in 1/100ths of a second
#define EXPORT_INTERVAL 40 // Export every 10 iterations instead of 50

#define dt 1
#define dx 0.01

#define Du 0.00002
#define Dv 0.00001

// Color map for visualizing the Gray-Scott model
#define COLOR_DEPTH 256
ColorMapObject* create_colormap() {
  ColorMapObject* colormap = GifMakeMapObject(COLOR_DEPTH, NULL);
  if (colormap == NULL) {
    return NULL;
  }
  
  // Create a simple grayscale color map
  for (int i = 0; i < COLOR_DEPTH; i++) {
    colormap->Colors[i].Red = i;
    colormap->Colors[i].Green = i;
    colormap->Colors[i].Blue = i;
  }
  
  return colormap;
}

double ff;
double k;


double f(double u, double v){
  return -u*v*v + ff*(1-u) ;
}

double g(double u, double v){
  return u*v*v-(ff+k)*v;
}


int calc(int s){
  int n1;
  int m;
  int y;
  int n;
  int x;
  int j;
  int jstart = 0;

  for(j=jstart;j<s;j++) {
    GifFileType* gif_file;
    ColorMapObject* colormap;
    char filename[50]; // Filename buffer
    
    // Create GIF filename with "gif/" prefix
    sprintf(filename, "gif/GrayScott-f%0.4f-k%0.4f-%02d.gif", ff, k, j);
    
    // Create colormap
    colormap = create_colormap();
    if (colormap == NULL) {
      printf("Failed to create colormap\n");
      exit(1);
    }
    
    // Create GIF file
    gif_file = EGifOpenFileName(filename, 0, NULL);
    if (gif_file == NULL) {
      printf("Cannot open GIF file: %s\n", filename);
      GifFreeMapObject(colormap);
      exit(1);
    }
    
    // Set GIF parameters
    if (EGifPutScreenDesc(gif_file, lattice_number_x, lattice_number_y, 8, 0, colormap) == GIF_ERROR) {
      printf("Error writing GIF screen descriptor\n");
      EGifCloseFile(gif_file, NULL);
      GifFreeMapObject(colormap);
      exit(1);
    }
    
    // a,b は double _Complex 型のC99標準複素配列と実質的に同じ
    // double _Complex a[4] としても動くけど計算速度が低下する可能性あり
    double *u, *Ku, *v, *Kv;
    fftw_complex *uhat, *Kuhat, *vhat, *Kvhat;
    u = (double*) malloc(sizeof(double) * lattice_number_x * lattice_number_y);
    uhat = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * lattice_number_x * lattice_number_y);
    Ku = (double*) malloc(sizeof(double) * lattice_number_x * lattice_number_y);
    Kuhat = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * lattice_number_x * lattice_number_y);

    v = (double*) malloc(sizeof(double) * lattice_number_x * lattice_number_y);
    vhat = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * lattice_number_x * lattice_number_y);
    Kv = (double*) malloc(sizeof(double) * lattice_number_x * lattice_number_y);
    Kvhat = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * lattice_number_x * lattice_number_y);

    // Allocate GIF buffer for frames
    GifByteType* gif_buffer = (GifByteType*)malloc(lattice_number_x * lattice_number_y * sizeof(GifByteType));
    if (gif_buffer == NULL) {
      printf("Failed to allocate GIF buffer\n");
      EGifCloseFile(gif_file, NULL);
      GifFreeMapObject(colormap);
      exit(1);
    }

    // プランの生成
    // フーリエ逆変換つまり位相因子を exp(-k)ではなく exp(+k)とする場合は
    // FFTW_FORWARD の代わりに FFTW_BACKWARD とする
    fftw_plan plan1, plan2, plan3, plan4, plan5, plan6;
    plan1 = fftw_plan_dft_r2c_2d( lattice_number_x, lattice_number_y, u, uhat, FFTW_ESTIMATE );
    plan2 = fftw_plan_dft_c2r_2d( lattice_number_x, lattice_number_y, uhat, u, FFTW_ESTIMATE );
    plan3 = fftw_plan_dft_r2c_2d( lattice_number_x, lattice_number_y, Ku, Kuhat, FFTW_ESTIMATE );
    plan4 = fftw_plan_dft_r2c_2d( lattice_number_x, lattice_number_y, v, vhat, FFTW_ESTIMATE );
    plan5 = fftw_plan_dft_c2r_2d( lattice_number_x, lattice_number_y, vhat, v, FFTW_ESTIMATE );
    plan6 = fftw_plan_dft_r2c_2d( lattice_number_x, lattice_number_y, Kv, Kvhat, FFTW_ESTIMATE );

    // フーリエ変換前の数列値を設定
    srand((unsigned)time(NULL));
    for (n1 = 0; n1 < lattice_number_x * lattice_number_y; n1++){
      u[n1] = 1+0.2 * rand()/RAND_MAX -0.1;
      Ku[n1] = 0;
      v[n1] = 0.2 * rand()/RAND_MAX -0.1;
      Kv[n1] = 0;
    }

    for(x=lattice_number_x/2-10;x<lattice_number_x/2+10;x++){
      for(y=lattice_number_y/2-10;y<lattice_number_y/2+10;y++){
        u[x*lattice_number_y+y]=0.5+0.2 * rand()/RAND_MAX -0.1;
        v[x*lattice_number_y+y]=0.25+0.2 * rand()/RAND_MAX -0.1;
      }
    }

    Ku[0] = (1 + 4* Du *dt / dx /dx)*lattice_number_x*lattice_number_y;
    Ku[1] = - Du *dt / dx /dx*lattice_number_x*lattice_number_y;
    Ku[lattice_number_y - 1] = - Du *dt / dx /dx*lattice_number_x*lattice_number_y;
    Ku[lattice_number_y] = - Du *dt / dx /dx*lattice_number_x*lattice_number_y;
    Ku[lattice_number_y * (lattice_number_x - 1)] = - Du *dt / dx /dx*lattice_number_x*lattice_number_y;


    Kv[0] = (1 + 4* Dv *dt / dx /dx)*lattice_number_x*lattice_number_y;
    Kv[1] = - Dv *dt / dx /dx*lattice_number_x*lattice_number_y;
    Kv[lattice_number_y - 1] = - Dv *dt / dx /dx*lattice_number_x*lattice_number_y;
    Kv[lattice_number_y] = - Dv *dt / dx /dx*lattice_number_x*lattice_number_y;
    Kv[lattice_number_y * (lattice_number_x - 1)] = - Dv *dt / dx /dx*lattice_number_x*lattice_number_y;

    fftw_execute(plan3);
    fftw_execute(plan6);

    double k1u, k1v, k2u, k2v, k3u, k3v, k4u, k4v, u_reaction, v_reaction;

    for (n1 = 0; n1 < 1; n1++){
      // Main simulation loop
      for (m = 0; m < t1; m++){
        //計算をそのまま回す
        for (y=0; y<lattice_number_x * lattice_number_y; y++){
          //  y=lattice_number_x * lattice_number_y/2;

          k1u = f(u[y], v[y]);
          k1v = g(u[y], v[y]);

          k2u = f(u[y] + k1u * dt / 2, v[y] + k1v * dt / 2);
          k2v = g(u[y] + k1u * dt / 2, v[y] + k1v * dt / 2);

          k3u = f(u[y] + k2u * dt / 2, v[y] + k2v * dt / 2);
          k3v = g(u[y] + k2u * dt / 2, v[y] + k2v * dt / 2);

          k4u = f(u[y] + k3u * dt, v[y] + k3v * dt);
          k4v = g(u[y] + k3u * dt, v[y] + k3v * dt);

          u_reaction = u[y] + (k1u + 2 * k2u + 2 * k3u + k4u) / 6 * dt;
          v_reaction = v[y] + (k1v + 2 * k2v + 2 * k3v + k4v) / 6 * dt;

          u[y] = u_reaction;
          v[y] = v_reaction;
        }

        // フーリエ変換実行   b[n]に計算結果が入る
        fftw_execute(plan1);
        fftw_execute(plan4);
        for( n=0; n<lattice_number_x * lattice_number_y; n++ ){
          uhat[n] = uhat[n] / Kuhat[n];
          vhat[n] = vhat[n] / Kvhat[n];
        }

        fftw_execute(plan2);
        fftw_execute(plan5);
        
        // Export GIF frame every EXPORT_INTERVAL iterations
        if (m % EXPORT_INTERVAL == 0) {
          // Set up graphic control extension for frame delay
          GraphicsControlBlock gcb;
          gcb.DisposalMode = DISPOSE_DO_NOT;
          gcb.UserInputFlag = FALSE;
          gcb.DelayTime = FRAME_DELAY;
          gcb.TransparentColor = NO_TRANSPARENT_COLOR;
          
          // Add the extension with the current frame index
          int imageIndex = m / EXPORT_INTERVAL;
          EGifGCBToSavedExtension(&gcb, gif_file, imageIndex);
          
          // Convert data to GIF color indices
          for (y = 0; y < lattice_number_y; y++) {
            for (x = 0; x < lattice_number_x; x++) {
              // Map u values to the range 0-255
              double value = u[x * lattice_number_y + y];
              // Clamp value between 0 and 1
              if (value < 0) value = 0;
              if (value > 1) value = 1;
              
              // Convert to color index
              int index = (int)(value * (COLOR_DEPTH - 1));
              gif_buffer[y * lattice_number_x + x] = index;
            }
          }
          
          // Add frame to GIF
          if (EGifPutImageDesc(gif_file, 0, 0, lattice_number_x, lattice_number_y, FALSE, NULL) == GIF_ERROR) {
            printf("Error writing GIF image descriptor\n");
            break;
          }
          
          // Write the image data
          for (y = 0; y < lattice_number_y; y++) {
            if (EGifPutLine(gif_file, &gif_buffer[y * lattice_number_x], lattice_number_x) == GIF_ERROR) {
              printf("Error writing GIF data\n");
              break;
            }
          }
          
          printf("Added frame at iteration %d\n", m);
        }
      }
    }

    // Close the GIF file
    if (EGifCloseFile(gif_file, NULL) == GIF_ERROR) {
      printf("Error closing GIF file\n");
    }
    
    // Free allocated memory
    free(gif_buffer);
    
    // 計算終了時、メモリ開放を忘れないように
    if(plan1) fftw_destroy_plan(plan1);
    if(plan2) fftw_destroy_plan(plan2);
    if(plan3) fftw_destroy_plan(plan3);
    fftw_free(u); fftw_free(uhat); fftw_free(Ku); fftw_free(Kuhat);
    if(plan4) fftw_destroy_plan(plan4);
    if(plan5) fftw_destroy_plan(plan5);
    if(plan6) fftw_destroy_plan(plan6);
    fftw_free(v); fftw_free(vhat); fftw_free(Kv); fftw_free(Kvhat);
    
    GifFreeMapObject(colormap);
  }

  return 0;
}

int main(void){
  // Create gif directory if it doesn't exist
  #ifdef _WIN32
    system("if not exist gif mkdir gif");
  #else
    system("mkdir -p gif");
  #endif
  
  // Reduce the parameter space to make testing faster
  // Only use a few interesting parameter combinations
  /*
  ff = 0.025;
  k = 0.055;
  calc(1); // Just generate 1 GIF for now
  */
  
  // Original parameter sweep
  for (ff=0.01;ff<0.06;ff+=0.001){
    for (k=0.04;k<0.07;k+=0.001){
      calc(1);
    }
  }

  
  return 0;
}
