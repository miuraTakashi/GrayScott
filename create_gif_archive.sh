#!/bin/bash
# GIFフォルダをアーカイブに圧縮するスクリプト

set -e

GIF_DIR="gif"
ARCHIVE_NAME="gif_archive"
MAX_SIZE="2G"  # 各アーカイブの最大サイズ（2GB）

echo "GIFフォルダをアーカイブに圧縮します..."
echo "フォルダサイズ: $(du -sh ${GIF_DIR} | cut -f1)"
echo "ファイル数: $(find ${GIF_DIR} -type f | wc -l)"

# アーカイブを作成（分割なし、単一ファイル）
if [ "$1" = "--split" ]; then
    echo "分割アーカイブを作成します..."
    tar -czf - ${GIF_DIR} | split -b ${MAX_SIZE} - ${ARCHIVE_NAME}.tar.gz.part
    echo "分割アーカイブを作成しました: ${ARCHIVE_NAME}.tar.gz.part*"
    echo "復元方法: cat ${ARCHIVE_NAME}.tar.gz.part* | tar -xzf -"
else
    echo "単一アーカイブを作成します（時間がかかる場合があります）..."
    tar -czf ${ARCHIVE_NAME}.tar.gz ${GIF_DIR}
    echo "アーカイブを作成しました: ${ARCHIVE_NAME}.tar.gz"
    echo "サイズ: $(du -sh ${ARCHIVE_NAME}.tar.gz | cut -f1)"
    echo "復元方法: tar -xzf ${ARCHIVE_NAME}.tar.gz"
fi

echo "完了しました！"

