#include <stdio.h>
#include <complex.h> // complex.h は fftw3.h より先に include する
#include <fftw3.h>
#include <stdlib.h>
#include <time.h>
// #include "MT.h" // This file isn't present, commenting out for now
// Will use stdlib.h rand() instead

#define _USE_MATH_DEFINES

#include <math.h>

#define lattice_number_x 128
#define lattice_number_y 128
#define t1 10000

#define dt 1
#define dx 0.01

#define Du 0.00002
#define Dv 0.00001

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
    FILE *outputfile;         // 出力ストリーム
    char filename[50]; //ファイル名の文字数
    sprintf(filename, "GrayScott-f%0.4f-k%0.4f-%02d.txt", ff, k, j);
    outputfile = fopen(filename, "w");  // ファイルを書き込み用にオープン(開く)
    if (outputfile == NULL) {          // オープンに失敗した場合
      printf("cannot open\n");         // エラーメッセージを出して
      exit(1);                         // 異常終了
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

    double k1u, k1v, k1w, k2u, k2v, k2w, k3u, k3v, k3w, k4u, k4v, k4w, u_reaction, v_reaction, w_reaction, u_diffusion, v_diffusion, w_diffusion;

    for (n1 = 0; n1 < 1; n1++){
      //ファイルに書き込む
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
          /*        printf("%g\t", (u_reaction));
          printf("%g\t", (v_reaction));
          printf("%g\t", (w_reaction));
          */    }

          // フーリエ変換実行   b[n]に計算結果が入る
          fftw_execute(plan1);
          fftw_execute(plan4);
          for( n=0; n<lattice_number_x * lattice_number_y; n++ ){
            uhat[n] = uhat[n] / Kuhat[n];
            vhat[n] = vhat[n] / Kvhat[n];
          }

          fftw_execute(plan2);
          fftw_execute(plan5);

        }

        for (y=0; y<lattice_number_y; y++){
          for (x=0; x<lattice_number_x; x++){
            fprintf(outputfile, "%g\t", (u[y + x * lattice_number_y])); // ファイルに書く
          }
          fprintf(outputfile, "\n");
        }
        fprintf(outputfile, "\n");
      }


      // b[n]の値を表示
      //int n;
      //for( n=0; n<lattice_number_x * lattice_number_y; n++ ){
      //printf("uhat_%d = %+lf %+lf*i\n", n, creal(uhat[n]), cimag(uhat[n]) );
      //  printf("vhat_%d = %+lf %+lf*i\n", n, creal(vhat[n]), cimag(vhat[n]) );
      //}


      // b[n]の値を表示
      //for( n=0; n<lattice_number_x * lattice_number_y; n++ ){
      //printf("u_%d = %+lf %+lf*i\n", n, creal(u[n]), cimag(u[n]) );
      //}

      // ここで a[m] の値を変えて再度 fftw_execute(plan1) を実行すれば、
      // b[n] が再計算される。
      fclose(outputfile);
      // 計算終了時、メモリ開放を忘れないように
      if(plan1) fftw_destroy_plan(plan1);
      if(plan2) fftw_destroy_plan(plan2);
      if(plan3) fftw_destroy_plan(plan3);
      fftw_free(u); fftw_free(uhat); fftw_free(Ku);
      if(plan4) fftw_destroy_plan(plan4);
      if(plan5) fftw_destroy_plan(plan5);
      if(plan6) fftw_destroy_plan(plan6);
      fftw_free(v); fftw_free(vhat); fftw_free(Kv);
    }

    return 0;
  }

  int main(void){
    for (ff=0.01;ff<0.06;ff+=0.001){
      for (k=0.04;k<0.07;k+=0.001){
        calc(10);
      }
    }
    return 0;
  }
