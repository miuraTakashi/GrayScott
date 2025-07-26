# Gray-Scott反応拡散モデル - 開発記録

## プロジェクト概要
Gray-Scott反応拡散モデルのC言語実装で、GIFアニメーションとMP4動画の生成機能を持つシミュレーションプログラム。

## 開発履歴

### 初期状態
- C言語によるGray-Scott反応拡散モデルの実装
- FFTWライブラリを使用した拡散項の計算
- GIFライブラリを使用したアニメーション出力
- Runge-Kutta法による時間積分

### 主要な改良・追加機能

#### 1. PowerPoint対応のMP4出力機能追加
**問題**: PowerPointでGIFアニメーションの再生速度が1fps程度と非常に遅い

**解決策**:
- PPMフレーム出力機能を追加
- 外部FFmpegを使用したMP4動画生成
- H.264エンコーディング（20fps、YUV420P形式）
- PowerPoint最適化設定

**実装内容**:
```c
// PPMフレーム出力
void write_ppm_frame(double* u, int frame_number, double ff_val, double k_val, int run_number)

// FFmpegによるMP4生成
void create_mp4_from_frames(double ff_val, double k_val, int run_number)
```

**設定**:
- `FRAME_RATE`: 20fps
- `FRAME_NUMBER`: 64フレーム
- 出力ディレクトリ: `mp4/`

#### 2. 出力ディレクトリの整理
- GIF出力: `gif/` ディレクトリ
- MP4出力: `mp4/` ディレクトリ
- 一時PPMファイルの自動クリーンアップ

#### 3. 性能最適化

**コンパイラ最適化**:
```makefile
CFLAGS = -Wall -Wextra -O3 -march=native -ffast-math -funroll-loops
```

**アルゴリズム最適化**:
- Runge-Kutta法からEuler法への変更（高速化優先）
- 反応項の関数インライン化
- FFTWプランの最適化（FFTW_ESTIMATE → FFTW_MEASURE）
- メモリアライメント最適化（fftw_malloc使用）

**変更前**:
```c
// RK4法による時間積分
k1u = f(u[y], v[y]);
k1v = g(u[y], v[y]);
// ... 複雑な4段階計算
```

**変更後**:
```c
// Euler法による時間積分（並列化対応）
const double reaction_u = -u_val * v_squared + f_param * (1.0 - u_val);
const double reaction_v = u_val * v_squared - (f_param + k_param) * v_val;
u[y] = u_val + reaction_u * dt;
v[y] = v_val + reaction_v * dt;
```

#### 4. OpenMP並列化

**並列化レベル**:
1. **パラメータスイープレベル**: 各(f,k)組み合わせの並列実行
2. **シミュレーション内**: 反応項計算の並列化
3. **画像処理**: GIF/PPMバッファ変換の並列化

**実装例**:
```c
// パラメータスイープの並列化
#pragma omp parallel for schedule(dynamic)
for (int i = 0; i < total_combinations; i++) {
    // パラメータ計算
    calc(1, f_val, k_val);
}

// 反応項計算の並列化
#pragma omp parallel for
for (y = 0; y < total_points; y++) {
    // 反応項計算
}
```

**OpenMP設定**:
- スレッド数: 10（M2 MacBookの10コア）
- スケジューリング: dynamic（負荷分散）
- スレッドセーフな進捗表示

#### 5. 進捗表示機能

**機能**:
- パラメータスイープの進捗をプログレスバーで表示
- 完了数/総数の表示
- パーセンテージ表示
- スレッドセーフな更新

**実装**:
```c
void print_progress(int current, int total, const char* task_name) {
    #pragma omp critical
    {
        // プログレスバー表示
    }
}
```

#### 6. ファイル管理の改善

**.gitignore追加**:
```
# Build artifacts
*.o
*.out
a.out
gray_scott
main

# System files
.DS_Store

# Output directories
gif/
mp4/
```

### 技術仕様

#### 依存ライブラリ
- **FFTW3**: 高速フーリエ変換
- **GifLib**: GIFアニメーション生成
- **OpenMP**: 並列処理
- **FFmpeg**: MP4動画生成（外部コマンド）

#### パラメータ設定
- **格子サイズ**: 128×128
- **時間ステップ**: dt = 1
- **空間ステップ**: dx = 0.01
- **総時間**: 10000
- **拡散係数**: Du = 0.00002, Dv = 0.00001

#### パラメータスイープ範囲
- **フィード率 f**: 0.01 ～ 0.06（ステップ 0.001）
- **キル率 k**: 0.04 ～ 0.07（ステップ 0.001）
- **総組み合わせ**: 1,500通り

### 性能改善結果

#### コンパイル最適化
- `-O3 -march=native -ffast-math`: CPU最適化
- `-funroll-loops`: ループ展開
- アライメント済みメモリ使用

#### 並列化効果
- **理論値**: 10コアで最大10倍高速化
- **実測予想**: 5-8倍高速化（I/Oボトルネック考慮）

#### メモリ使用量
- 基本配列: 約1MB（128×128×8バイト×8配列）
- FFTW最適化メモリ使用

### ビルド・実行方法

#### 必要な環境
```bash
# macOS (Homebrew)
brew install fftw giflib libomp ffmpeg
```

#### コンパイル
```bash
make clean && make
```

#### 実行
```bash
./gray_scott
```

#### 出力ファイル
- **GIF**: `gif/GrayScott-f{f}-k{k}-00.gif`
- **MP4**: `mp4/GrayScott-f{f}-k{k}-00.mp4`

### 今後の改善案

#### 可能な最適化
1. **GPU並列化**: CUDA/OpenCLによるGPU計算
2. **メモリ最適化**: キャッシュ効率の改善
3. **I/O最適化**: 非同期ファイル書き込み
4. **分散処理**: MPI対応

#### 機能拡張
1. **パラメータ設定ファイル**: JSON/YAML設定
2. **リアルタイム可視化**: OpenGL表示
3. **統計解析**: パターン分類・解析機能
4. **Webインターフェース**: ブラウザ操作

### 開発環境
- **OS**: macOS (Apple Silicon M2)
- **コンパイラ**: GCC with OpenMP
- **IDE**: Cursor
- **バージョン管理**: Git/GitHub

### 参考文献・技術資料
- Gray, P. & Scott, S.K. (1983). Autocatalytic reactions in the isothermal, continuous stirred tank reactor
- FFTW Documentation: https://www.fftw.org/
- OpenMP Specification: https://www.openmp.org/
- FFmpeg Documentation: https://ffmpeg.org/

---
*最終更新: 2024年12月* 