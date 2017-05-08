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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <pthread.h>

#define jbp_file_not_correct 0
#define jbp_file_correct 1
#define jbp_file_empty 2

#define max_download_thread 4
#define download_one_piece_size 1024*200



/**
 ** several fix laster will be added such as:
 ** download process bar
 ** check recv buffer "content-length"
 ** real break point support
 */

typedef struct break_point{

    long file_size;
    long num_of_part_file;
    long size_of_one_piece;
    long total_num_of_piece_of_whole_file;
    char server_ip[128+1];
    int  server_port;

}break_point;

typedef struct break_point_part
{
    long start_num_of_piece_of_this_part_file;
    long end_num_of_piece_of_this_part_file;
    long size_of_last_incomplete_piece;
    long alread_download_num_of_piece;
}break_point_of_part;

typedef struct thread_func_arg
{   
    int part_num;
    char path[500];
    char ip_str[129];
    char port[20];
    char file_name[100];
}thread_func_arg;

int Http_link_parse(char *link, char **url, char **port, char **path, char **file_name);
int Http_get_ip_str_from_url(char *url, char **ip_str);
int Http_connect_to_server(char *ip, int port, int *socket_fd);
int Http_query_file_size(char *path, char *host_ip, char *port, int socket_fd, long long *file_size);

int Http_create_breakpoint_file(char *file_name, FILE **fp_breakpoint, long file_size, long num_of_part_file, long size_of_one_piece,
                                                                                       long total_num_of_piece_of_whole_file,
                                                                                       char *server_ip, int server_port);
int Http_create_breakpoint_part_file(char *file_name, FILE **fp_breakpoint_part, int part_num, long start_num_of_piece_of_this_part_file,
                                                                                 long end_num_of_piece_of_this_part_file,
                                                                                 long size_of_last_incompelet_piece,
                                                                                 long alread_download_num_of_piece);
int Update_breakpoint_part_file(FILE *fp_breakpoint_part, int num_of_piece_tobe_added);
int Delete_breakpoint_file(char *file_name, FILE *fp);


int Http_create_download_file(char *file_name, FILE **fp_download_file,int part);
int Http_restore_orignal_file_name(char *download_part_file_name);


int Http_recv_file(int socket_fd, long long range_begin, long long range_end, unsigned char *buffer,long buffer_size,
                                                                                char *path, char *host_ip, char *port);
int Save_download_part_of_file(FILE *fp, unsigned char *buffer, long buffer_size, long long file_offset);
int Merge_all_download_part_file(char *file_name, int part_num);

void *thread_func(void *arg);
int JHttp_download_whole_file(char *link);
int JHttp_download_whole_file_jbp(char *link);