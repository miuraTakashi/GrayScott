#include <stdio.h>
#include <complex.h> // complex.h は fftw3.h より先に include する
#include "/opt/homebrew/opt/fftw/include/fftw3.h"
#include <stdlib.h>
#include <time.h>
// #include "MT.h" // This file isn't present, commenting out for now
// Will use stdlib.h rand() instead
#include "/opt/homebrew/opt/giflib/include/gif_lib.h" // Include the GIF library with absolute path
#include <omp.h> // OpenMP for parallelization

#define _USE_MATH_DEFINES

#include <math.h>

// Define TRUE/FALSE if not already defined by gif_lib.h
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define totalTime 10000
#define Du 0.00002
#define Dv 0.00001


// Simulation parameters

#define lattice_number_x 128
#define lattice_number_y 128

#define dt 1
#define dx 0.01

#define totalLoop (int)(totalTime/dt)


//Visualization parameters

#define FRAME_DELAY 30 // Delay between frames in 1/100ths of a second (0.02 seconds) - Try very short delay for PowerPoint
#define FRAME_NUMBER 128 
#define EXPORT_INTERVAL (int)(totalLoop/FRAME_NUMBER)

// MP4 output parameters
#define FRAME_RATE 20 // Frames per second for MP4 output
#define OUTPUT_MP4 1  // Set to 1 to enable MP4 output, 0 for GIF only 


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

// Function to write a PPM frame for MP4 conversion
void write_ppm_frame(double* u, int frame_number, double ff_val, double k_val, int run_number) {
  char filename[100];
  sprintf(filename, "mp4/frames_f%0.4f_k%0.4f_%02d/frame_%04d.ppm", ff_val, k_val, run_number, frame_number);
  
  FILE* fp = fopen(filename, "wb");
  if (!fp) {
    printf("Failed to create PPM file: %s\n", filename);
    return;
  }
  
  // Write PPM header
  fprintf(fp, "P6\n%d %d\n255\n", lattice_number_x, lattice_number_y);
  
  // Write pixel data (create buffer first for parallel processing)
  unsigned char* pixel_buffer = (unsigned char*)malloc(lattice_number_x * lattice_number_y * 3);
  
  #pragma omp parallel for collapse(2)
  for (int y = 0; y < lattice_number_y; y++) {
    for (int x = 0; x < lattice_number_x; x++) {
      double value = u[x * lattice_number_y + y];
      // Clamp value between 0 and 1
      if (value < 0) value = 0;
      if (value > 1) value = 1;
      
      // Convert to RGB (grayscale)
      unsigned char intensity = (unsigned char)(value * 255);
      int idx = (y * lattice_number_x + x) * 3;
      pixel_buffer[idx] = intensity;     // Red
      pixel_buffer[idx + 1] = intensity; // Green  
      pixel_buffer[idx + 2] = intensity; // Blue
    }
  }
  
  // Write buffer to file
  fwrite(pixel_buffer, 1, lattice_number_x * lattice_number_y * 3, fp);
  free(pixel_buffer);
  
  fclose(fp);
}

// Function to create MP4 from PPM frames using system FFmpeg
void create_mp4_from_frames(double ff_val, double k_val, int run_number) {
  char frames_dir[100];
  char output_file[100];
  char ffmpeg_cmd[300];
  
  sprintf(frames_dir, "mp4/frames_f%0.4f_k%0.4f_%02d", ff_val, k_val, run_number);
  sprintf(output_file, "mp4/GrayScott-f%0.4f-k%0.4f-%02d.mp4", ff_val, k_val, run_number);
  
  // Create FFmpeg command
  sprintf(ffmpeg_cmd, 
    "ffmpeg -y -framerate %d -i %s/frame_%%04d.ppm -c:v libx264 -pix_fmt yuv420p -crf 18 %s 2>/dev/null",
    FRAME_RATE, frames_dir, output_file);
  
  int result = system(ffmpeg_cmd);
  
  if (result == 0) {
    // Clean up PPM frames
    char cleanup_cmd[150];
    sprintf(cleanup_cmd, "rm -rf %s", frames_dir);
    system(cleanup_cmd);
  } else {
    printf("    ✗ Failed to create MP4 for f=%.4f, k=%.4f\n", ff_val, k_val);
  }
}

double ff;
double k;

// Thread-safe progress display function
void print_progress(int current, int total, const char* task_name) {
  #pragma omp critical
  {
    int progress = (int)((double)current / total * 100);
    int bar_length = 50;
    int filled_length = (int)((double)current / total * bar_length);
    
    printf("\r%s: [", task_name);
    for (int i = 0; i < bar_length; i++) {
      if (i < filled_length) {
        printf("=");
      } else if (i == filled_length && current < total) {
        printf(">");
      } else {
        printf(" ");
      }
    }
    printf("] %d%% (%d/%d)", progress, current, total);
    fflush(stdout);
    
    if (current == total) {
      printf("\n");
    }
  }
}


double f(double u, double v){
  return -u*v*v + ff*(1-u) ;
}

double g(double u, double v){
  return u*v*v-(ff+k)*v;
}


int calc(int s, double f_param, double k_param){
  int n1;
  int m;
  int y;
  int n;
  int x;
  int j;
  int jstart = 0;

  for(j=jstart;j<s;j++) {
    // Create frames directory for MP4 output if needed
    if (OUTPUT_MP4) {
      char frames_dir[100];
      sprintf(frames_dir, "mp4/frames_f%0.4f_k%0.4f_%02d", f_param, k_param, j);
      char mkdir_cmd[150];
      sprintf(mkdir_cmd, "mkdir -p %s", frames_dir);
      system(mkdir_cmd);
    }
    GifFileType* gif_file;
    ColorMapObject* colormap;
    char filename[50]; // Filename buffer
    
    // Create GIF filename with "gif/" prefix
    sprintf(filename, "gif/GrayScott-f%0.4f-k%0.4f-%02d.gif", f_param, k_param, j);
    
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
    
    // Add application extension for looping animation
    unsigned char loop_count[] = {1, 0, 0}; // Loop indefinitely
    if (EGifPutExtensionLeader(gif_file, APPLICATION_EXT_FUNC_CODE) == GIF_ERROR ||
        EGifPutExtensionBlock(gif_file, 11, "NETSCAPE2.0") == GIF_ERROR ||
        EGifPutExtensionBlock(gif_file, 3, loop_count) == GIF_ERROR ||
        EGifPutExtensionTrailer(gif_file) == GIF_ERROR) {
      printf("Error writing loop extension\n");
    }
    
    // Use FFTW aligned memory allocation for better performance
    // a,b は double _Complex 型のC99標準複素配列と実質的に同じ
    // double _Complex a[4] としても動くけど計算速度が低下する可能性あり
    double *u, *Ku, *v, *Kv;
    fftw_complex *uhat, *Kuhat, *vhat, *Kvhat;
    const int array_size = lattice_number_x * lattice_number_y;
    
    u = (double*) fftw_malloc(sizeof(double) * array_size);
    uhat = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * array_size);
    Ku = (double*) fftw_malloc(sizeof(double) * array_size);
    Kuhat = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * array_size);

    v = (double*) fftw_malloc(sizeof(double) * array_size);
    vhat = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * array_size);
    Kv = (double*) fftw_malloc(sizeof(double) * array_size);
    Kvhat = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * array_size);

    // Allocate GIF buffer for frames
    GifByteType* gif_buffer = (GifByteType*)malloc(lattice_number_x * lattice_number_y * sizeof(GifByteType));
    if (gif_buffer == NULL) {
      printf("Failed to allocate GIF buffer\n");
      EGifCloseFile(gif_file, NULL);
      GifFreeMapObject(colormap);
      exit(1);
    }

    // プランの生成 - MEASURE for better performance (takes longer to create but faster execution)
    // フーリエ逆変換つまり位相因子を exp(-k)ではなく exp(+k)とする場合は
    // FFTW_FORWARD の代わりに FFTW_BACKWARD とする
    fftw_plan plan1, plan2, plan3, plan4, plan5, plan6;
    plan1 = fftw_plan_dft_r2c_2d( lattice_number_x, lattice_number_y, u, uhat, FFTW_MEASURE );
    plan2 = fftw_plan_dft_c2r_2d( lattice_number_x, lattice_number_y, uhat, u, FFTW_MEASURE );
    plan3 = fftw_plan_dft_r2c_2d( lattice_number_x, lattice_number_y, Ku, Kuhat, FFTW_MEASURE );
    plan4 = fftw_plan_dft_r2c_2d( lattice_number_x, lattice_number_y, v, vhat, FFTW_MEASURE );
    plan5 = fftw_plan_dft_c2r_2d( lattice_number_x, lattice_number_y, vhat, v, FFTW_MEASURE );
    plan6 = fftw_plan_dft_r2c_2d( lattice_number_x, lattice_number_y, Kv, Kvhat, FFTW_MEASURE );

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

    // Removed unused RK4 variables since we switched to Euler method

    for (n1 = 0; n1 < 1; n1++){
      // Main simulation loop
      for (m = 0; m < totalLoop; m++){
        // Optimized reaction computation with OpenMP parallelization
        const int total_points = lattice_number_x * lattice_number_y;
        
        // Parallel Euler method for reaction terms
        #pragma omp parallel for
        for (y = 0; y < total_points; y++) {
          const double u_val = u[y];
          const double v_val = v[y];
          const double v_squared = v_val * v_val;
          
          // Inline the reaction functions for better performance
          const double reaction_u = -u_val * v_squared + f_param * (1.0 - u_val);
          const double reaction_v = u_val * v_squared - (f_param + k_param) * v_val;
          
          u[y] = u_val + reaction_u * dt;
          v[y] = v_val + reaction_v * dt;
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
          int frame_index = m / EXPORT_INTERVAL;
          
          // Export PPM frame for MP4 if enabled
          if (OUTPUT_MP4) {
            write_ppm_frame(u, frame_index, f_param, k_param, j);
          }
          
          // Set up graphic control extension for frame delay
          GraphicsControlBlock gcb;
          gcb.DisposalMode = DISPOSE_BACKGROUND; // Changed for PowerPoint compatibility
          gcb.UserInputFlag = FALSE;
          gcb.DelayTime = FRAME_DELAY; // 2 = 0.02 seconds
          gcb.TransparentColor = NO_TRANSPARENT_COLOR;
          
          // Add the extension with the current frame index
          EGifGCBToSavedExtension(&gcb, gif_file, frame_index);
          
          // Convert data to GIF color indices (parallelized)
          #pragma omp parallel for collapse(2)
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
          
          // Frame export (no progress display)
        }
      }
    }

    // Close the GIF file
    if (EGifCloseFile(gif_file, NULL) == GIF_ERROR) {
      printf("Error closing GIF file\n");
    }
    
    // Create MP4 from frames if enabled
    if (OUTPUT_MP4) {
      create_mp4_from_frames(f_param, k_param, j);
    }
    
    // Free allocated memory
    free(gif_buffer);
    
    // 計算終了時、メモリ開放を忘れないように
    if(plan1) fftw_destroy_plan(plan1);
    if(plan2) fftw_destroy_plan(plan2);
    if(plan3) fftw_destroy_plan(plan3);
    if(plan4) fftw_destroy_plan(plan4);
    if(plan5) fftw_destroy_plan(plan5);
    if(plan6) fftw_destroy_plan(plan6);
    
    fftw_free(u); fftw_free(uhat); fftw_free(Ku); fftw_free(Kuhat);
    fftw_free(v); fftw_free(vhat); fftw_free(Kv); fftw_free(Kvhat);
    
    GifFreeMapObject(colormap);
  }

  return 0;
}

int main(void){
  // Create output directories if they don't exist
  #ifdef _WIN32
    system("if not exist gif mkdir gif");
    system("if not exist mp4 mkdir mp4");
  #else
    system("mkdir -p gif");
    system("mkdir -p mp4");
  #endif
  
  // Reduce the parameter space to make testing faster
  // Only use a few interesting parameter combinations
  /*
  ff = 0.025;
  k = 0.055;
  calc(1); // Just generate 1 GIF for now
  */
  
  // Parallel parameter sweep
  printf("Gray-Scott parameter sweep: f=[0.005-0.07], k=[0.04-0.07], step=0.001\n");
  printf("OpenMP threads: %d\n", omp_get_max_threads());
  
  // Define parameter ranges
  const double f_start = 0.005, f_end = 0.07, f_step = 0.001;
  const double k_start = 0.04, k_end = 0.07, k_step = 0.001;
  
  const int f_count = (int)((f_end - f_start) / f_step + 0.5);
  const int k_count = (int)((k_end - k_start) / k_step + 0.5);
  const int total_combinations = f_count * k_count;
  
  printf("Total combinations: %d\n", total_combinations);
  
  int completed_combinations = 0;
  
  // Parallel parameter sweep
  #pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < total_combinations; i++) {
    // Convert linear index to f,k parameters
    int f_idx = i / k_count;
    int k_idx = i % k_count;
    
    double f_val = f_start + f_idx * f_step;
    double k_val = k_start + k_idx * k_step;
    
    calc(1, f_val, k_val);
    
    // Update progress (thread-safe)
    #pragma omp atomic
    completed_combinations++;
    
    // Show progress every 10 completed simulations
    if (completed_combinations % 10 == 0 || completed_combinations == total_combinations) {
      print_progress(completed_combinations, total_combinations, "Progress");
    }
  }
  
  printf("\nCompleted: %d simulations\n", total_combinations);

  
  return 0;
}
