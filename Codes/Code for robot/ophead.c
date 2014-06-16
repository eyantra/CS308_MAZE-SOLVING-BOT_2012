#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
LBR = B
LBS = R
RBL = B
SBL = R
SBS = B
LBL =  S
*/

//int string_length(char *s)
//{
//   int c = 0;
//
//   while(s[c]!='\n')
//      c++;
//
//   return c;
//}

int check(char* s){
    int i=0;
    int len;
    len = strlen(s);
    if(strcmp(s,"B")==0||len<3) return 1;
    while(s[i]!='\n'){
        if(s[i]=='B') return 0;
        i++;
    }
    return 1;
}

char* reduced(char* oldpath){
    char* str;
    str = (char*)malloc(sizeof(char));
    str = strdup(oldpath);
    char* strcheck;
    strcheck = (char*)malloc(sizeof(char));
    strcheck = strdup(str);
    int len;
    len = strlen(str);//sizeof(str)/sizeof(char);
    char* temp;
    temp = (char*)malloc(len*sizeof(char));
    char* k;
    k = (char*)malloc(2*sizeof(char));
            strcat(temp,"");

    while(len >=3){

        if(strncmp(str,"LBR",3)==0){
            str+=3;
            strcat(temp,"B");
        }

        else if(strncmp(str,"LBS",3)==0){
            str+=3;
            strcat(temp,"R");
        }

        else if(strncmp(str,"RBL",3)==0){
            str+=3;
            strcat(temp,"B");
        }

        else if(strncmp(str,"SBL",3)==0){
            str+=3;
            strcat(temp,"R");
        }

        else if(strncmp(str,"SBS",3)==0){
            str+=3;
            strcat(temp,"B");
        }

        else if(strncmp(str,"LBL",3)==0){
            str+=3;
            strcat(temp,"S");
        }

        else{
            k[0] = str[0];
            strcat(temp,k);
            str++;
        }
        len--;

    }

    if(check(temp)==1) return temp;
    else{
	   strcat(temp,str);
        return reduced(temp);
    }
}


