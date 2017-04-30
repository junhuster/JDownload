#!/bin/sh
gcc -o JDownload JWebFileTrans.c JDownload.c
echo "start to download"
./JDownload 1 "http://www.flashget.com/apps/flashget3.7.0.1222cn.exe"
echo "download complete"
