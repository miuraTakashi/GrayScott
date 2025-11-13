#!/bin/bash
# Netlify用Gitリポジトリセットアップスクリプト

set -e

echo "=========================================="
echo "Netlify用Gitリポジトリのセットアップ"
echo "=========================================="
echo ""

# 現在のディレクトリを確認
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "作業ディレクトリ: $SCRIPT_DIR"
echo ""

# .gitignoreが存在するか確認
if [ ! -f .gitignore ]; then
    echo "📝 .gitignoreを作成中..."
    cat > .gitignore << 'EOF'
__pycache__/
*.pyc
*.pyo
*.pyd
.Python
*.so
*.egg
*.egg-info/
dist/
build/
a.out
gray_scott
main
*.npz
.DS_Store
*.swp
*.swo
*~
EOF
    echo "✅ .gitignoreを作成しました"
else
    echo "✅ .gitignoreは既に存在します"
fi

echo ""

# Gitリポジトリが既に初期化されているか確認
if [ -d .git ]; then
    echo "⚠️  Gitリポジトリは既に初期化されています"
    read -p "続行しますか？ (y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "中断しました"
        exit 1
    fi
else
    echo "📦 Gitリポジトリを初期化中..."
    git init
    echo "✅ Gitリポジトリを初期化しました"
fi

echo ""

# ファイルをステージング
echo "📁 ファイルをステージング中..."
echo "   （15,601個のGIFファイルがあるため、時間がかかります）"
echo ""

git add .gitignore
git add fk_click_show_gif.html
git add fk_data.json

echo "📁 GIFファイルを追加中..."
echo "   （約7.3GB、15,601ファイル - 数分かかる場合があります）"
git add gif/

echo ""
echo "✅ ファイルのステージングが完了しました"
echo ""

# コミット
echo "💾 コミット中..."
git commit -m "Initial commit: Add Gray-Scott visualization files

- Interactive f-k parameter map visualization
- 15,601 GIF animation files
- Data cache file (fk_data.json)"

echo ""
echo "✅ コミットが完了しました"
echo ""

# ステータス表示
echo "=========================================="
echo "📊 リポジトリの状態"
echo "=========================================="
git status --short | head -20
echo "..."
echo ""

# 次のステップを表示
echo "=========================================="
echo "🎯 次のステップ"
echo "=========================================="
echo ""
echo "1. GitHubでリポジトリを作成:"
echo "   https://github.com/new"
echo ""
echo "2. リモートリポジトリを追加:"
echo "   git remote add origin https://github.com/ユーザー名/リポジトリ名.git"
echo ""
echo "3. GitHubにプッシュ:"
echo "   git branch -M main"
echo "   git push -u origin main"
echo ""
echo "   ⚠️  7.3GBのプッシュには30分〜1時間かかる場合があります"
echo ""
echo "4. NetlifyでGitHubリポジトリを連携:"
echo "   https://app.netlify.com"
echo "   → Add new site → Import an existing project → GitHub"
echo ""
echo "=========================================="

