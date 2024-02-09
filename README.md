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

编译完成后运行`./update`可以更新glibc列表
然后使用`./get`获取glibc对应的版本
如:
```sh
./get
```
```sh
./get all
```
```sh
./get libc6_2.27-3ubuntu1.6_amd64
```
```sh
./get 6
```