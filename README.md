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

## 可視化ツール

### インタラクティブf-kマップ

Jupyter Notebookで全パラメータ空間の可視化が可能です：

```bash
# 必要なPythonライブラリのインストール
pip install --break-system-packages ipywidgets ipympl pillow numpy matplotlib

# Jupyter Labの起動
jupyter lab fk_click_show_gif.ipynb
```

**機能:**
- f-kパラメータ空間の全体像を空間変化率でカラーマップ表示
- マップ上の点をクリックすると対応するGIFアニメーションを表示
- 初回実行時は全GIFファイルを解析（数分）、以降はキャッシュから高速読み込み
- 理論的な分岐曲線（Hopf分岐、数値解境界）も表示

**ライブラリ構成:**
- `fk_analysis.py`: GIF解析とデータ処理ライブラリ
- `fk_click_show_gif.ipynb`: インタラクティブ可視化ノートブック

## 出力ファイル

### GIFアニメーション
```
gif/GrayScott-f0.0250-k0.0550-00.gif
```

### MP4動画 (PowerPoint推奨)
```
mp4/GrayScott-f0.0250-k0.0550-00.mp4
```

## GIFフォルダの同期について

gifフォルダは約7.3GB、15,000個以上のファイルを含むため、Gitで直接管理するのは適していません。
現在、gifフォルダは`.gitignore`で無視されています。

### 同期方法

詳細は [`sync_gif_folder.md`](sync_gif_folder.md) を参照してください。

**推奨方法:**

1. **アーカイブファイルとして保存**（最も簡単）
   ```bash
   ./create_gif_archive.sh
   # 作成された gif_archive.tar.gz をGitHub Releasesや外部ストレージにアップロード
   ```

2. **rsyncで直接同期**
   ```bash
   ./sync_gif_rsync.sh user@remote-server /path/to/destination
   ```

3. **Git LFSを使用**（GitHub有料プランが必要な場合あり）
   ```bash
   ./setup_git_lfs.sh
   ```

詳細な手順とその他の方法については、`sync_gif_folder.md`を参照してください。

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

- **`website/`**: Web公開用のファイル一式（HTML, JS, MP4）
    - `google_sites_embed.html`: 埋め込み用HTMLファイル
    - `fk_web_data.js`: シミュレーションデータ
    - `mp4/`: MP4動画ファイル
- **`scripts/`**: データ処理・管理用スクリプト
    - `generate_web_data.py`: Web用データ生成
    - `resize_gifs.py`: 画像リサイズ
    - `fk_analysis.py`: 解析ライブラリ
- **`gif/`**: オリジナルGIF出力（高解像度）
- **`gif_half/`**: リサイズ版GIF出力
- **`main.c`**: シミュレーション本体（C言語）

## ホームページ（可視化マップ）

GitHub Pagesでインタラクティブなf-kマップを公開しています。

**URL**: [https://miuraTakashi.github.io/GrayScott/website/google_sites_embed.html](https://miuraTakashi.github.io/GrayScott/website/google_sites_embed.html)

**アクセス方法**:
1. 上記URLをクリックして開きます。
2. 画面上部の "GIF Base URL" 入力欄に `./mp4` と入力します（デフォルトで入力されている場合もあります）。
3. マップ上の点をクリックすると、そのパラメータに対応するシミュレーション動画が再生されます。

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

## 更新履歴

### 2025/11/15
- **可視化機能の改善**
  - Jupyter Notebook (`fk_click_show_gif.ipynb`) のGIF表示サイズを128x128に最適化
  - カラーバーの非表示オプションを追加
  - GIFプレビューウィンドウの余白を削除し、実際のGIFサイズに合わせて表示

- **データエクスポート機能**
  - Wolfram Language互換形式へのエクスポート機能を追加 (`export_to_wolfram.py`)
  - CSV、JSON、Wolfram Language (.wl) 形式でのデータ出力に対応
  - `fk_analysis.py`にエクスポート関数を追加

- **プロジェクト整理**
  - Google Sites関連の試行ファイルを削除（13ファイル）
  - 一時ファイルとデバッグスクリプトを整理
  - README.mdに更新履歴セクションを追加

### 2025/10/05
- **大規模データ処理の最適化**
  - GIFフォルダ（約7.3GB、15,000ファイル以上）の同期方法を整理
  - Git LFS、rsync、アーカイブ作成スクリプトを追加
  - `.gitignore`にgifフォルダを追加

- **インタラクティブ可視化ツールの追加**
  - `fk_analysis.py`: GIF解析とデータ処理ライブラリ
  - `fk_click_show_gif.ipynb`: f-kパラメータマップのインタラクティブ可視化
  - 空間変化率によるカラーマップ表示
  - クリックで対応するGIFアニメーションを表示
  - 理論的分岐曲線（Hopf分岐、数値解境界）の表示

### 初期リリース
- Gray-Scott反応拡散モデルのC言語実装
- OpenMP並列化による高性能シミュレーション
- GIFアニメーションとMP4動画の生成機能
- パラメータスイープ機能（1,500通りの組み合わせ）

## 作者

- **開発者**: [あなたの名前]
- **連絡先**: [メールアドレス]
- **GitHub**: [GitHubプロフィール]

---

⭐ このプロジェクトが役に立った場合は、スターをつけていただけると嬉しいです！ 