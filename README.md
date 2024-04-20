#   通过glibc版本获取glibc库

### 编译前需要安装一下依赖

1.  Ubuntu或Debian：
    ```sh
    sudo apt-get update
    sudo apt-get install zlib1g-dev libcurl4-openssl-dev libarchive-dev
    ```
2.  CentOS或Fedora：
    ```sh
    sudo yum install zlib-devel libcurl-devel libarchive-devel
    ```
3.  Arch Linux：
    ```sh
    sudo pacman -Sy zlib curl libarchive
    ```

### 编译
```sh
make
```

编译完成后运行 `./gglibc update` `./gglibc u` `./gglibc -u`可以更新glibc列表
然后使用`./gglibc`获取glibc对应的版本