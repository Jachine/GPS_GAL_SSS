// Gps_for_MFC.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "do_files.h"
#include "do_util.h"
#include "do_nav.h"
#include "do_signal.h"

#define SCENE_PATH "scenario\\GNSS.scn"
#define RINEX_TAIL ".rinex"
#define PARAS_TOTAL 14
G_PARAS g_paras;

void TcharToChar (const TCHAR * tchar, char * _char)  
{  
    int iLength ;  
//��ȡ�ֽڳ���   
iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
//��tcharֵ����_char    
WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
} 

void CharToTchar (const char * _char, TCHAR * tchar)  
{  
    int iLength ;  
  
    iLength = MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, NULL, 0) ;  
    MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, tchar, iLength) ;  
} 
int prase_scenario(const char *dir,const char *filename){
	FILE *fp;
    int sat_id,sat_num=0;
    int i,index;
	int time[3]={0};
    char *s,*p,*p1,*id;
    char line[MAX_BUF];
	char file_path[MAX_PATH];
	char local_paras[16][256];
	strcpy(file_path,dir);
	strcat(file_path,filename);
    if (!(fp=fopen(file_path,"r"))) {
        fprintf(stderr,"e1b file open error : %s\n",file_path);
        return -1;
    }
	 while (!feof(fp) && fgets(line, LINE_BUF_LEN, fp)){
        s = line;
        if (s[strlen(s) - 1] == '\n')//change the line end to string end
            s[strlen(s) - 1] = '\0';
		if(strncmp(s,"#G",2)==0){
            p=s;
            index=0;
            if((p1 = strtok(p, ","))) {
                //sscanf(p1,"%d",local_paras+index);
				//index++;
            }
            while((p1 = strtok(NULL, ","))) {
                //sscanf(p1,"%d",local_paras+index);
                strcpy(local_paras[index],p1);
				printf("paras = %s\n", p1);
				index++;
            }
            if(index!=PARAS_TOTAL){printf("Error: Raw Scene Data File is not right \n");}
		}
	 }
	strcpy(g_paras.Sig_Path,local_paras[2]);
	strcpy(g_paras.Nav_Path,local_paras[3]);
	if((p1 = strstr(local_paras[3], ".txt"))){
	*p1=0;
	strcat(local_paras[3],RINEX_TAIL);
	strcpy(g_paras.Rinex_Path,local_paras[3]);
	}
	if(strstr(local_paras[0],"S")){
		g_paras.mode=STATIC_MODE;
		 if((p1 = strtok(local_paras[7], "N"))){
			sscanf(p1,"%lf",g_paras.static_user_pos+0);
		 }
		 if((p1 = strtok(local_paras[8], "E"))){
			sscanf(p1,"%lf",g_paras.static_user_pos+1);
		 }
		 if((p1 = strtok(local_paras[9], "m"))){
			sscanf(p1,"%lf",g_paras.static_user_pos+2);
		 }
		 p1=local_paras[10];
		 sscanf(p1,"%2d:%2d:%2d",time,time+1,time+2);
		 printf("time: %2d:%2d:%2d\n",time[0],time[1],time[2]);
		 return time[1];
	 }
	 else{
		g_paras.mode=DYN_MODE;
		p1=local_paras[7];
		sscanf(p1,"%2d:%2d:%2d",time,time+1,time+2);
		//change [11] to [10], add while to deal with the ; not , ... so strange 
		if((p1 = strtok(local_paras[10], ";"))){
			strcpy(g_paras.Kml_Path,p1);
			while((p1 = strtok(NULL, ";")))
				strcpy(g_paras.Kml_Path,p1);
		}
		else{
			printf("ERROR: scn kml not found or file wrong \n");
			exit(-1);
		}
			
		return time[1];
	 }
}
int _tmain(int argc, _TCHAR* argv[])
{
	int mins=0;
	printf("Gps starts \n");
	char local_path[MAX_PATH];
	TCHAR szFilePath[MAX_PATH];
	//���exe��ǰ·��
    GetModuleFileName(NULL, szFilePath, MAX_PATH);     
    (_tcsrchr(szFilePath, _T('\\')))[0] = 0;
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	//Disgusting Tchar to Char
	TcharToChar(szFilePath,local_path);
	//���������ļ����õ��ź�ʱ��
	mins = prase_scenario(local_path,SCENE_PATH);
	//prase_mkl("G:\\Domon_Work\\WangLei_V6.5\\Release\\trajectory\\input.kml",10,NULL);
	//printf("path == %s\n",local_path);
	//Sleep(5000);
	if(mins<=0)mins=1;
    main_signal(mins, 1);//mins == how long nav data, 1=simple way,0=common way
    return 0;
}

