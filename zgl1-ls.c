#include<stdio.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<time.h>
#include <pwd.h>
#include<grp.h>
#include<locale.h>
#include<sys/ioctl.h>
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
int flag_DIR=0;
int color_on;
int width=-1;
void print_s(struct entry* en);
void print_i(struct entry* en);
void print_(char** p,int index);
void print_name(struct entry* en,int len_max);
void collect(struct entry** arr,int* count,struct stat* sta,char* name);
void swap_sta(struct stat** p);
void swap_t(struct entry** arr,int count);
void swap_r(struct entry** arr,int count);
void print_l(struct entry* en,char* str);
void mode_to_string(mode_t mode, char str[11]);
void swap_name(struct entry** en,int count);
void color_init(void);
char *style_of_mode(mode_t m);
int max_len(struct entry** en,int count);
int main(int argc,char* argv[])
{
    if(isatty(STDOUT_FILENO))
    {
        struct winsize win;
        if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&win)!=-1&&win.ws_col>0)
        width=win.ws_col;
    }
    if(width<0)
    {
        char* env=getenv("COLUMNS");
        int c=0;
        if(env!=NULL)
        c=atoi(env);
        if(c)
        width=c;
        else
        width=80;
    }
    setlocale(LC_COLLATE,"zh_CN.UTF-8");
    color_init();
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
    if(index==0&&flag_DIR==0)
    {
        index=1;
        p[0]=".";
    }
    if(index==0&&flag_DIR==1)
    return;
        for(int i=0;i<index;i++)
        {
            char** str_DIR=NULL;
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
            int count_DIR=0;
            while((r=readdir(dir))!=NULL)
            {
                char path[4096];
                if(p[i][strlen(p[i])-1]=='/')
                snprintf(path, sizeof(path),"%s%s",p[i],r->d_name);
                else
                snprintf(path, sizeof(path),"%s/%s",p[i],r->d_name);
                struct stat sta;
                if(opt_res.res_a==0)
                {
                    if(r->d_name[0]!='.')
                    {
                        if(lstat(path, &sta)==-1)
                        continue;
                    }
                        
                    else
                    continue;
                }
                else{
                if(lstat(path, &sta)==-1)
                continue;
                }
            
            collect(&list,&count,&sta,r->d_name);
        }
            swap_name(&list,count);
            swap_t(&list,count);
            swap_r(&list,count);
            int len_s=0,len_i=0;
            if(opt_res.res_i==1)
            len_i=9;
            if(opt_res.res_s)
            len_s=3;
            int len_max=max_len(&list,count);
            int col=width/(len_max+1+len_i+len_s);
            if(col<1)
            col=1;
            for(int j=0;j<count;j++)
            {
                print_i(&list[j]);
                print_s(&list[j]);
                if(opt_res.res_l==1)
                {
                    char str[11]={0};
                    mode_to_string(list[j].st.st_mode,str);
                    print_l(&list[j],str);
                }                
                print_name(&list[j],len_max);
                if(opt_res.res_l==1&&j!=count-1)
                {
                    printf("\n");
                }
                if(opt_res.res_l==0)
                {
                   if((j+1)%col==0)
                   printf("\n");
                }
            }
            closedir(dir);
            if(index>1&&opt_res.res_R==0&&i!=index-1)
            printf("\n");
            if(opt_res.res_R==1)
            printf("\n");
            for(int j=0;j<count;j++){
            if(S_ISDIR(list[j].st.st_mode)&&opt_res.res_R==1)
            {
                char path_DIR[4096];
                if(strcmp(list[j].name,".")==0||strcmp(list[j].name,"..")==0)
                continue;
                if(p[i][strlen(p[i])-1]=='/')
                 snprintf(path_DIR, sizeof(path_DIR),"%s%s",p[i],list[j].name);
                 else
                 snprintf(path_DIR, sizeof(path_DIR),"%s/%s",p[i],list[j].name);
                if(opt_res.res_a==0)
                {
                    if(list[j].name[0]=='.')
                    continue;
                }
                flag_DIR=1;
                count_DIR++;
                char** pp_DIR=realloc(str_DIR,(count_DIR)*sizeof(char*));
                if(pp_DIR==NULL)
               break;
                str_DIR=pp_DIR;
                str_DIR[count_DIR-1]=strdup(path_DIR);
            }
        }
            if(opt_res.res_R==1){
            flag_DIR=1;
            print_(str_DIR,count_DIR);
            for(int i=0;i<count_DIR;i++)
            free(str_DIR[i]);
            free(str_DIR);
            }
        }
}
void print_name(struct entry* en,int len_max)
{
    char* c=style_of_mode(en->st.st_mode);
    if(c!=NULL)
    printf("%s",c);
    if(opt_res.res_a==1)
    {
            printf("%-*s",len_max+1,en->name);
    }
    else{
            if(en->name[0]!='.')
        printf("%-*s",len_max+1,en->name);
    }
    if(c!=NULL)
    printf("%s","\033[0m");
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
   (*arr)[*count].name=strdup(name);
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
    if (S_ISREG(mode)) str[0] = '-';
    else if (S_ISDIR(mode)) str[0] = 'd';
    else if (S_ISCHR(mode)) str[0] = 'c';
    else if (S_ISBLK(mode)) str[0] = 'b';
    else if (S_ISFIFO(mode))str[0] = 'p';
    else if (S_ISLNK(mode)) str[0] = 'l';
    else if (S_ISSOCK(mode))str[0] = 's';
    else  str[0] = '?';
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
        if(pw!=NULL)
        {
            int len1=strlen(pw->pw_name);
        printf("%-*s",len1+1,pw->pw_name);
        }
        else
        printf("NUKNOWN");
        struct group* gp=getgrgid(en->st.st_gid);
        if(gp!=NULL)
        {
            int len2=strlen(gp->gr_name);
        printf("%-*s",len2+1,gp->gr_name);
        }
        else
         printf("NUKNOWN");
        printf("%6ld ",en->st.st_size);
        char buf[30]={0};
        struct tm time;
        localtime_r(&(en->st.st_mtim.tv_sec),&time);
        strftime(buf,30,"%-m月 %e %H:%M ",&time);
        printf("%s",buf);
}
void swap_name(struct entry** en,int count)
{
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
void color_init(void)
{
    const char *term = getenv("TERM");
    if (getenv("NO_COLOR"))                     color_on = 0;
    else if (term && !strcmp(term, "dumb"))     color_on = 0;
    else                                        color_on = isatty(STDOUT_FILENO);
}
char *style_of_mode(mode_t m)
{
    if (S_ISDIR(m))               return "\033[1;34m";
    if (S_ISLNK(m))               return "\033[1;36m";
    if (S_ISFIFO(m))              return "\033[33m";
    if (S_ISCHR(m) || S_ISBLK(m)) return "\033[1;33m";
    if (m & 0111)                 return "\033[1;32m";
    return NULL;
}
int max_len(struct entry** en,int count)
{
    int max=0;
    for(int i=0;i<count;i++)
    {
        int len=strlen((*en)[i].name);
        if(len>max)
        max=len;
    }
    return max;
}
