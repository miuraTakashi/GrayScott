#!/bin/bash
# rsyncを使用してGIFフォルダを同期するスクリプト

set -e

GIF_DIR="gif"
REMOTE_HOST="${1:-}"
REMOTE_PATH="${2:-}"

if [ -z "$REMOTE_HOST" ] || [ -z "$REMOTE_PATH" ]; then
    echo "使用方法: $0 <リモートホスト> <リモートパス>"
    echo "例: $0 user@example.com /path/to/destination"
    echo ""
    echo "または環境変数を設定:"
    echo "  export GIF_SYNC_HOST=user@example.com"
    echo "  export GIF_SYNC_PATH=/path/to/destination"
    exit 1
fi

# 環境変数から読み込み（オプション）
REMOTE_HOST="${GIF_SYNC_HOST:-$REMOTE_HOST}"
REMOTE_PATH="${GIF_SYNC_PATH:-$REMOTE_PATH}"

echo "GIFフォルダを同期します..."
echo "送信元: ${GIF_DIR}/"
echo "送信先: ${REMOTE_HOST}:${REMOTE_PATH}/"
echo ""

# rsyncで同期（進捗表示、圧縮、削除されたファイルも削除）
rsync -avz --progress --delete \
    "${GIF_DIR}/" \
    "${REMOTE_HOST}:${REMOTE_PATH}/"

echo ""
echo "同期が完了しました！"

