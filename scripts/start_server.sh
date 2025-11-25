#!/bin/bash
# Gray-Scott Visualization ローカルサーバー起動スクリプト

echo "=========================================="
echo "Gray-Scott Visualization サーバーを起動中..."
echo "=========================================="
echo ""
echo "ブラウザで以下のURLを開いてください:"
echo "  http://localhost:8000/fk_click_show_gif.html"
echo ""
echo "サーバーを停止するには Ctrl+C を押してください"
echo "=========================================="
echo ""

cd "$(dirname "$0")"
python3 -m http.server 8000

