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
/**
 ** link ex. : http://www-us.apache.org/dist/hbase/stable/hbase-1.2.4-bin.tar.gz
 ** url: points to the parsed url,ex.: www-us.apache.org/dist/hbase/stable
 ** port: points to the parsed port or the default port 80 for http
 ** file_name: points to the parsed file name, ex.: hbase-1.2.4-bin.tar.gz
 */
int Http_link_parse(char *link, char **url, char **port, char **path, char **file_name){

    /**
     ** check argument
     */
     if(NULL==link){
        printf("Http_link_parse: argument error, please provide correct link\n");
        exit(0);
     }

     char *url_begin=NULL;
     char *url_end=NULL;

     url_begin=strstr(link,"http://");
     if(NULL==url_begin){
        printf("Http_link_parse: not valid http link\n");
        exit(0);
     }

     url_begin=url_begin+7;

     int link_length=strlen(link);

     int i=0;
     for(i=link_length;i>=7;i--){
        if('/'!=link[i]){
            continue;
        }
        else{
            break;
        }
     }

     int j=0;
     for(j=7;j<link_length;j++){
        if('/'!=link[j]){
            continue;
        }else{
            break;
        }
     }

     if(j>=link_length){
        printf("Http_link_parse: Http link path not intact\n");
        exit(0);
     }

     if(i<7){
        printf("Http_link_parse: Http link path not intact\n");
        exit(0);
     }
     char *path_begin=&(link[j]);
     int path_length=link_length-j;

     char *colon=strstr(url_begin,":");
     char *port_begin=NULL;
     int url_length=0;
     int port_length=0;
     if(NULL==colon){

        *port="80";//default http port
        url_end=&(link[j]);
        url_length=url_end-url_begin;

     }else{

        port_length=&(link[i])-colon-1;
        port_begin=colon+1;

        url_length=colon-url_begin;

     }

     char *file_name_tmp=&(link[i])+1;
     int file_length=(link_length-1)-i;

     *url=(char *)malloc(sizeof(char)*(url_length+1));
     if(port_length!=0){
        *port=(char *)malloc(sizeof(char)*(port_length+1));
        if(NULL==*port){
            printf("Http_link_parsed: malloc failed\n");
            exit(0);
        }
        memcpy(*port,port_begin,port_length);
        (*port)[port_length]='\0';
     }

     *path=(char *)malloc(sizeof(char)*(path_length+1));
     *file_name=(char *)malloc(sizeof(char)*(file_length+1));

     if(NULL==*url || NULL==*path ||NULL==*file_name){
            printf("Http_link_parsed: malloc failed\n");
            exit(0);
     }

     memcpy(*url,url_begin,url_length);
     (*url)[url_length]='\0';

     memcpy(*path,path_begin,path_length);
     (*path)[path_length]='\0';

     memcpy(*file_name, file_name_tmp, file_length);
     (*file_name)[file_length]='\0';

     return 1;

}


/**
 ** url ex. : www-us.apache.org
 ** ip_str is the output, points to ip str such as : 127.0.0.1
 */
int Http_get_ip_str_from_url(char *url, char **ip_str){

    /**
     ** check argument
     */
     if(NULL==url){
        printf("Http_get_ip_str_from_url: argument error\n");
        exit(0);
     }

     struct addrinfo *addrinfo_result=NULL;
     struct addrinfo *addrinfo_cur=NULL;
     struct addrinfo hint;
     memset(&hint,0,sizeof(struct addrinfo));

     int res=getaddrinfo(url,"80",&hint,&addrinfo_result);
     if(res!=0){
        printf("Http_get_ip_str_from_url: getaddrinfo failed\n");
        exit(0);
     }

     addrinfo_cur=addrinfo_result;
     struct sockaddr_in *sockin=NULL;
     char ip_addr_str[INET_ADDRSTRLEN+1];

     if(NULL!=addrinfo_cur){
        sockin=(struct sockaddr_in *)addrinfo_cur->ai_addr;
        const char *ret=inet_ntop(AF_INET,&(sockin->sin_addr),ip_addr_str,INET_ADDRSTRLEN);
        int ip_addr_str_len=strlen(ip_addr_str);
        *ip_str=(char *)malloc(sizeof(char)*(ip_addr_str_len+1));
        if(NULL==ret){
            printf("Http_get_ip_str_from_url: ip_str malloc failed\n");
            exit(0);
        }
        memcpy(*ip_str,ip_addr_str,ip_addr_str_len);
        (*ip_str)[ip_addr_str_len]='\0';
     }

     freeaddrinfo(addrinfo_result);

     return 1;

}
/**
 ** ip: such as : 140.211.11.105
 ** port: such as 80
 */
int Http_connect_to_server(char *ip, int port, int *socket_fd){

    /**
     ** check argument error
     */
     if(ip==NULL || socket_fd==NULL){
        printf("Http_connect_to_server: argument error\n");
        exit(0);
     }

     *socket_fd=socket(AF_INET,SOCK_STREAM,0);
     if(*socket_fd<0){
        perror("Http_connect_to_server");

        exit(0);
     }

     struct sockaddr_in sock_address;
     sock_address.sin_family=AF_INET;
     int ret_0 =inet_pton(AF_INET,ip,&(sock_address.sin_addr.s_addr));
     if(ret_0!=1){
        printf("Http_connect_to_server: inet_pton failed\n");
        exit(0);
     }
     sock_address.sin_port=htons(port);

     int ret_1=connect(*socket_fd, (struct sockaddr*)&sock_address, sizeof(struct sockaddr_in));
     if(ret_1!=0){
        printf("Http_connect_to_server: invoke connect failed\n");
        exit(0);
     }

     return 1;
}

int Http_query_file_size(char *path, char *host_ip, char *port, int socket_fd,long long *file_size){
    /**
     ** check argument error
     */
     if(NULL==path || NULL==host_ip || NULL==port){

        printf("Http_query_file_size: argument error\n");
        exit(0);
     }

     char send_buffer[100];
     sprintf(send_buffer,"HEAD %s",path);
     strcat(send_buffer," HTTP/1.1\r\n");
     strcat(send_buffer,"host: ");
     strcat(send_buffer,host_ip);
     strcat(send_buffer," : ");
     strcat(send_buffer,port);
     strcat(send_buffer,"\r\nConnection: Keep-Alive\r\n\r\n");

     int ret=send(socket_fd, send_buffer, strlen(send_buffer),0);
     if(ret<1){
        printf("Http_query_file_size: send failed\n");
        exit(0);
     }

     char recv_buffer[500];
     int ret_recv=recv(socket_fd,recv_buffer,500,0);
     if(ret_recv<1){
        printf("Http_query_file_size: recv failed\n");
        exit(0);
     }

     if(recv_buffer[13]!='O' || recv_buffer[14]!='K'){
        printf("Http_query_file_size: server response message status not ok\n");
     }

     char *ptr=strstr(recv_buffer,"Content-Length");

     if(NULL==ptr){

        printf("Http_query_file_size: recv message seems wrong\n");
        exit(0);

     }

     ptr=ptr+strlen("Content-Length")+2;
     *file_size=atoll(ptr);

     return 1;

}
int Http_create_download_file(char *file_name, FILE **fp_download_file,int part){
    /**
     ** check argument error
     */
     if(file_name==NULL || fp_download_file==NULL || part<0){

        printf("Http_create_download_file: argument error\n");
        exit(0);
     }

     char buffer_for_part[max_download_thread+1];
     sprintf(buffer_for_part,"%d",part);
     int part_str_length=strlen(buffer_for_part);
     char *download_file_name=(char *)malloc((strlen(file_name)+5+part_str_length+1)*sizeof(char));
     if(NULL==download_file_name){

        printf("Http_create_download_file: malloc failed\n");
        exit(0);

     }
     strcpy(download_file_name,file_name);
     strcat(download_file_name,".part");
     strcat(download_file_name,buffer_for_part);

     if(access(download_file_name,F_OK)==0){
        int ret=remove(download_file_name);
        if(ret!=0){
            printf("Http_create_download_file: remove file failed\n");
            exit(0);
        }
     }

     *fp_download_file=fopen(download_file_name,"w+");
     if(NULL==*fp_download_file){
        printf("Http_create_download_file: fopen failed\n");
        exit(0);
     }


     if(download_file_name!=NULL){
        free(download_file_name);
     }

     return 1;
}

int Http_create_breakpoint_file(char *file_name, FILE **fp_breakpoint, long file_size, long num_of_part_file, long size_of_one_piece,
                                                                                       long total_num_of_piece_of_whole_file,
                                                                                       char *server_ip, int server_port){
    /**
     ** check argument error
     */
     if(file_name==NULL || fp_breakpoint==NULL){

        printf("Http_create_breakpoint_file: argument error\n");
        exit(0);
     }

     char *break_point_file_name=(char *)malloc((strlen(file_name)+4+1)*sizeof(char));
     if(NULL==break_point_file_name){

        printf("Http_create_breakpoint_file: malloc failed\n");
        exit(0);

     }
     strcpy(break_point_file_name,file_name);
     strcat(break_point_file_name,".jbp");

     if(access(break_point_file_name,F_OK)==0){
        int ret=remove(break_point_file_name);
        if(ret!=0){
            perror("Http_create_breakpoint_file,remove,\n");
            exit(0);
        }
     }

     *fp_breakpoint=fopen(break_point_file_name,"w+");
     if(NULL==*fp_breakpoint){
        printf("Http_create_breakpoint_file: fopen failed\n");
        exit(0);
     }

     unsigned char *break_point_buffer=(unsigned char *)malloc(sizeof(break_point)+1000);
     if(NULL==break_point_buffer){
            printf("Http_create_breakpoint_file: malloc failed\n");
            exit(0);
     }

     ((break_point *)break_point_buffer)->file_size=file_size;
     ((break_point *)break_point_buffer)->num_of_part_file=number_of_part_file;
     ((break_point *)break_point_buffer)->size_of_one_piece=size_of_one_piece;
     ((break_point *)break_point_buffer)->total_num_of_piece_of_whole_file=total_num_of_piece_of_whole_file;
     ((break_point *)break_point_buffer)->server_port=server_port;

     memcpy(((break_point *)break_point_buffer)->server_ip, server_ip, strlen(server_ip));
     ((break_point *)break_point_buffer)->server_ip[strlen(server_ip)]='\0';

     
     int ret_fwrite=fwrite(break_point_buffer,sizeof(break_point),1,*fp_breakpoint);
     fflush(*fp_breakpoint);

     if(ret_fwrite!=1){
            printf("Http_create_breakpoint_file: fwrite failed \n");
            exit(0);
     }

     if(break_point_file_name!=NULL){
        free(break_point_file_name);
     }

     return 1;
}

int Http_create_breakpoint_part_file(char *file_name, FILE **fp_breakpoint_part, int part_num, long start_num_of_piece_of_this_part_file,
                                                                                 long end_num_of_piece_of_this_part_file,
                                                                                 long size_of_last_incomplete_piece,
                                                                                 long alread_download_num_of_piece){
    if(file_name==NULL || fp_breakpoint_part==NULL || part_num<0){
        printf("Http_create_breakpoint_part_file, argument error\n");
        exit(0);
    }

    char buffer_for_part_num[6];
    sprintf(buffer_for_part_num, "%d",part_num);
    int part_num_str_len=strlen(buffer_for_part_num);
    char *break_point_part_file_name=(char *)malloc((strlen(file_name)+4+part_num_str_len+1)*sizeof(char));
    if(break_point_part_file_name==NULL){
        printf("Http_create_breakpoint_part_file,malloc failed\n");
        exit(0);
    }

    strcpy(break_point_part_file_name,file_name);
    strcat(break_point_part_file_name,".jbp");
    strcat(break_point_part_file_name,buffer_for_part_num);

    if(access(break_point_part_file_name,F_OK)==0){
        int ret=remove(break_point_part_file_name);
        if(ret!=0){
            perror("Http_create_breakpoint_part_file,remove");
            exit(0);
        }
    }

    *fp_breakpoint_part=fopen(break_point_part_file_name, "w+");
    if(*fp_breakpoint_part==NULL){
        printf("Http_create_breakpoint_part_file,fopen failed\n");
        exit(0);
    }

    break_point_of_part bpt;
    bpt.start_num_of_piece_of_this_part_file=start_num_of_piece_of_this_part_file;
    bpt.end_num_of_piece_of_this_part_file=end_num_of_piece_of_this_part_file;
    bpt.size_of_last_incomplete_piece=size_of_last_incomplete_piece;
    bpt.alread_download_num_of_piece=alread_download_num_of_piece;


    int ret=fwrite(&bpt, sizeof(break_point_of_part), 1, *fp_breakpoint_part);
    if(ret!=1){
        printf("Http_create_breakpoint_part_file,fwrite, break_point_of_part,error\n");
        exit(0);
    }

    
    fflush(*fp_breakpoint_part);


    if(break_point_part_file_name!=NULL){
        free(break_point_part_file_name);
    }

    return 0;

}


int Update_breakpoint_part_file(FILE *fp_breakpoint_part, int num_of_piece_tobe_added){

    if(fp_breakpoint_part==NULL || num_of_piece_tobe_added<1){
        printf("Update_breakpoint_part_file,argument error\n");
        exit(0);
    }

    break_point_of_part *bpt=(break_point_of_part *)malloc(sizeof(break_point_of_part));
    if(bpt==NULL){
        printf("Update_breakpoint_part_file,malloc failed\n");
        exit(0);
    }
    fseek(fp_breakpoint_part, 0, SEEK_SET);
    int ret_fread=fread(bpt, sizeof(break_point_of_part), 1, fp_breakpoint_part);

    int start_num=bpt->start_num_of_piece_of_this_part_file;
    int end_num=bpt->end_num_of_piece_of_this_part_file;
    bpt->alread_download_num_of_piece=bpt->alread_download_num_of_piece+num_of_piece_tobe_added;
    if((bpt->alread_download_num_of_piece)<=(bpt->end_num_of_piece_of_this_part_file-bpt->start_num_of_piece_of_this_part_file+1+1)){   
        fseek(fp_breakpoint_part, 0, SEEK_SET);
        int ret=fwrite(bpt, sizeof(break_point_of_part), 1, fp_breakpoint_part);
        if(ret!=1){
            printf("Update_breakpoint_part_file,fwrite failed\n");
            exit(0);
        }
        fflush(fp_breakpoint_part);
    }else{
        printf("Update_breakpoint_part_file, num_of_piece_tobe_added not correct\n");
        exit(0);
    }

}

int Delete_breakpoint_file(char *file_name,FILE *fp){

    if(file_name==NULL || fp==NULL){
        printf("Delete_breakpoint_file, argument error\n");
        exit(0);
    }

    break_point *bp=(break_point *)malloc(sizeof(break_point));
    fseek(fp, 0, SEEK_SET);
    int ret=fread(bp, sizeof(break_point), 1, fp);
    if(ret!=1){
        printf("Delete_breakpoint_file,fread failed\n");
        exit(0);
    }
    int num=bp->num_of_part_file;

    char *buffer=(char *)malloc((strlen(file_name)+4+100));
    for(int i=0;i<num;i++){

        char buffer_part[6];
        sprintf(buffer_part, "%d",i);
        strcpy(buffer,file_name);
        strcat(buffer,".jbp");
        strcat(buffer,buffer_part);

        if(access(buffer, F_OK)==0){
            if(remove(buffer)!=0){
                perror("Delete_breakpoint_file,remove .jbp_num");
                exit(0);
            }
        }

    }

    

    fclose(fp);
    strcpy(buffer, file_name);
    strcat(buffer, ".jbp");
    
    if(access(buffer, F_OK)==0){
        if(remove(buffer)!=0){
            perror("Delete_breakpoint_file,remove .jbp");
            exit(0);
        }
    }

    if(buffer!=NULL){
        free(buffer);
    }

    return 0;
}

int Http_check_breakpoint_file(char *link,FILE *fp){

    /**
     ** check argument
     */
     if(NULL==link || NULL==fp){
        printf("Http_check_breakpoint_file: argument error\n");
        exit(0);
     }


     fseek(fp,0,SEEK_END);
     long break_file_size=ftell(fp);

     fseek(fp,0,SEEK_SET);
     char *buffer_for_breakpoint_file=(char *)malloc(sizeof(char)*(break_file_size+1));
     if(NULL==buffer_for_breakpoint_file){
            printf("Http_check_breakpoint_file: malloc failed\n");
            exit(0);
     }

     int ret=fread(buffer_for_breakpoint_file,break_file_size,1,fp);
     if(ret!=1){
            printf("Http_check_breakpoint_file: fread failed\n");
            exit(0);
     }

     if(ret==0) return jbp_file_empty;

     int res=-1;
     if((break_file_size-sizeof(break_point))!=strlen(link)) return jbp_file_not_correct;
     buffer_for_breakpoint_file+=sizeof(break_point);
     for(;*link;link++,buffer_for_breakpoint_file++){
        if(*link!=*buffer_for_breakpoint_file)
            return jbp_file_not_correct;
     }

     return jbp_file_correct;

}

int Http_restore_orignal_file_name(char *download_part_file_name){
    /**
     ** check argument
     */
     if(download_part_file_name==NULL){
        printf("Http_restore_orignal_file_name: argument error\n");
        exit(0);
     }
     char *new_name=(char *)malloc(sizeof(char)*(strlen(download_part_file_name)+1));
     if(NULL==new_name){
        printf("Http_restore_orignal_file_name: malloc failed\n");
        exit(0);
     }

     strcpy(new_name,download_part_file_name);

     int file_length=strlen(new_name);
     for(int i=file_length;i>0;i--){
        if(new_name[i]=='.'){
            new_name[i]='\0';
            break;
        }
     }

     int ret=rename(download_part_file_name,new_name);
     if(ret!=0){
        printf("Http_restore_orignal_file_name: rename failed\n");
        exit(0);
     }

     return 1;


}

int Http_recv_file(int socket_fd, long long range_begin, long long range_end, unsigned char *buffer, long buffer_size,
                                                                                char *path, char *host_ip, char *port){
    /**
     ** check argument
     */
     if(range_begin<0 || range_end<0 || range_end<range_begin || NULL==buffer || buffer_size<1){
        printf("Http_recv_file: rename failed\n");
        exit(0);
     }

     char send_buffer[200];
     char buffer_range[100];
     sprintf(buffer_range, "\r\nRange: bytes=%lld-%lld",range_begin,range_end);

     sprintf(send_buffer,"GET %s",path);
     strcat(send_buffer," HTTP/1.1\r\n");
     strcat(send_buffer,"host: ");
     strcat(send_buffer,host_ip);
     strcat(send_buffer," : ");
     strcat(send_buffer,port);
     strcat(send_buffer,buffer_range);
     strcat(send_buffer, "\r\nKeep-Alive: 200");
     strcat(send_buffer,"\r\nConnection: Keep-Alive\r\n\r\n");

     int download_size=range_end-range_begin+1;
     
     int port_num=atoi(port);
     int ret0=send(socket_fd,send_buffer,strlen(send_buffer),0);
     if(ret0!=strlen(send_buffer)){
        printf("send failed, retry\n");
        perror("Http_recv_file");
        exit(0);
     }
		int recv_size=0;
		int length_of_http_head=0;
        while(1){
			
            long ret=recv(socket_fd,buffer+recv_size+length_of_http_head,buffer_size,0);
            if(ret<=0){
            	
                recv_size=0;
                length_of_http_head=0;
                memset(buffer,0,buffer_size);
                
                int ret=close(socket_fd);
                if(ret!=0){
                    perror("Http_recv_file");
                    exit(0);
                }
				
				//seems not need to sleep
				
                Http_connect_to_server(host_ip,port_num,&socket_fd);
                int ret0=send(socket_fd,send_buffer,strlen(send_buffer),0);
                if(ret0!=strlen(send_buffer)){
                    printf("send failed, retry\n");
                    perror("Http_recv_file");
                    exit(0);
                }

                continue;

            }

            if(recv_size==0){	 
                char *ptr=strstr(buffer,"Content-Length");
                if(ptr==NULL){
                    printf("Http_recv_file: recv buffer error\n%s\n",buffer);
                    exit(0);
                }
                int size=atoll(ptr+strlen("Content-Length")+2);
                if(size!=download_size){
                    printf("Http_recv_file: send recv not match\n");
                    exit(0);
                }
                
                char *ptr2=strstr(buffer,buffer_range+15);
                if(NULL==ptr2){
                	printf("Http_recv_file: expected range do not match recv range, %s\n%s\n",buffer,buffer_range+15);
                	exit(0);
                }

                char *ptr1=strstr(buffer,"\r\n\r\n");
                if(ptr1==NULL){
                    printf("Http_recv_file: http header not correct\n");
                    exit(0);
                }

                length_of_http_head=ptr1-(char*)buffer+4;              
                recv_size=recv_size+ret-length_of_http_head;
				
            }else{
            	recv_size+=ret;
            }      
            	
            if(recv_size==download_size){
                	break;
            }
			
        }

     return 1;
}

int Save_download_part_of_file(FILE *fp, unsigned char *buffer, long buffer_size, long long file_offset){
    /**
     ** check argument error
     */
     if(NULL==fp || NULL==buffer || buffer_size<1 || file_offset<0){
        printf("Save_download_part_of_file: argument error\n");
        exit(0);
     }


     fseek(fp,file_offset,SEEK_SET);
     int ret=fwrite(buffer,buffer_size,1,fp);
     if(ret!=1){
        printf("Save_download_part_of_file: fwrite failed\n");
        exit(0);
     }
     return 1;

}


int JHttp_download_one_piece(int socket_fd,  long long range_begin,long long range_end,FILE *fp_download,FILE *fp_jbp,
                            unsigned char *buffer,long buffer_size,long long offset,char *path, char *host_ip, char *port){

    /**
     ** check argument error
     ** sub func will check it, so we omit argument check here
     */

     Http_recv_file(socket_fd, range_begin, range_end, buffer,buffer_size,path,host_ip,port);
     unsigned char* ptr=strstr(buffer,"\r\n\r\n");
     if(ptr==NULL){
        printf("JHttp_download_one_piece: download file seems not correct\n");
        exit(0);
     }
     ptr+=4;
     Save_download_part_of_file(fp_download,ptr,range_end-range_begin+1,offset);
     Update_breakpoint_part_file(fp_jbp,1);

     return 1;
}

int JHttp_download_whole_file(char *link){
    /**
     ** check argument errorint Http_link_parse(char *link, char **url, char **port, char **path, char **file_name);
     */
     if(NULL==link){
        printf("JHttp_download_whole_file: argument error\n");
        exit(0);
     }

     char *url=NULL;
     char *port=NULL;
     char *path=NULL;
     char *file_name=NULL;

     Http_link_parse(link,&url,&port,&path,&file_name);

     char *ip_str=NULL;
     Http_get_ip_str_from_url(url,&ip_str);

     int socket_fd=-1;
     int port_int=atoi(port);
     Http_connect_to_server(ip_str,port_int,&socket_fd);

     long long file_size=0;
     Http_query_file_size(path,ip_str,port,socket_fd,&file_size);

     FILE *fp_breakpoint_file=NULL;
     int piece_num=file_size/(download_one_piece_size);
     int size_of_last_piece=file_size%(download_one_piece_size);
     Http_create_breakpoint_file(file_name,&fp_breakpoint_file,file_size,number_of_part_file,download_one_piece_size,piece_num,ip_str,port_int);

     FILE *fp_breakpoint_part_file=NULL;
     Http_create_breakpoint_part_file(file_name, &fp_breakpoint_part_file, 0, 1, piece_num, size_of_last_piece, 0);

     FILE *fp_download_file=NULL;
     Http_create_download_file(file_name,&fp_download_file,0);

     long buffer_size=(download_one_piece_size)+1000;//besides file content, server will also send http header
     unsigned char *buffer=(unsigned char *)malloc(sizeof(unsigned char)*buffer_size);
    
     if(NULL==buffer){
        printf("JHttp_download_whole_file: malloc failed\n");
        exit(0);
     }
    
     for(int i=1;i<=piece_num;i++){
		 
     	memset(buffer,0,buffer_size);
     	
        long range_begin=(i-1)*(download_one_piece_size);
        long range_end=i*(download_one_piece_size)-1;
		
        Http_recv_file(socket_fd,range_begin,range_end,buffer,buffer_size,path,ip_str,port);
		
        long long offset=(i-1)*(download_one_piece_size);
		
        char *ptr=strstr(buffer,"\r\n\r\n");
        if(NULL==ptr){
            printf("JHttp_download_whole_file:recv file seems not correct\n");
            exit(0);
        }
		
        ptr+=4;//pass \r\n\r\n
        
        Save_download_part_of_file(fp_download_file,ptr,(download_one_piece_size),offset);
        Update_breakpoint_part_file(fp_breakpoint_part_file, 1);
        
     }

     if(size_of_last_piece>0){
		 
     	memset(buffer,0,buffer_size);
     	
        long range_begin=piece_num*(download_one_piece_size);
        long range_end=range_begin+size_of_last_piece-1;
		
        Http_recv_file(socket_fd,range_begin,range_end,buffer,buffer_size,path,ip_str,port);
		
        long long offset=piece_num*(download_one_piece_size);
		
        char *ptr=strstr(buffer,"\r\n\r\n");
        if(NULL==ptr){
            printf("JHttp_download_whole_file:recv file seems not correct\n");
            exit(0);
        }
		
        ptr+=4;

        Save_download_part_of_file(fp_download_file,ptr,(range_end-range_begin+1),offset);
     }

     if(fclose(fp_download_file)!=0){
        printf("JHttp_download_whole_file:fclose file failed\n");
     }

     char *download_file_name_part=(char *)malloc(sizeof(char)*(strlen(file_name)+6+1));
     strcpy(download_file_name_part,file_name);
     strcat(download_file_name_part,".part0");

     Http_restore_orignal_file_name(download_file_name_part);
     Delete_breakpoint_file(file_name, fp_breakpoint_file);

     return 1;
}


int JHttp_download_whole_file_jbp(char *link){

    if(link==NULL){
        printf("JDFS_http_download: argument error\n");
        exit(0);
    }

    char *file_name=NULL;
    char *url=NULL;
    char *port=NULL;
    char *path=NULL;
    Http_link_parse(link, &url, &port, &path, &file_name);

   
    char *file_name_jbp=(char *)malloc((strlen(file_name))+4+1);
    if(file_name_jbp==NULL){
        printf("JDFS_http_download_jbp,malloc failed\n");
        exit(0);
    }
    strcpy(file_name_jbp,file_name);
    strcat(file_name_jbp,".jbp");
    FILE *fp=fopen(file_name_jbp, "r+");
    if(fp==NULL){
        printf("JDFS_http_download_jbp,fopen failed\n");
        exit(0);
    }

    break_point *bp=(break_point *)malloc(sizeof(break_point));

    if(bp==NULL){
        printf("JDFS_http_download_jbp,malloc failed\n");
        exit(0);
    }

    int ret=fread(bp, sizeof(break_point), 1, fp);
    if(ret!=1){
        printf("JDFS_http_download_jbp,fread failed\n");
        exit(0);
    }

    int num_of_part_file=bp->num_of_part_file;
    int size_of_one_piece=bp->size_of_one_piece;
    int total_piece=bp->total_num_of_piece_of_whole_file;

    char server_ip[128+1];
    strcpy(server_ip, bp->server_ip);
    int server_port=bp->server_port;

    int socket_fd=-1;
    Http_connect_to_server(server_ip, server_port, &socket_fd);


    long buffer_size=(size_of_one_piece)+1024;
    unsigned char *buffer=(unsigned char *)malloc(sizeof(unsigned char)*buffer_size);

    if(NULL==buffer){
        printf("JDFS_http_download: malloc failed\n");
        exit(0);
    }


    char *part_jbp_file_name=(char *)malloc(strlen(file_name)+100);
    char *part_file_name=(char *)malloc(strlen(file_name)+100);
    FILE *fp_jbp_part=NULL;
    FILE *fp_part=NULL;

    for(int i=0; i<number_of_part_file; i++){
        
        char temp_buffer[6];
        sprintf(temp_buffer, "%d",i);
        strcpy(part_jbp_file_name,file_name);
        strcat(part_jbp_file_name,".jbp");
        strcat(part_jbp_file_name,temp_buffer);

        strcpy(part_file_name, file_name);
        strcat(part_file_name,".part");
        strcat(part_file_name,temp_buffer);

        fp_jbp_part=fopen(part_jbp_file_name, "r+");
        fp_part=fopen(part_file_name,"r+");
        if(fp_part==NULL || fp_jbp_part==NULL){
            printf("JDFS_http_download_jbp,fopen failed\n");
            exit(0);
        }

        break_point_of_part bpt;
        int ret=fread(&bpt, sizeof(break_point_of_part), 1, fp_jbp_part);
        if(ret!=1){
            printf("JDFS_http_download_jbp,fread failed\n");
            exit(0);
        }

        long begin_piece=bpt.start_num_of_piece_of_this_part_file+bpt.alread_download_num_of_piece;
        long end_piece=bpt.end_num_of_piece_of_this_part_file;
        long size_of_last_incomplete_piece=bpt.size_of_last_incomplete_piece;
        
        for(int i=begin_piece;i<=end_piece;i++){

            memset(buffer, 0, buffer_size);
            long range_begin=(i-1)*size_of_one_piece;
            long range_end=i*(size_of_one_piece)-1;
            Http_recv_file(socket_fd, range_begin, range_end, buffer, buffer_size, path, server_ip, port);
            long offset=(i-1)*(download_one_piece_size);

            char *ptr=strstr(buffer,"\r\n\r\n");
            if(NULL==ptr){
                printf("JHttp_download_whole_file:recv file seems not correct\n");
                exit(0);
            }
        
            ptr+=4;//pass \r\n\r\n
        
            Save_download_part_of_file(fp_part,ptr,(download_one_piece_size),offset);
            Update_breakpoint_part_file(fp_jbp_part, 1);
                       

        }

        if(size_of_last_incomplete_piece>0){

            memset(buffer, 0, buffer_size);
            long range_begin=total_piece*size_of_one_piece;
            long range_end=range_begin+size_of_last_incomplete_piece-1;
            Http_recv_file(socket_fd, range_begin, range_end, buffer, buffer_size, path, server_ip, port);

            long offset=total_piece*size_of_one_piece;

            char *ptr=strstr(buffer,"\r\n\r\n");
            if(NULL==ptr){
                printf("JHttp_download_whole_file:recv file seems not correct\n");
                exit(0);
            }
        
            ptr+=4;//pass \r\n\r\n
        
            Save_download_part_of_file(fp_part,ptr,(download_one_piece_size),offset);
            Update_breakpoint_part_file(fp_jbp_part, 1);
                
        }

        fclose(fp_jbp_part);    
        
    }

    if(buffer!=NULL){
        free(buffer);
    }

    char *download_file_name_part=(char *)malloc(sizeof(char)*(strlen(file_name)+6+1));
    strcpy(download_file_name_part,file_name);
    strcat(download_file_name_part,".part0");

    Http_restore_orignal_file_name(download_file_name_part);

    Delete_breakpoint_file(file_name, fp);

    return 0;

}