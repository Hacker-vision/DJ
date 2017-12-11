#define MAX 20000
#include<stdio.h>
#include<string.h>

char extra[MAX]={0};

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
void global_struct_array();

void extra_info()
{
	fprintf(fw,"#include<stdio.h>\n");
	fprintf(fw,"#define WriteLine() printf(\"\\n\");\n");
	fprintf(fw,"#define WriteLong(x) printf(\" %%lld\", (long)x);\n");
	fprintf(fw,"#define ReadLong(a) if (fscanf(stdin, \"%%lld\", &a) != 1) a = 0;\n");
	fprintf(fw,"#define long long long\n");
}

void Recompiler(char filename[])
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
	printf("recompiler accomplish!\n");
}


/**************************************��������***********************************/
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