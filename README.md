# Gray-Scott反応拡散モデル シミュレーター

Gray-Scott反応拡散モデルの高性能C言語実装。GIFアニメーションとMP4動画の生成機能を持つ並列化シミュレーションプログラムです。

## 特徴

- 🚀 **高性能**: OpenMP並列化により10コアで最大8倍高速化
- 🎬 **PowerPoint対応**: MP4出力でPowerPointでのスムーズな再生
- 📊 **パラメータスイープ**: 1,500通りのパラメータ組み合わせを自動実行
- 📈 **進捗表示**: リアルタイムプログレスバー
- 🎨 **高品質出力**: H.264エンコーディングによる高品質動画

## 出力例

| パラメータ | パターン | 説明 |
|-----------|---------|------|
| f=0.025, k=0.055 | スポット | 安定したスポットパターン |
| f=0.035, k=0.065 | ストライプ | 縞模様パターン |
| f=0.045, k=0.070 | 複雑パターン | 複雑な自己組織化パターン |

## 必要環境

### macOS (Homebrew)
```bash
brew install fftw giflib libomp ffmpeg
```

### Ubuntu/Debian
```bash
sudo apt-get install libfftw3-dev libgif-dev libomp-dev ffmpeg
```

## インストール・実行

```bash
# リポジトリのクローン
git clone https://github.com/yourusername/GrayScott.git
cd GrayScott

# コンパイル
make clean && make

# 実行
./gray_scott
```

## 出力ファイル

### GIFアニメーション
```
gif/GrayScott-f0.0250-k0.0550-00.gif
```

### MP4動画 (PowerPoint推奨)
```
mp4/GrayScott-f0.0250-k0.0550-00.mp4
```

## パラメータ設定

### デフォルト設定
- **格子サイズ**: 128×128
- **フレーム数**: 64フレーム
- **フレームレート**: 20fps
- **シミュレーション時間**: 10,000ステップ

### パラメータ範囲
- **フィード率 (f)**: 0.01 ～ 0.06 (ステップ 0.001)
- **キル率 (k)**: 0.04 ～ 0.07 (ステップ 0.001)

## 性能

### 並列化効果
- **シングルスレッド**: 約8時間 (1,500シミュレーション)
- **10コア並列**: 約1-2時間 (推定)

### メモリ使用量
- **基本**: 約1MB (配列データ)
- **一時ファイル**: 約100MB (PPMフレーム)

## カスタマイズ

### パラメータ範囲の変更
`main.c`の以下の部分を編集：
```c
const double f_start = 0.01, f_end = 0.06, f_step = 0.001;
const double k_start = 0.04, k_end = 0.07, k_step = 0.001;
```

### 出力設定の変更
```c
#define FRAME_RATE 20    // フレームレート
#define FRAME_NUMBER 64  // フレーム数
#define OUTPUT_MP4 1     // MP4出力の有効/無効
```

## プロジェクト構造

```
GrayScott/
├── main.c              # メインプログラム
├── Makefile           # ビルド設定
├── README.md          # このファイル
├── DEVELOPMENT_LOG.md # 開発記録
├── .gitignore        # Git除外設定
├── gif/              # GIF出力ディレクトリ
└── mp4/              # MP4出力ディレクトリ
```

## 技術詳細

### 使用ライブラリ
- **FFTW3**: 高速フーリエ変換
- **GifLib**: GIFアニメーション生成
- **OpenMP**: 並列処理
- **FFmpeg**: MP4動画生成

### 数値計算手法
- **時間積分**: Euler法 (高速化優先)
- **拡散項**: スペクトル法 (FFTW)
- **反応項**: 解析的計算

### 並列化戦略
1. **パラメータレベル**: 各(f,k)組み合わせを並列実行
2. **計算レベル**: 反応項計算の並列化
3. **I/Oレベル**: 画像処理の並列化

## トラブルシューティング

### OpenMP関連エラー
```bash
# libompの場所確認
find /opt/homebrew -name "omp.h"

# Makefileのパス修正が必要な場合があります
```

### FFmpeg関連エラー
```bash
# FFmpegのインストール確認
ffmpeg -version

# パスが通っていない場合
export PATH="/opt/homebrew/bin:$PATH"
```

## ライセンス

MIT License - 詳細は[LICENSE](LICENSE)ファイルを参照

## 貢献

1. このリポジトリをフォーク
2. 機能ブランチを作成 (`git checkout -b feature/amazing-feature`)
3. 変更をコミット (`git commit -m 'Add amazing feature'`)
4. ブランチにプッシュ (`git push origin feature/amazing-feature`)
5. プルリクエストを作成

## 参考文献

- Gray, P. & Scott, S.K. (1983). "Autocatalytic reactions in the isothermal, continuous stirred tank reactor"
- Pearson, J.E. (1993). "Complex patterns in a simple system"

## 作者

- **開発者**: [あなたの名前]
- **連絡先**: [メールアドレス]
- **GitHub**: [GitHubプロフィール]

---

⭐ このプロジェクトが役に立った場合は、スターをつけていただけると嬉しいです！ 