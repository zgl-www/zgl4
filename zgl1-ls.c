#include<stdio.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<stdbool.h>
#include<time.h>
#include <pwd.h>
#include<grp.h>
#include<locale.h>
typedef struct res_{
    int res_l;
    int res_s;
    int res_R;
    int res_r;
    int res_t;
    int res_a;
    int res_i;
    }res;
res opt_res={0};
struct entry {
    struct stat st;
    char* name;
};
void print_s(struct entry* en);
void print_i(struct entry* en);
void print_(char** p,int index);
void print_name(struct entry* en,struct res_* res_name);
void collect(struct entry** arr,int* count,struct stat* sta,char* name);
void swap_sta(struct stat** p);
void swap_t(struct entry** arr,int count);
void swap_r(struct entry** arr,int count);
void print_l(struct entry* en,char* str);
void mode_to_string(mode_t mode, char str[11]);
void swap_l(struct entry** en,int count);
int main(int argc,char* argv[])
{
    setlocale(LC_COLLATE,"zh_CN.UTF-8");
    int opt;
     char** p=(char**)malloc(sizeof(char*)*20);
     int index=0;
    while((opt=getopt(argc,argv,"-laRrtsi"))!=-1)
    {
        if(opt==1)
        {
           p[index]=(char*)malloc(sizeof(char)*100);
           strcpy(p[index],optarg);
           index++;
        }
        else
        {
            switch((char)opt)
            {
                case'l':opt_res.res_l=1;break;
                case'a':opt_res.res_a=1;break;
                case't':opt_res.res_t=1;break;
                case'R':opt_res.res_R=1;break;
                case'r':opt_res.res_r=1;break;
                case'i':opt_res.res_i=1;break;
                case's':opt_res.res_s=1;break;
                default:break;
            }
        }
    }
    print_(p,index);
        return 0;
}
void print_s(struct entry* en)
{
    if(opt_res.res_s==1)
    printf("%-3d",(int)(en->st).st_blocks/2);
}
void print_i(struct entry* en)
{
    if(opt_res.res_i==1)
    printf("%-9d", (int)(en->st).st_ino);
}
void print_(char** p,int index)
{
    if(index==0)
    {
        index=1;
        p[0]=".";
    }
        for(int i=0;i<index;i++)
        {
            printf("%s:\n",p[i]);
            DIR* dir=opendir(p[i]);
            if (dir == NULL) {
               perror(p[i]);
                 continue;
                      }
            int opt;
            struct dirent* r;
            int count=0;
            struct entry* list=NULL;
            while((r=readdir(dir))!=NULL)
            {
                char path[4096];
                snprintf(path, sizeof(path),"%s/%s",p[i],r->d_name);
                  struct stat sta;
                if(opt_res.res_a==0)
                {
                    if(r->d_name[0]!='.')
                    lstat(path, &sta);
                    else
                    continue;
                }
                else{
                lstat(path, &sta);
                }
            collect(&list,&count,&sta,r->d_name);
            }
            swap_t(&list,count);
            swap_r(&list,count);
            swap_l(&list,count);
            for(int i=0;i<count;i++)
            {
                if(opt_res.res_l==0){
                if(i%4==0&&i!=0)
                printf("\n");
                }
                print_i(list);
                print_s(list);
                if(opt_res.res_l==1)
                {
                    char str[11]={0};
                    mode_to_string(list->st.st_mode,str);
                    print_l(list,str);
                }                
                print_name(list,&opt_res);
                if(opt_res.res_l==1&&i!=count-1)
                {
                    printf("\n");
                }
                list++;
            }
            closedir(dir);
            printf("\n");
        }
}
void print_name(struct entry* en,struct res_* res_name)
{
    if(res_name->res_a==1)
    {
            printf("%-*s",17,en->name);
    }
    else{
            if(en->name[0]!='.')
        printf("%-*s",17,en->name);
    }
}
void collect(struct entry** arr,int* count,struct stat* sta,char* name)
{
    struct entry* stm=(struct entry*)realloc(*arr,(*count+1)*sizeof(struct entry));
    if(stm==NULL)
    { 
        printf("error\n");
        return;
    }
   *arr=stm;
   (*arr)[*count].st=*sta;
   (*arr)[*count].name=name;
   (*count)++;
}
void swap_t(struct entry** arr,int count)
{
      if(opt_res.res_t==1)
      {
         for(int i=0;i<count-1;i++)
         {
            for(int j=i+1;j<count;j++)
            {
                if((*arr)[i].st.st_mtim.tv_sec<(*arr)[j].st.st_mtim.tv_sec||((*arr)[i].st.st_mtim.tv_sec==(*arr)[j].st.st_mtim.tv_sec&&(*arr)[i].st.st_mtim.tv_nsec<(*arr)[j].st.st_mtim.tv_nsec))
                {
                        struct entry temp=(*arr)[i];
                        (*arr)[i]=(*arr)[j];
                        (*arr)[j]=temp;
                }
            }
         }
      }
}
void swap_r(struct entry** arr,int count)
{
    if(opt_res.res_r==1)
    {
        for(int i=0;i<count/2;i++)
        {
            struct entry temp=(*arr)[i];
            (*arr)[i]=(*arr)[count-1-i];
            (*arr)[count-1-i]=temp;
        }
    }
}
void mode_to_string(mode_t mode, char str[11])
{
    str[0]=S_ISREG(mode)?'-':'d';
    str[1]=((S_IRUSR&mode)==256)?'r':'-';
    str[2]=((S_IWUSR&mode)==128)?'w':'-';
    str[3]=((S_IXUSR&mode)==64)?'x':'-';
    str[4]=((S_IRGRP&mode)==32)?'r':'-';
    str[5]=((S_IWGRP&mode)==16)?'w':'-';
    str[6]=((S_IXGRP&mode)==8)?'x':'-';
    str[7]=((S_IROTH&mode)==4)?'r':'-';
    str[8]=((S_IWOTH&mode)==2)?'w':'-';
    str[9]=((S_IXOTH&mode)==1)?'x':'-';
    str[10]='\0';
}
void print_l(struct entry* en,char* str)
{
        printf("%-11s",str);
        printf("%-3lu",en->st.st_nlink);
        struct passwd *pw=getpwuid(en->st.st_uid);
        int len1=strlen(pw->pw_name);
        printf("%-*s",len1+1,pw->pw_name);
        struct group* gp=getgrgid(en->st.st_gid);
        int len2=strlen(gp->gr_name);
        printf("%-*s",len2+1,gp->gr_name);
        printf("%6ld ",en->st.st_size);
        char buf[30]={0};
        struct tm time;
        localtime_r(&(en->st.st_mtim.tv_sec),&time);
        strftime(buf,30,"%-m月 %e %H:%M ",&time);
        printf("%s",buf);
}
void swap_l(struct entry** en,int count)
{
    if(opt_res.res_l==1){
    for(int i=0;i<count-1;i++)
    {
         for(int j=i+1;j<count;j++)
         {
            if(strcoll((*en)[i].name,(*en)[j].name)>0)
            {
                struct entry temp=(*en)[i];
                (*en)[i]=(*en)[j];
                (*en)[j]=temp;
            }
         }
    }
}
}

