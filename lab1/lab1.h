#include<stdio.h>
#include<string.h>

#include<iostream>
using namespace std;

#define MAX 20000
#define array_size 400
#define expr_size 1000

char inst[MAX][expr_size];
char extra[MAX]={0};
int num_inst=0;

//�ṹ��
char struct_name[1000][100];
int num_s=0;

//����
char var_name[1000][100];
int num_v=0;

//������������
char param_name[1000][100];
int param_place[1000];
int num_p=0;

int num_big=0;//} num for need now

FILE *fw=stdout;

void pause();
bool load_inst(char filename[]);
void write_file(char filename[]);

void recompiler();
void get_expr(int now,char dst[]);
void global_struct();
void define_var();
void global_def();
void local_define(int start);
void detect_array();//�������
void def_strcut_global();
void translate_fuc(int &now);
void transform_load(int reg,int now);
bool simple_expr(char op[]);
void group_expr(char dst[],char op[],int t1,int t2,char v1[],char v2[]);
void cmp_inst(char op[],char fuc1[],char fuc2[],char op1[],char op2[],int &i);
bool judge_while(int now,int jump);

void extra_info()
{
	fprintf(fw,"#include<stdio.h>\n");
	fprintf(fw,"#define WriteLine() printf(\"\\n\");\n");
	fprintf(fw,"#define WriteLong(x) printf(\" %%lld\", (long)x);\n");
	fprintf(fw,"#define ReadLong(a) if (fscanf(stdin, \"%%lld\", &a) != 1) a = 0;\n");
	fprintf(fw,"#define long long long\n");
}

void write_file(char filename[])
{
	if(strlen(filename)==0)
	{
		printf("input the filename to save c file:");
		gets(filename);
	}
	fw=fopen(filename,"w");
	extra_info();
	global_def();
	recompiler();
	fclose(fw);
	printf("accomplish!\n");
}

bool load_inst(char filename[])
{
	FILE *p=NULL;
	if(strlen(filename)==0)
	{
		printf("input the 3adr file name:");
		gets(filename);
	}
	p=fopen(filename,"r");
	if(p==NULL)
	{
		printf("%s is not exist",filename);
		return false;
	}
	char str[100];
	int n=1;
	int temp=0;
	while(!feof(p))
	{
		fscanf(p,"%s %d%s ",str,&temp,str);
		if(temp==n)
		{
			fgets(str,100,p);
			str[strlen(str)-1]='\0';
			strcpy(inst[n++],str);
		}
	}
	num_inst=n;
	fclose(p);
	return true;
}


/**************************************��������***********************************/
void pause()
{
	//char t[10];
	//gets(t);
}

void add_tab()//����tab
{
	for(int i=0;i<num_big;i++)
		fprintf(fw,"\t");
}

void extra_judge(int i)//������жϣ�ĿǰΪ����  }
{
	for(int c=0;c<(int)extra[i];c++)
	{
		num_big--;
		add_tab();//���tab��
		fprintf(fw,"} \n");//���뻺����
	}
}

int get_op(char src[],char dst[],int &reg)//��ȡ�����������ͣ����ʽ���������������ֲ�/ȫ�֣�
{
	if(src[0]<='9'&&src[0]>='0')//����
	{
		strcpy(dst,src);
		return 1;
	}
	else if(src[0]=='(')//���ʽ
	{
		sscanf(src,"(%d)",&reg);
		return 2;
	}
	else//����
	{
		int l=strlen(src),i=0;
		for(i=0;i<l;i++)
		{
			if(src[i]=='#')
			{
				break;
			}
			else
				dst[i]=src[i];
		}
		dst[i]='\0';
		sscanf(&src[i+1],"%d",&reg);
		return 3;
	}
}

bool find_base(char src[],char dst[])//����_base֮ǰ�ı�������
{
	
	int l=strlen(src);
	int i=0;
	for(i=0;i<l-4;i++)
	{
		if(src[i]=='_'&&src[i+1]=='b'&&src[i+2]=='a'&&src[i+3]=='s'&&src[i+4]=='e')
		{
			int n=i;
			for(int t=0;t<n;t++)
				dst[t]=src[t];
			dst[n]='\0';
			return true;
		}
	}
	return false;
}

bool judge_offset(char src[],char dst[])//�ж�src���Ƿ����_offset����ȡ��������
{
	int l=strlen(src);
	int i=0;
	for(i=0;i<l;i++)
	{
		if(src[i]=='_')
		{
			dst[i]='\0';
			if(strcmp(&src[i+1],"offset")==0)
				return true;
			else
				return false;
		}
		dst[i]=src[i];
	}
	return false;
}


//�������壬nameΪ���еı��������飬listΪ��Ҫ������ı���
void define_var(char name[][100],int &num_v,int list[],int &num)
{
	int i=0,j=0;
	for(i=0;i<num_v;i++)
	{	
		for(j=i-1;j>=0;j--)//�����û���Ѿ��������
		{
			if(strcmp(name[j],name[i])==0)
				break;
		}
		if(j<0)
		{
			list[num++]=i;
		}
	}
	num_v=0;
}