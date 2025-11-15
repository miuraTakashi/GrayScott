# GIFフォルダの同期方法

gifフォルダは7.3GB、15,601ファイルを含むため、Gitで直接管理するのは適していません。
以下の方法で安全に同期できます。

## 推奨方法

### 1. Git LFS (Large File Storage) を使用する

Git LFSを使用すると、大きなファイルをGitリポジトリ外で管理できます。

#### セットアップ
```bash
# Git LFSをインストール（未インストールの場合）
# macOS: brew install git-lfs
# または https://git-lfs.github.com/ からダウンロード

# リポジトリでGit LFSを初期化
git lfs install

# .gitattributesファイルを作成
echo "gif/** filter=lfs diff=lfs merge=lfs -text" >> .gitattributes

# gifフォルダを追加
git add .gitattributes
git add gif/
git commit -m "Add gif folder using Git LFS"
git push
```

**注意**: GitHubの無料アカウントでは月1GBのストレージと1GB/月の帯域幅制限があります。
7.3GBのデータには有料プランが必要な場合があります。

### 2. アーカイブファイルとして保存（推奨）

gifフォルダを圧縮して、GitHub Releasesや別の場所に保存します。

#### アーカイブ作成スクリプト
```bash
# gifフォルダを圧縮（分割アーカイブ）
tar -czf gif_archive_part1.tar.gz --files-from=<(find gif -type f | head -n 7800)
tar -czf gif_archive_part2.tar.gz --files-from=<(find gif -type f | tail -n +7801)

# または単一アーカイブ（時間がかかります）
tar -czf gif_archive.tar.gz gif/
```

#### 復元方法
```bash
tar -xzf gif_archive.tar.gz
```

### 3. rsyncを使用した直接同期

別のサーバーやストレージに直接同期します。

```bash
# リモートサーバーに同期
rsync -avz --progress gif/ user@remote-server:/path/to/gif/

# またはローカルネットワークの別のマシンに同期
rsync -avz --progress gif/ /path/to/destination/
```

### 4. 外部ストレージサービスを使用

- **Google Drive / Dropbox**: クラウドストレージにアップロード
- **AWS S3 / Google Cloud Storage**: オブジェクトストレージを使用
- **GitHub Releases**: アーカイブファイルをリリースとしてアップロード

### 5. データ生成スクリプトを提供

gifファイルを再生成できるスクリプトを提供し、必要に応じて生成する方法。

## 現在の設定

現在、gifフォルダは`.gitignore`で無視されているため、Gitでは管理されていません。
これは正しい設定です。上記のいずれかの方法で同期してください。

## 推奨されるワークフロー

1. **開発環境**: gifフォルダはローカルに保持
2. **共有**: Git LFSまたはアーカイブファイルを使用
3. **バックアップ**: 外部ストレージに定期的にバックアップ

