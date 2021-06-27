# 搭建基于nginx的rtmp服务端
##  准备环境
1.  安装好的docker，并更换为国内源。[点击查看国内源教程](https://blog.csdn.net/weixin_45485719/article/details/106200988)
2.  拉取ubuntu:18.04的镜像:
    ```shell 
    > docker pull ubuntu:18.04
    ```
3.  拉起容器，开放22，80，443，1935，8080 并映射到宿主机:
    ```shell
    > docker run -itd -p 22:22 -p 80:80 -p 443:443 -p 1935:1935 -p 8080:8080 --name u18_rtmp ubuntu:18.04 /bin/bash
    ```
4.  进入容器:
    ```shell
    > docker exec -it u18_rtmp /bin/bash 
    ```
5.  更换ubuntu源为国内源：
    ```shell
    $ cp -a /etc/apt/sources.list /etc/apt/sources.list.bak
    $ sed -i "s@http://.*archive.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list
    $ sed -i "s@http://.*security.ubuntu.com@http://repo.huaweicloud.com@g" /etc/apt/sources.list
    $ apt-get update -y
    $ apt-get install zip unzip wget vim git -y
    ```
6.  安装ssh服务(可选，看你自己的操作习惯了)
    ```shell
    $ passwd root
    $ apt-get install openssh-server -y
    $ vim /ett/ssh/ssd_config   # 修改 `#PermitRootLogin prohibit-password` 为 `PermitRootLogin yes`
    $ service ssh start
    ```
##  编译安装nginx
1.  下载安装包
    ```shell
    $ mkdir -p /home/mushroom/download
    $ cd /home/mushroom/download
    $ wget http://nginx.org/download/nginx-1.20.1.tar.gz
    $ wget https://codeload.github.com/arut/nginx-rtmp-module/zip/refs/tags/v1.2.2
    ```
2.  编译安装
    ```shell
    $ apt-get install build-essential zlib1g-dev libpcre3 libpcre3-dev libssl-dev libxslt1-dev libxml2-dev libgd-dev libgeoip-dev libgoogle-perftools-dev libperl-dev libtool openssl -y
    $ tar -zvxf nginx-1.20.1.tar.gz
    $ unzip nginx-rtmp-module-1.2.2.zip
    $ mv nginx-rtmp-module-1.2.2 nginx-1.20.1/
    $ cd nginx-1.20.1
    $ ./configure --prefix=/usr/local/nginx --add-module=./nginx-rtmp-module-1.2.2 --with-http_ssl_module --with-http_v2_module
    $ make -j4
    $ make install
    ```
## 配置nginx
1.  配置ssl证书
    ```shell
    $ mkdir -p /home/mushroom/nginx/cert #拷贝证书文件到此目录
    ```
2.  修改 `/usr/local/nginx/conf/mime.types` 为
    ```text
    types {
        text/html                                        html htm shtml;
        text/css                                         css;
        text/xml                                         xml;
        image/gif                                        gif;
        image/jpeg                                       jpeg jpg;
        application/javascript                           js;
        application/atom+xml                             atom;
        application/rss+xml                              rss;

        text/mathml                                      mml;
        text/plain                                       txt;
        text/vnd.sun.j2me.app-descriptor                 jad;
        text/vnd.wap.wml                                 wml;
        text/x-component                                 htc;

        image/png                                        png;
        image/svg+xml                                    svg svgz;
        image/tiff                                       tif tiff;
        image/vnd.wap.wbmp                               wbmp;
        image/webp                                       webp;
        image/x-icon                                     ico;
        image/x-jng                                      jng;
        image/x-ms-bmp                                   bmp;

        font/woff                                        woff;
        font/woff2                                       woff2;

        application/java-archive                         jar war ear;
        application/json                                 json;
        application/mac-binhex40                         hqx;
        application/msword                               doc;
        application/pdf                                  pdf;
        application/postscript                           ps eps ai;
        application/rtf                                  rtf;
        application/vnd.apple.mpegurl                    m3u8;
        application/vnd.google-earth.kml+xml             kml;
        application/vnd.google-earth.kmz                 kmz;
        application/vnd.ms-excel                         xls;
        application/vnd.ms-fontobject                    eot;
        application/vnd.ms-powerpoint                    ppt;
        application/vnd.oasis.opendocument.graphics      odg;
        application/vnd.oasis.opendocument.presentation  odp;
        application/vnd.oasis.opendocument.spreadsheet   ods;
        application/vnd.oasis.opendocument.text          odt;
        application/vnd.openxmlformats-officedocument.presentationml.presentation
                                                        pptx;
        application/vnd.openxmlformats-officedocument.spreadsheetml.sheet
                                                        xlsx;
        application/vnd.openxmlformats-officedocument.wordprocessingml.document
                                                        docx;
        application/vnd.wap.wmlc                         wmlc;
        application/x-7z-compressed                      7z;
        application/x-cocoa                              cco;
        application/x-java-archive-diff                  jardiff;
        application/x-java-jnlp-file                     jnlp;
        application/x-makeself                           run;
        application/x-perl                               pl pm;
        application/x-pilot                              prc pdb;
        application/x-rar-compressed                     rar;
        application/x-redhat-package-manager             rpm;
        application/x-sea                                sea;
        application/x-shockwave-flash                    swf;
        application/x-stuffit                            sit;
        application/x-tcl                                tcl tk;
        application/x-x509-ca-cert                       der pem crt;
        application/x-xpinstall                          xpi;
        application/xhtml+xml                            xhtml;
        application/xspf+xml                             xspf;
        application/zip                                  zip;

        application/octet-stream                         bin exe dll;
        application/octet-stream                         deb;
        application/octet-stream                         dmg;
        application/octet-stream                         iso img;
        application/octet-stream                         msi msp msm;

        audio/midi                                       mid midi kar;
        audio/mpeg                                       mp3;
        audio/ogg                                        ogg;
        audio/x-m4a                                      m4a;
        audio/x-realaudio                                ra;

        video/3gpp                                       3gpp 3gp;
        video/mp2t                                       ts;
        video/mp4                                        mp4;
        video/mpeg                                       mpeg mpg;
        video/quicktime                                  mov;
        video/webm                                       webm;
        video/x-flv                                      flv;
        video/x-m4v                                      m4v;
        video/x-mng                                      mng;
        video/x-ms-asf                                   asx asf;
        video/x-ms-wmv                                   wmv;
        video/x-msvideo                                  avi;
    }
    ```
3. 配置nginx.conf，修改 `/usr/local/nginx/conf/nginx.conf` 为
    ```shell
    # For more information on configuration, see:
    #   * Official English Documentation: http://nginx.org/en/docs/
    #   * Official Russian Documentation: http://nginx.org/ru/docs/

    # user nginx;
    worker_processes auto;
    error_log /usr/local/nginx/logs/error.log;
    pid /run/nginx.pid;

    # Load dynamic modules. See /usr/share/nginx/README.dynamic.
    include /usr/share/nginx/modules/*.conf;

    events {
        worker_connections 1024;
    }

    http 
    {
        log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                        '$status $body_bytes_sent "$http_referer" '
                        '"$http_user_agent" "$http_x_forwarded_for"';

        access_log  /usr/local/nginx/logs/access.log  main;

        sendfile            on; #内核直接发送文件到客户端
        tcp_nopush          on; #够一定数据量再发送
        tcp_nodelay         on; #同上
        keepalive_timeout   120;
        types_hash_max_size 2048;

        include             /usr/local/nginx/conf/mime.types;
        default_type        application/octet-stream;

        # Load modular configuration files from the /etc/nginx/conf.d directory.
        # See http://nginx.org/en/docs/ngx_core_module.html#include
        # for more information.
        include /etc/nginx/conf.d/*.conf;

        server {
            listen       80 default_server;
    #        listen       [::]:80 default_server;
            server_name  _;
            root         /usr/share/nginx/html;

            # Load configuration files for the default server block.
            include /etc/nginx/default.d/*.conf;
            location / {
                resolver 223.5.5.5;
                #proxy_pass $scheme://$http_host$request_uri;
                proxy_buffers   256 4k;                         
                proxy_max_temp_file_size 0k; 
            }
            error_page 404 /404.html;
                location = /40x.html {
            }

            error_page 500 502 503 504 /50x.html;
                location = /50x.html {
            }
        }

    # Settings for a TLS enabled server.
    #
        server {
            listen       443 ssl http2 default_server;
            listen       [::]:443 ssl http2 default_server;
            server_name  _;
            root         /usr/share/nginx/html;

            ssl_certificate "/home/mushroom/nginx/cert/5780116_rtmp.xiangxiang.love.pem";
            ssl_certificate_key "/home/mushroom/nginx/cert/5780116_rtmp.xiangxiang.love.key";
            ssl_session_cache shared:SSL:1m;
            ssl_session_timeout  10m;
            ssl_ciphers HIGH:!aNULL:!MD5;
            ssl_prefer_server_ciphers on;

            # Load configuration files for the default server block.
            include /etc/nginx/default.d/*.conf;

            location / {
            }

            error_page 404 /404.html;
                location = /40x.html {
            }

            error_page 500 502 503 504 /50x.html;
                location = /50x.html {
            }
        }
        server 
        {
            listen 8080;
            # Configure the RTMP status overview HTTP page=========================================
            location /stat 
            {
                rtmp_stat all;
                rtmp_stat_stylesheet stat.xsl;
            }
            location /stat.xsl 
            {
                root /opt/rtmp/nginx-rtmp-module/;
            }
            # The configuration RTMP status overview interface ends==========================
        }

    }
    # 直播功能实现配置rtmp协议
    rtmp 
    {
        server 
        {
            listen 1935;
            chunk_size 4000;
            application live 
            {
                live on;
            }
        }
    }
    ```
4.  启动关闭
    ```shell
    $ /usr/local/nginx/sbin/nginx #启动
    $ /usr/local/nginx/sbin/nginx -s stop #停止
    $ /usr/local/nginx/sbin/nginx -s reload #重新加载
    ```
## 测试
1.  播放流:  
    VLC->媒体->打开网络串流->rtmp://127.0.0.1:1935/live/test->播放
2.  推流：
    ```
    > ffmpeg -re -i ./连名带姓-黄霄云.flv -f flv rtmp://127.0.0.1:1935/live/test
    ```
## 推流代码
1.  详见 [国内gitee](https://gitee.com/xmushroom/x_ffmpeg/tree/main/rtmp_pusher_1.0) \ [全球最大同性交友平台](https://github.com/Mushroom0709/x_ffmpeg/tree/main/rtmp_pusher_1.0)。均配置为VS2019-Debug/Release-x64下一键启动。