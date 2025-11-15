#!/bin/bash
# Git LFSをセットアップしてgifフォルダを管理するスクリプト

set -e

echo "Git LFSのセットアップを開始します..."

# Git LFSがインストールされているか確認
if ! command -v git-lfs &> /dev/null; then
    echo "エラー: Git LFSがインストールされていません"
    echo "インストール方法:"
    echo "  macOS: brew install git-lfs"
    echo "  または https://git-lfs.github.com/ からダウンロード"
    exit 1
fi

# Git LFSを初期化
echo "Git LFSを初期化しています..."
git lfs install

# .gitattributesファイルを作成または更新
echo ".gitattributesファイルを設定しています..."
if [ -f .gitattributes ]; then
    if ! grep -q "gif/\*\*" .gitattributes; then
        echo "gif/** filter=lfs diff=lfs merge=lfs -text" >> .gitattributes
    else
        echo "gif/** の設定は既に存在します"
    fi
else
    echo "gif/** filter=lfs diff=lfs merge=lfs -text" > .gitattributes
fi

echo ""
echo "設定が完了しました！"
echo ""
echo "次のステップ:"
echo "1. .gitattributesをコミット:"
echo "   git add .gitattributes"
echo "   git commit -m 'Configure Git LFS for gif folder'"
echo ""
echo "2. gifフォルダを追加（注意: 大きなデータです）:"
echo "   git add gif/"
echo "   git commit -m 'Add gif folder via Git LFS'"
echo "   git push"
echo ""
echo "注意: GitHubの無料アカウントでは月1GBのストレージ制限があります。"
echo "7.3GBのデータには有料プランが必要な場合があります。"

