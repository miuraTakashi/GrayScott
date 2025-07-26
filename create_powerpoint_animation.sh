#!/bin/bash

# PowerPoint用のアニメーション画像を作成するスクリプト

echo "Creating PowerPoint-compatible animation frames..."

# gifディレクトリ内のGIFファイルを検索
for gif_file in gif/*.gif; do
    if [ -f "$gif_file" ]; then
        # ファイル名から拡張子を除去
        basename=$(basename "$gif_file" .gif)
        
        # 出力ディレクトリを作成
        output_dir="gif/powerpoint_${basename}"
        mkdir -p "$output_dir"
        
        echo "Processing $gif_file..."
        
        # ImageMagickを使ってGIFを個別のPNG画像に分解
        if command -v convert >/dev/null 2>&1; then
            convert "$gif_file" "$output_dir/frame_%03d.png"
            echo "Created PNG frames in $output_dir/"
            echo "PowerPointで使用方法:"
            echo "1. すべてのPNG画像をPowerPointにインポート"
            echo "2. 各スライドに1つの画像を配置"
            echo "3. スライド切り替えを'なし'に設定し、自動切り替えを0.1秒に設定"
        else
            echo "ImageMagick (convert command) が見つかりません。"
            echo "brew install imagemagick でインストールしてください。"
        fi
        
        break # 最初のGIFファイルのみ処理
    fi
done

echo "完了!" 