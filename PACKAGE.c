#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
void* NTS(void*);
void* STN(void*);
pthread_mutex_t mutex,nts,stn,mutex1;
int ntscnt,stncnt;
int ntscyc,stncyc;
int onnts,onstn;
time_t start;
int VILLCNT,STARVCNT;
void display(int id,char* str)
{
        pthread_mutex_lock(&mutex);
        for(int i=0;i<VILLCNT;i++)
        {
                if(id==-1)
                {
                        printf("Vill. %d     |",i+1);
                }
                else
                {
                        if(i==id-1)
                        {
                                printf("%8s    |",str);
                        }
                        else
                        {
                                printf("            |");
                        }
                }
        }
        printf("%ld sec\n",time(NULL)-start);
        pthread_mutex_unlock(&mutex);
}
void* NTS(void* id)
{
        display(*(int*)id,"Appeared");
        display(*(int*)id,"N to S");
        display(*(int*)id,"Waiting");
        pthread_mutex_lock(&nts);
        display(*(int*)id,"Got Perm");
        pthread_mutex_lock(&mutex);
        ntscnt++;
        ntscyc++;
        if(ntscnt==1)
                pthread_mutex_trylock(&stn);
        if(ntscyc!=STARVCNT)
                pthread_mutex_unlock(&nts);
        else
                ntscyc=0;
        pthread_mutex_unlock(&mutex);
        display(*(int*)id,"Crossing");
        pthread_mutex_lock(&mutex1);
        onnts++;
        while(onnts>1);
        pthread_mutex_unlock(&mutex1);
        sleep((rand()%5)+2);
        display(*(int*)id,"Crossed!");
        pthread_mutex_lock(&mutex);
        ntscnt--;
        onnts--;
        if(ntscnt==0)
                pthread_mutex_unlock(&stn);
        pthread_mutex_unlock(&mutex);
        sleep(2);
        if(!pthread_mutex_trylock(&stn))
        {
                pthread_mutex_unlock(&nts);
                pthread_mutex_unlock(&stn);
        }
        pthread_exit(NULL);
}
void* STN(void* id)
{
        display(*(int*)id,"Appeared");
        display(*(int*)id,"S to N");
        display(*(int*)id,"Waiting");
        pthread_mutex_lock(&stn);
        display(*(int*)id,"Got Perm");
        pthread_mutex_lock(&mutex);
        stncnt++;
        stncyc++;
        if(stncnt==1)
                pthread_mutex_trylock(&nts);
        if(stncyc!=STARVCNT)
                pthread_mutex_unlock(&stn);
        else
                stncyc=0;
        pthread_mutex_unlock(&mutex);
        display(*(int*)id,"Crossing");
        pthread_mutex_lock(&mutex1);
        onstn++;
        while(onstn>1);
        pthread_mutex_unlock(&mutex1);
        sleep((rand()%5)+2);
        display(*(int*)id,"Crossed!");
        pthread_mutex_lock(&mutex);
        stncnt--;
        onstn--;
        if(stncnt==0)
                pthread_mutex_unlock(&nts);
        pthread_mutex_unlock(&mutex);
        sleep(2);
        if(!pthread_mutex_trylock(&nts))
        {
                pthread_mutex_unlock(&stn);
                pthread_mutex_unlock(&nts);
        }
        pthread_exit(NULL);
}
int main(int argc,char *argv[])
{
        int n1=(rand()%3)+2;
        int n2=(rand()%3)+2,STARVCNT=3;
        if(argc==1)
        {
                VILLCNT=n1+n2;
        }
        else if(argc==2)
        {
                STARVCNT=atoi(argv[1]);
                VILLCNT=n1+n2;
        }
        else if(argc==3)
        {
                n1=atoi(argv[1]);
                n2=atoi(argv[2]);
                VILLCNT=n1+n2;
        }
        else if(argc==4)
        {
                n1=atoi(argv[1]);
                n2=atoi(argv[2]);
                STARVCNT=atoi(argv[3]);
                VILLCNT=n1+n2;
        }
        else
        {
                printf("Invalid number of arguments\n");
                exit(0);
        }
        start=time(NULL);
        pthread_t vill[VILLCNT];
        ntscnt=0;
        stncnt=0;
        ntscyc=0;
        stncyc=0;
        onnts=0;
        onstn=0;
        int id[VILLCNT];
        int t;
        pthread_mutex_init(&mutex,NULL);
        pthread_mutex_init(&nts,NULL);
        pthread_mutex_init(&stn,NULL);
        pthread_mutex_init(&mutex1,NULL);
        display(-1,"");
        for(int i=0;i<VILLCNT;i++)
        {
                void* (*villgen)(void*);
                id[i]=i+1;
                t=rand()%2;
                if(t==1 && n1!=0)
                {
                        villgen=&NTS;
                        n1--;
                }
                else if(t==0 && n2!=0)
                {
                        villgen=&STN;
                        n2--;
                }
                else if(t==0 && n1!=0)
                {
                        villgen=&NTS;
                        n1--;
                }
                else if(t==1 && n2!=0)
                {
                        villgen=&STN;
                        n2--;
                }
                sleep(4);
                pthread_create(&vill[i],NULL,villgen,(&id[i]));
        }
        pthread_exit(NULL);
        exit(0);
}
