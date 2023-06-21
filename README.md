# OMOS

## Overview
## git command

1. clone the project
    ```shell
    git clone https://github.com/remon-nomer66/OMOS.git
    ```
2. ブランチを切り替える
    ```shell
    git checkout [編集するブランチ名]
    ```

3. 編集等を行った後
    ```shell
    git add . 
    or
    git add [編集したファイル名/階層名] 
    ```

4. コミットメッセージを入力
    ```shell
    git commit -m "commit message"
    ```

5.  リモートリポジトリに反映
    ```
    git push origin [編集したブランチ名]
    ```

# 再度編集を再開する場合
## 作業前
1. スタッシュ
    ```shell
    git stash -u
    ```
2. developブランチに移動,最新のorigin developブランチのソースを取ってくる
    ```shell
    git checkout develop
    git pull origin develop
    ```
3. 開発していたブランチに戻る
    ```shell
    git checkout 自分のブランチ
    ```
4. developブランチを開発ブランチにリベース
     ```shell
    git rebase develop
    ```
5. スタッシュを戻す
    ```shell
    git stash apply stash@{0}
    ```

## 適当に更新しておくのでこれ以降は適度に更新してね

