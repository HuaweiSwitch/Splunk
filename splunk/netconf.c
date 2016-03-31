#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "libnetconf.h"

char * g_password = NULL;

int savepsw(char * password)
{
    if( g_password != NULL )
    {
        free(g_password);
        g_password = NULL;
    }

    g_password = malloc( strlen(password)+1 );
    if( g_password == NULL )
    {
        return -1;
    }
    memset( g_password, 0, strlen(password)+1 );

    memcpy( g_password, password, strlen(password) );

    return 0;
}

int parseCmd(int argc, char *argv[], char * ipaddr, unsigned short * port, char *username, char *password, char *infilename, char *outfilename)
{
    char portStr[32] = {0};
    
    if (13 != argc 
        || strcmp(argv[1],  "-ip") 
        || strcmp(argv[3],  "-port") 
        || strcmp(argv[5],  "-user") 
        || strcmp(argv[7],  "-psw")
        || strcmp(argv[9],  "-in")
        || strcmp(argv[11], "-out") )
    {
        return -1;
    }
    
    strcat(ipaddr,      argv[2]);
    strcat(portStr,     argv[4]);
    strcat(username,    argv[6]);
    strcat(password,    argv[8]);
    strcat(infilename,  argv[10]);
    strcat(outfilename, argv[12]);

    (*port) = atoi(portStr);

    if( savepsw(password) != 0 )
    {
        return -1;
    }
    
    return 0;
}

char * getsenddata(char * filename)
{
    FILE  *fp = NULL;
    int filelen = 0;
    char * pbuf = NULL;

    fp = fopen(filename, "r");
    if( fp == NULL )
    {
        return NULL;
    }

    fseek(fp,0,SEEK_END);
    filelen = ftell(fp);
    rewind(fp);

    pbuf = malloc(filelen+1);
    if( pbuf == NULL )
    {
        fclose(fp);
        return NULL;
    }
    memset( pbuf, 0, filelen+1 );

    fread( pbuf, 1, filelen, fp );
    pbuf[filelen] = 0;

    fclose(fp);

    return pbuf;
}

void savedata(char * filename, char * data)
{
    FILE  *fp = NULL;
    struct tm *gm_date;
    time_t seconds;
    char timeStr[20] = {0};

    fp = fopen(filename, "a");
    if( fp == NULL )
    {
        return;
    }

    /* 先要写下时间戳 */
    
    time(&seconds);   
    gm_date = gmtime(&seconds);

    sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d", 
        gm_date->tm_year+1900, gm_date->tm_mon, gm_date->tm_mday, 
        gm_date->tm_hour, gm_date->tm_min, gm_date->tm_sec );

    fputs(timeStr, fp);
    fputs("\n  ", fp);
    
    fputs(data, fp);
    fputs("\n\n", fp);

    fclose(fp);

    return;
}

char* callback_sshauth_password_set (const char* username,
        const char* hostname)
{
    return g_password;
}

#define LIBSSH2_SESSION  char
int callback_ssh_hostkey_check_set (const char* hostname, LIBSSH2_SESSION *session)
{
    return 0;    
}

int main(int argc, char *argv[])
{
    unsigned short port   = 0;
    char host[32]         = {0};
    char user[128]        = {0};
    char password[128]    = {0};
    char infilename[128]  = {0};
    char outfilename[128] = {0};

    struct nc_cpblts  * cpblts;
    struct nc_session * session;
    nc_rpc   * rpc   = NULL;
    nc_reply * reply = NULL;
    NC_REPLY_TYPE reply_type;
    
    char  *send_data    = NULL;
    char  *send_point   = NULL;
    char  *data_pointer = NULL;

    /* 先获取输入参数 */
    if( parseCmd(argc, argv, host, &port, user, password, infilename, outfilename) != 0 )
    {
        printf("Command paramt error.\n");
        return -1;
    }
    
    /* 设置回调，不需要用户确认 */
    nc_callback_ssh_host_authenticity_check(callback_ssh_hostkey_check_set);
    /* 设置回调，密码从配置文件读取 */
    nc_callback_sshauth_password(callback_sshauth_password_set);
    
    cpblts = nc_session_get_cpblts_default();

    session = nc_session_connect(host, port, user, cpblts);
    if (session == NULL) 
    {
        printf("Connecting to the NETCONF server failed.\n");
        return  -1;
    }
    else
    {
        printf("Connecting to the NETCONF server Succeed.\n");
    }

    send_data = getsenddata(infilename);
    if( send_data == NULL )
    {
        printf("get send data fail");
        return -1;
    }

    rpc = nc_rpc_generic(send_data);
    free(send_data);

    send_point = nc_rpc_dump(rpc);
    printf("send: %s\n", send_point);

    nc_session_send_recv(session, rpc, &reply);
    data_pointer = nc_reply_get_data(reply);
    printf("data: %s\n", data_pointer);
    
    savedata(outfilename, data_pointer);

    nc_rpc_free(rpc);
    nc_reply_free(reply);

    /* close NETCONF session */
    nc_session_free(session);

    printf("Send to NETCONF server and receive reply success.\n");

    return 0;
}

