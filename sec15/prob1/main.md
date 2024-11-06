## a
- 正しいことを確認する
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob1$ ls -l test_a.txt
----rwxrwx 1 bin101 bin101 0 11月  6 07:12 test_a.txt
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob1$ cat test_a.txt
cat: test_a.txt: 許可がありません
```
```
$ id
uid=1001(outsider) gid=1001(outsider) groups=1001(outsider)
$ cat test_a.txt
$
```

## b
- 検索許可: x
```
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob1$ chmod 677 test_b
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob1$ ls test_b
ls: 'test_b/test_b.txt' にアクセスできません: 許可がありません
test_b.txt
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob1$ cat test_b/test_b.txt
cat: test_b/test_b.txt: 許可がありません
bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob1$ cd test_b
bash: cd: test_b: 許可がありません
```
- lsでエラーは出たが、中身は見ることができた(??)

## c
- ディレクトリの必要なパーミション
    - ファイル新規作成
        - wx
    - 読み取り用オープン
        - x
    - 書き込み用のオープン
        - x
    - ファイル削除
        - wx
    - 移動元ディレクトリ
        - wx
            - ファイル削除と同じなのは直感とあう
    - 移動先ディレクトリ
        - wx
- ファイルの必要なパーミション
    - ファイル新規作成
        - 関係なし
    - 読み取り用オープン
        - r
    - 書き込み用のオープン
        - w
    - ファイル削除
        - 必要なし
    - 移動先ファイル
        - 必要なし
- スティッキービットの影響
    - ファイル移動
        - 移動元ディレクトリにスティッキービットがあったら禁止
        ```
        bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob1/sticky$ sudo -iu outsider
        sudo: ディレクトリーを /home/outsider に変更できません: そのようなファイルやディレクトリはありません
        $ ls -l . from_dir7       
        .:
        合計 8
        drwxrwxrwt 2 bin101 bin101 4096 11月  6 20:01 from_dir7
        drwxrwxrwx 2 bin101 bin101 4096 11月  6 20:02 to_dir7

        from_dir7:
        合計 0
        -rwxrwxrwx 1 bin101 bin101 0 11月  6 20:01 file7
        $ mv from_dir7/file7 to_dir7/file7
        mv: 'from_dir7/file7' から 'to_dir7/file7' へ移動できません: 許可されていない操作です
        ```
        - 移動先にスティッキービットがある場合、可能
        ```
        bin101@bin101-Inspiron-16-5635:~/code/LinuxProgrammingInterface/sec15/prob1/sticky$ sudo -iu outsider
        sudo: ディレクトリーを /home/outsider に変更できません: そのようなファイルやディレクトリはありません
        $ ls -l . from_dir7
        .:
        合計 8
        drwxrwxrwx 2 bin101 bin101 4096 11月  6 20:01 from_dir7
        drwxrwxrwt 2 bin101 bin101 4096 11月  6 20:02 to_dir7

        from_dir7:
        合計 0
        -rwxrwxrwx 1 bin101 bin101 0 11月  6 20:01 file7
        $ mv from_dir7/file7 to_dir7/file7
        $ ls to_dir7
        file7
        ```
    - ファイル削除
        - 削除できなかった