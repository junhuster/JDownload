/**
    JDownload: a socket based app which can download web resource
    Copyright (C) 2017  zhang jun
    contact me: zhangjunhust@hust.edu.cn
    		http://www.cnblogs.com/junhuster/
    		http://weibo.com/junhuster 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **/
#include"JWebFileTrans.h"
int main(int argc, char *argv[]){
	
	if(argc!=3){
		printf("argument error\n");
		exit(0);
	}
	
	/** parse protocol **/
	char *kind=argv[1];
	char *link=argv[2];

	int kind_int=atoi(kind);	

	char *ptr=NULL;	

	if(kind_int==0){
		ptr=strstr(link,"http://");
		if(ptr!=NULL){
			printf("download link:\n%s\n",link);
			JHttp_download_whole_file(link);
			printf("download success\n");
			return 0;
		}

		ptr=strstr(link,"ftp://");
		if(ptr!=NULL){
			printf("ftp download will be supported later\n");
			return 0;
		}else{
		
			printf("not support protocol, download not execute\n");
			return 0;
		}

	}else if(kind_int==1){
		printf("download link:\n%s\n",link);
		JHttp_download_whole_file_jbp(link);
		printf("download success\n");
		return 0;
	}else{
		printf("JDownload: argument error\n");
		return 0;
	}
		
	return 0;
}
