/*二维细胞生成机代码
 	Author:tybitsfox	2014-6-24
 */
#include"clsscr.h"
#include<curses.h>

#define		XX			100
#define		YY			40
#define		nul			' '
#define		star		'*'
#define		shu			'|'
#define 	heng		'-'
#define		timz		200000
#define		seed_num	XX*YY/10
char a[YY][XX],b[YY][XX];
int  flag=0;
int	 ev_counts=0;			//进化次数
int	 lv_counts=0;			//生存数量
int  rep=0;					//重复次数。
int  lst[10]={0};			//最后10次的生存数量
int  head;					//list的头指针

//{{{ void pump()
void pump()
{
	struct timeval tm;
	tm.tv_sec=0;
	tm.tv_usec=timz;
	select(0,NULL,NULL,NULL,&tm);
}//}}}
//{{{ int initdata()
int initdata()
{
	int i,j,k;
	if(flag!=2)
	{
		for(i=0;i<XX;i++)
		{
			for(j=0;j<YY;j++)
			{
				a[j][i]=nul;
				b[j][i]=nul;
			}
		}
	}
	ev_counts=lv_counts=0;
	rep=0;head=0;
	srand(time(0));
	for(j=0;j<=YY+1;j++)
	{
		move(j,9);
		echochar(shu);
		move(j,XX+10);
		echochar(shu);
		refresh();
	}
	for(i=10;i<XX+10;i++)
	{
		move(0,i);
		echochar(heng);
		move(YY+1,i);
		echochar(heng);
		refresh();
	}
	return 0;
}//}}}
//{{{ void seed()
void seed()
{
	int i,j,k,m,n;
	char *c,name[128];
	for(i=0;i<YY;i++)
	{
		for(j=0;j<XX;j++)
		{
			a[i][j]=nul;
			b[i][j]=nul;
		}
	}
	i=rand()%seed_num+seed_num;
	for(j=0;j<i;j++)
	{
		k=rand()%(XX*YY);
		m=k/XX;n=k%XX;
		a[m][n]=star;
	}
	if(flag==0)//save
	{
		c=&a[0][0];
		memset(name,0,sizeof(name));
		snprintf(name,sizeof(name),"%ld.sav",time(0));
		k=open(name,O_CREAT|O_RDWR,0644);
		if(k<=0)
			return;
		i=XX;j=YY;
		write(k,(void*)&i,4);
		write(k,(void*)&j,4);
		write(k,c,i*j);
		close(k);
	}
}//}}}
int goon();

//{{{ int main(int argc,char **argv)
int main(int argc,char **argv)
{
	int i,j,k,l;
	char *c;
	if(argc==2)
	{
		if(memcmp(argv[1],"--nosave",sizeof("--nosave"))==0)
			flag=1;
		else
		{
			if(memcmp(argv[1],"--help",sizeof("--help"))==0) //show help
			{
				printf("HELP\n");
				return 0;
			}//add another para~~~
		}
	}
	if(argc==3)
	{
		if(memcmp(argv[1],"-f",sizeof("-f"))==0)
		{
			flag=2;
			k=open(argv[2],O_RDONLY);
			if(k<=0)
			{
				printf("can't open file %s\n",argv[2]);
				return 0;
			}
			l=read(k,(void*)&i,4);
			if(l!=4)
			{
				close(k);
				printf("read error no1\n");
				return 0;
			}
			l=read(k,(void*)&j,4);
			if(l!=4)
			{
				close(k);
				printf("read error no2\n");
				return 0;
			}
			if(i!=XX || j!=YY)
			{
				close(k);
				printf("file data error\n");
				return 0;
			}
			c=&a[0][0];
			memset(c,0,XX*YY);
			l=read(k,c,i*j);
			close(k);
			if(l!=i*j)
			{
				printf("read error no3\n");
				return 0;
			}
		}
	}
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr,true);
	if(COLS<=XX || LINES<=(YY-2))
	{
		move(LINES/2,COLS/2-10);
		printw("window size is to small");
		refresh();
		goto end_01;
	}
	initdata();
	goon();
end_01:	
	k=getchar();
	endwin();
	return 0;
}//}}}
//{{{ int goon()
int goon()
{
	static int st=0;
	int i,j,k,m,n,v1,v2;
	char *c1,*c,ch;
	c=&b[0][0];
	c1=&a[0][0];
	if(flag!=2)
		seed();
	while(1)
	{
		memset(c,nul,XX*YY);
		for(i=0;i<XX;i++)
		{
			for(j=0;j<YY;j++)
			{
				m=n=0;
				if(j>0)
				{
					if(i>0)
					{
						if(a[j-1][i-1]==star)
							m++;
					}
					if(a[j-1][i]==star)
						m++;
					if(i<(XX-1))
					{
						if(a[j-1][i+1]==star)
							m++;
					}
				}
				if(i>0)
				{
					if(a[j][i-1]==star)
						m++;
				}
				if(i<(XX-1))
				{
					if(a[j][i+1]==star)
						m++;
				}
				if(j<(YY-1))
				{
					if(i>0)
						if(a[j+1][i-1]==star)
							m++;
					if(a[j+1][i]==star)
						m++;
					if(i<(XX-1))
						if(a[j+1][i+1]==star)
							m++;
				}
				if(m==2)
					b[j][i]=a[j][i];
				else
				{
					if(m==3)
						b[j][i]=star;
					else
						b[j][i]=nul;
				}
			}
		}
		lv_counts=0;
		for(i=0;i<XX;i++)
		{
			for(j=0;j<YY;j++)
			{
				move(j+1,i+10);
				echochar(b[j][i]);
				if(b[j][i]==star)
					lv_counts++;
			}
		}
		lst[head++]=lv_counts;
		if(head>=10)
			head=0;
		n=0;
		for(m=0;m<10;m++)
		{
			if(lst[m]==lv_counts)
				n++;
		}
		if(n>2)
			rep++;
		else
			rep=0;
		ev_counts++;
		move(4,XX+15);
		printw("ev_counts: %04d",ev_counts);
		move(6,XX+15);
		printw("lv_counts: %04d",lv_counts);
		if(rep>60)
		{
			move(8,XX+15);
			printw("finished evolution!");
			move(10,XX+15);
			printw("press any key to exit..");
			refresh();
			break;
		}
		refresh();
		pump();
		memcpy(c1,c,XX*YY);
		//break;
	}
	return 0;
}//}}}





