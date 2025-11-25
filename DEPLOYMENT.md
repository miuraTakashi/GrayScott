# デプロイメントガイド: Gray-Scott可視化アプリ

このドキュメントでは、`visualization/fk_click_show_gif.ipynb`をホームページ上で動かすための方法を説明します。

## 方法1: Google Colab（最も簡単）

### メリット
- ✅ セットアップ不要
- ✅ 無料
- ✅ すぐに共有可能

### デプロイ手順

1. **Colabにnotebookをアップロード**
   - Google Colab (https://colab.research.google.com/) にアクセス
   - `visualization/fk_click_show_gif.ipynb`をアップロード

2. **GIFファイルをアップロード**
   - Colabのファイルシステムに`gif`フォルダをアップロード
   - または、Google Driveに配置してマウント

3. **共有リンクを取得**
   - Colabの「共有」ボタンからリンクを取得
   - 「リンクを知っている全員」に設定

4. **ホームページに埋め込み**
   ```html
   <iframe 
     src="https://colab.research.google.com/github/yourusername/GrayScott/blob/main/visualization/fk_click_show_gif.ipynb" 
     width="100%" 
     height="800px">
   </iframe>
   ```

### 注意事項
- Colabのセッションは一定時間でタイムアウトします
- 大きなGIFフォルダのアップロードには時間がかかります
- Google Driveに配置してマウントする方が効率的です

---

## 方法2: Voila

### メリット
- ✅ Jupyter notebookをそのまま使用可能
- ✅ インタラクティブ性を維持

### デプロイ手順

#### 1. ローカルでVoilaをインストール
```bash
pip install voila
```

#### 2. Voilaサーバーを起動
```bash
voila visualization/fk_click_show_gif.ipynb
```

#### 3. クラウドにデプロイ（例: Heroku）

1. **Procfileを作成**
   ```
   web: voila visualization/fk_click_show_gif.ipynb --port=$PORT --enable_nbextensions=True
   ```

2. **requirements.txtに追加**
   ```
   voila>=0.5.0
   ipywidgets>=8.0.0
   ipympl>=0.9.0
   ```

3. **Herokuにデプロイ**
   ```bash
   heroku create your-app-name
   git push heroku main
   ```

---

## 方法3: Binder

### メリット
- ✅ GitHubリポジトリから自動起動
- ✅ 無料
- ✅ セットアップ不要

### デプロイ手順

1. **requirements.txtを作成**
   ```
   ipywidgets>=8.0.0
   ipympl>=0.9.0
   pillow>=10.0.0
   numpy>=1.24.0
   matplotlib>=3.7.0
   ```

2. **GitHubにプッシュ**

3. **Binderリンクを作成**
   ```
   https://mybinder.org/v2/gh/yourusername/GrayScott/main
   ```

4. **ホームページに埋め込み**
   ```html
   <iframe 
     src="https://mybinder.org/v2/gh/yourusername/GrayScott/main?urlpath=voila%2Frender%2Fvisualization%2Ffk_click_show_gif.ipynb" 
     width="100%" 
     height="800px">
   </iframe>
   ```

### 注意事項
- Binderの起動には数分かかります
- セッションは一定時間でタイムアウトします
- 大きなGIFフォルダはGit LFSが必要です

---

## 推奨方法の比較

| 方法 | 難易度 | コスト | パフォーマンス | 埋め込み | 推奨度 |
|------|--------|--------|----------------|----------|--------|
| **Google Colab** | ⭐ | 無料 | ⭐⭐⭐ | ⚠️ | ⭐⭐⭐⭐ |
| **Voila** | ⭐⭐⭐ | 無料〜有料 | ⭐⭐⭐ | ✅ | ⭐⭐⭐ |
| **Binder** | ⭐⭐ | 無料 | ⭐⭐ | ✅ | ⭐⭐⭐ |

---

## 大容量GIFファイルの扱い（7GB、15,000枚）

大容量のGIFファイルを扱う場合、以下の方法が推奨されます：

- **GitHub Pages**: 静的ファイルとしてホスティング（Git LFS使用）
- **外部ストレージ**: AWS S3、Google Cloud Storageなどに配置
- **サンプルデータ**: 開発・デモ用に100-200枚のGIFのみを使用

---

## トラブルシューティング

### Colabでmatplotlib widgetが動かない
- `!pip install ipympl`を実行
- カーネルを再起動

### Voilaでウィジェットが表示されない
- `voila --enable_nbextensions=True`で起動
- `jupyter nbextension enable --py widgetsnbextension`を実行

---

## 参考リンク

- [Google Colab](https://colab.research.google.com/)
- [Voila Documentation](https://voila.readthedocs.io/)
- [Binder Documentation](https://mybinder.readthedocs.io/)

