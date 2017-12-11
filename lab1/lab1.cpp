#include"lab1.h"


int main(int argc,char **argv)
{
	char filename[300]="";
	if(argc>1)
	{
		if(strcmp(argv[1],"-h")==0)
		{
			printf("recompiler src dst\n");
		}
		else
		{
			strcpy(filename,argv[1]);
			if(load_inst(filename))
			{
				if(argc>2)
					strcpy(filename,argv[2]);
				else
					strcpy(filename,"");
				write_file(filename);
			}
		}
	}
	else
	{
		if(load_inst(filename))
		{
			strcpy(filename,"");
			write_file(filename);
		}
	}
	pause();
	return 0;
}


//������
void recompiler()
{
	int i=0;
	char op[expr_size],fuc1[expr_size],fuc2[expr_size];
	for(i=1;i<num_inst;i++)
	{
		extra_judge(i);//������жϣ�ĿǰΪ����  }
		sscanf(inst[i],"%s %s %s",op,fuc1,fuc2);
		if(strcmp(op,"nop")==0)//��ָ��
			continue;
		else if(strcmp(op,"entrypc")==0)//ָʾmain����
		{
			fprintf(fw,"void main(");
			local_define(i+1);//�ֲ���������붨��
			i++;
		}
		else if(strcmp(op,"enter")==0)//�����Ķ���
		{
			fprintf(fw,"void fuction_%d(",i);
			local_define(i+1);//�����ľֲ��������
			continue;
		}
		else if(strcmp(op,"cmpeq")==0)//�Ƚ����
		{
			cmp_inst(op,fuc1,fuc2,"==","!=",i);//�ж�����ʶ�𣬰�����while
		}
		else if(strcmp(op,"cmple")==0)//�Ƚ�С�ڵ���
		{
			cmp_inst(op,fuc1,fuc2,"<=",">",i);
		}
		else if(strcmp(op,"cmplt")==0)//�Ƚ�С��
		{
			cmp_inst(op,fuc1,fuc2,"<",">=",i);
		}
		else if(strcmp(op,"load")==0)//�����ڴ����ݣ�ȫ�ֱ������ṹ�壬����
		{
			int reg=0;
			sscanf(fuc1,"(%d)",&reg);
			transform_load(reg,i);//loadָ���ת����ת��Ϊһ���򵥵ı��ʽ������֮���ʹ��
		}
		else if(strcmp(op,"store")==0)//�洢�ڴ����ݣ�ȫ�ֱ������ṹ�壬����
		{
			char v1[expr_size],v2[expr_size];
			int reg1,reg2=0;
			int t1=get_op(fuc1,v1,reg1);//��ȡ��ǰ������ ������������ȫ��/�ֲ��������ʽ
			int t2=get_op(fuc2,v2,reg2);

			if(t1==2)
				get_expr(reg1,v1);//��ȡ���ʽ
			if(t2==2)
				get_expr(reg2,v2);

			sprintf(inst[i]," %s=%s",v2,v1);
			add_tab();//���tab
			fprintf(fw,"%s=%s;\n",v2,v1);
		}
		else if(strcmp(op,"param")==0)//���������ں�������
		{
			translate_fuc(i);//��������ת����������������ʱΪcallָ��
		}
		else if(strcmp(op,"call")==0)//�޲����ĺ�������
		{
			int reg=0;
			sscanf(fuc1,"[%d]",&reg);
			add_tab();
			fprintf(fw,"fuction_%d();\n");
		}
		else if(strcmp(op,"else")==0)//����ָ�����else
		{
			add_tab();
			fprintf(fw,"else\n");
			add_tab();
			fprintf(fw,"{\n");
			num_big++;
			int t=0;
			sscanf(fuc1,"%d",&t);
			extra[t]++;
		}
		else if(strcmp(op,"write")==0)//���
		{
			char v1[expr_size];
			int reg=0;
			int t=get_op(fuc1,v1,reg);
			if(t==2)
				get_expr(reg,v1);
			add_tab();
			fprintf(fw,"WriteLong(%s);\n",v1);
		}
		else if(strcmp(op,"wrl")==0)//����
		{
			add_tab();
			fprintf(fw,"WriteLine();\n");
		}
		else if(simple_expr(op))//�򵥵ı��ʽ��add,sub,mul,div,mod,neg,move
		{
			char dst[expr_size];
			get_expr(i,dst);
			if(strcmp(op,"move")==0)
			{
				add_tab();
				fprintf(fw,"%s;\n",dst);
			}
		}
		else if(strcmp(op,"ret")==0)//��������ָ��
		{
			num_big--;
			add_tab();
			fprintf(fw,"}\n");
		}
	}
}



/*********************************ȫ�ֱ����Ķ���************************************/

void global_def()//ȫ�ֱ����Ķ��壬�������飬�ṹ�壬����
{
	detect_array();//������飬������ȫ������

	global_struct();//���ṹ���壬������

	int list[1000],num=0;
	define_var(var_name,num_v,list,num);//ȫ�ֱ�������
	for(int i=0;i<num;i++)
		fprintf(fw,"long %s; \n",var_name[list[i]]);
}


/*********************************����Ķ����ת��************************************/

void transform_array(int n)//����ת��
{
	char op[expr_size],fuc1[expr_size],type[expr_size],op1[expr_size],expr[expr_size];
	sscanf(inst[n],"%s %s %s",op,expr,type);
	sscanf(inst[n+1],"%s %s %s",op1,fuc1,type);
	
	char name[expr_size];
	if(find_base(fuc1,name))//������ı�־������base
	{
		if(strcmp(type,"GP")==0)
		{
			strcpy(var_name[num_v++],name);
		}

		sprintf(inst[n]," nop ");
		sprintf(inst[n+1]," nop ");

		int reg=0;
		int t=get_op(expr,fuc1,reg);
		
		if(t==1)
			sprintf(inst[n+2]," array %s[%s] %s %s ",name,fuc1,name,type);
		else if(t==3)
		{
			sprintf(inst[n+1]," var %s ",expr);
			sprintf(inst[n+2]," array %s[%s] %s %s ",name,fuc1,name,type);
		}
		else if(t==2)
		{
			sprintf(inst[n+2]," array %s[(%s)] %s %s %s %d",name,expr,name,type,"reg",reg);
		}
	}

}

void detect_array()//������飬������
{
	int i=0,j=0;
	char op[expr_size],fuc1[expr_size],fuc2[expr_size],op1[expr_size],op2[expr_size];
	num_v=0;
	for(i=0;i<num_inst-2;i++)
	{
		sscanf(inst[i],"%s %s %s",op,fuc1,fuc2);//����ĸ�ʽmul��add��add
		sscanf(inst[i+1],"%s %s %s",op1,fuc1,fuc2);
		sscanf(inst[i+2],"%s %s %s",op2,fuc1,fuc2);
		if(strcmp(op,"mul")==0&&strcmp(op1,"add")==0&&strcmp(op2,"add")==0)
		{
			transform_array(i);
			i=i+2;
		}
	}

	int list[expr_size],num=0;
	define_var(var_name,num_v,list,num);
	for(i=0;i<num;i++)
		fprintf(fw,"long %s[%d]; \n",var_name[list[i]],array_size);

}

/*********************************�ֲ�����Ķ���************************************/

void local_array(int start)//��ȡ�ֲ����飬������
{
	char op[expr_size],fuc1[expr_size],fuc2[expr_size];
	int i=0;
	for(i=start;i<num_inst;i++)
	{
		sscanf(inst[i],"%s",op);
		if(strcmp(op,"ret")==0)
			break;
		if(strcmp(op,"array")==0)//��⵽����ָ��array
		{
			sscanf(inst[i],"%s %s %s %s",op,op,fuc1,fuc2);
			if(strcmp(fuc2,"FP")==0)
			{
				strcpy(var_name[num_v++],fuc1);
			}
		}
	}

	int list[expr_size],num=0;
	define_var(var_name,num_v,list,num);
	for(i=0;i<num;i++)
	{
		add_tab();
		fprintf(fw,"long %s[%d]; \n",var_name[list[i]],array_size);
	}
}



/*********************************�ṹ���ת���Ͷ���************************************/

void def_strcut_global()//����ȫ�ֽṹ�����
{
	int i=0,j=0;
	for(i=0;i<num_s;i++)
	{
		for(j=i-1;j>=0;j--)
		{
			if(strcmp(struct_name[j],struct_name[i])==0)
				break;
		}
		if(j<0)
		{
			fprintf(fw,"long %s; \n",struct_name[i]);
		}
	}
}

void global_struct()//�ṹ�嶨��
{
	int i=0;
	char op[expr_size],fuc1[expr_size],type[expr_size],fuc2[expr_size],dst[expr_size];

	for(i=1;i<num_inst;i++)//��������ָ��ҵ��ṹ��Ľṹ����ת�� �ṹ�嶼��Ϊ��������
	{
		sscanf(inst[i],"%s %s %s",op,fuc1,type);
		if(strcmp(op,"add")==0&&find_base(fuc1,dst))//�ҵ�base���ṹ��ı�־1
		{
			sprintf(inst[i]," nop ");
			char temp[200];
			strcpy(temp,dst);
			int now=i;
			for(i=i+1;i<num_inst;i++)//offset����ṹ��Ľ�һ������
			{
				sscanf(inst[i],"%s %s %s",op,fuc1,fuc2);
				int reg=0;
				get_op(fuc2,dst,reg);
				if(strcmp(op,"add")==0&&judge_offset(dst,fuc2))
				{
					sprintf(inst[i]," nop ");
					strcat(temp,"_");
					strcat(temp,fuc2);
				}
				else if(i==now+1)//���ֻ��baseû��offset��˵����ȫ�ֱ�����ʹ���˸���ָ��global
				{
					sprintf(inst[i-1],"global %s ",temp);
					strcpy(var_name[num_v++],temp);//��ȫ�ֱ�����������
					break;
				}
				else if(i>now+1)//�����offset����˵��Ϊ�ṹ�壬ʹ�ø���ָ��struct��ָ���ǽṹ��
				{
					sprintf(inst[i-1]," struct %s %s ",temp,type);
					if(strcmp(type,"GP")==0)
						strcpy(struct_name[num_s++],temp);//��ȫ�ֽṹ�屣������
					i--;
					break;
				}
				else
					break;
			}
		}
	}
	def_strcut_global();//�����⵽��ȫ�ֽṹ�����
}

/*********************************�ֲ������Ķ��壬���������Ķ���************************************/

bool judge_two_op(char op[])//����Ƿ�Ϊ��������
{
	if(strcmp(op,"add")==0||strcmp(op,"sub")==0||strcmp(op,"mul")==0||strcmp(op,"div")==0||strcmp(op,"mod")==0)
		return true;
	else if(strcmp(op,"move")==0||strcmp(op,"cmpeq")==0||strcmp(op,"cmple")==0||strcmp(op,"cmplt")==0)
		return true;
	else
		return false;
}

void sort_list(int list[],int num)
{
	int i=0,j=0,temp=0;
	for(i=0;i<num;i++)
	{
		for(j=i+1;j<num;j++)
		{
			if(param_place[list[i]]<param_place[list[j]])
			{
				temp=list[i];
				list[i]=list[j];
				list[j]=temp;
			}
		}
	}
}

void local_define(int start)//�ֲ��������壬���������飬�ṹ��
{
	char op[expr_size],fuc1[expr_size],fuc2[expr_size];
	int i=0;
	num_p=0;
	num_v=0;

	char v1[expr_size],v2[expr_size];
	int t1,t2,reg1,reg2;
	for(i=start;i<num_inst;i++)
	{
		sscanf(inst[i],"%s %s %s",op,fuc1,fuc2);
		if(strcmp(op,"ret")==0)
			break;
		if(judge_two_op(op))//˫������������Ҳ���ܰ�����������Ҫ����Ƿ��оֲ�����
		{
			t1=get_op(fuc1,v1,reg1);
			t2=get_op(fuc2,v2,reg2);
			if(t1==3)
			{
				if(reg1>0)
				{
					param_place[num_p]=reg1;
					strcpy(param_name[num_p++],v1);
				}
				else
					strcpy(var_name[num_v++],v1);
			}
			if(t2==3)
			{
				if(reg2>0)
				{
					param_place[num_p]=reg1;
					strcpy(param_name[num_p++],v2);
				}
				else
					strcpy(var_name[num_v++],v2);
			}
		}
		else if(strcmp(op,"neg")==0||strcmp(op,"var")==0||strcmp(op,"param")==0||strcmp(op,"write")==0)//�������������п��ܰ�����������Ҫ���
		{
			t1=get_op(fuc1,v1,reg1);
			if(t1==3)
			{
				if(reg1>0)
				{
					param_place[num_p]=reg1;
					strcpy(param_name[num_p++],v1);
				}
				else
					strcpy(var_name[num_v++],v1);
			}
			if(strcmp(op,"var")==0)
				sprintf(inst[i]," nop ");
		}
		else if(strcmp(op,"struct")==0&&strcmp(fuc2,"FP")==0)//�ṹ�嶨��
		{
			strcpy(var_name[num_v++],fuc1);
		}
	}

	int list[1000],num=0;
	define_var(param_name,num_p,list,num);//���庯�����ò���
	sort_list(list,num);
	for(i=0;i<num-1;i++)
		fprintf(fw,"long %s,",param_name[list[i]]);
	if(num-1>=0)
		fprintf(fw,"long %s",param_name[list[num-1]]);
	fprintf(fw,") \n{ \n");

	num_big++;

	num=0;
	define_var(var_name,num_v,list,num);//�ֲ���������
	for(i=0;i<num;i++)
	{
		add_tab();
		fprintf(fw,"long %s; \n",var_name[list[i]]);
	}

	local_array(start);//�ֲ����鶨��

}


/***********************************��ָ֧���while***********************************/

bool judge_while(int now,int jump)//�ж��Ƿ�ǰ��cmpΪwhile
{
	char op[expr_size],fuc1[expr_size],fuc2[expr_size];
	sscanf(inst[jump-1],"%s %s %s",op,fuc1,fuc2);
	if(strcmp(op,"br")==0)
	{
		int t=0;
		sscanf(fuc1,"[%d]",&t);
		if(t<=now)
			return true;
		else if(t>=jump)
		{
			sprintf(inst[jump-1],"else %d",t);
			extra[jump-1]++;
			extra[jump]--;
		}
	}
	return false;
}

//if��while�ļ��
void cmp_inst(char op[],char fuc1[],char fuc2[],char op1[],char op2[],int &i)
{
	char temp[expr_size];
	char v1[expr_size],v2[expr_size];
	int t1=0,t2=0,reg2=0,reg1=0;//flag����ָʾ�Ƿ�Ϊ����

	if(get_op(fuc1,v1,reg1)==2)
		get_expr(reg1,v1);
	if(get_op(fuc2,v2,reg2)==2)
		get_expr(reg2,v2);//��ȡ�ж����Ե����������������ǳ���������Ǳ�����Ҫ�ӵ��������С�
			
	sscanf(inst[i+1],"%s %s %s",op,fuc1,fuc2);
	int jump=0;
	sscanf(fuc2,"[%d]",&jump);
	bool flag_while=judge_while(i,jump);//�ж��Ƿ����Ϊwhileѭ��

	if(strcmp(op,"blbc")==0)//�����ĵ���
	{
		if(flag_while)
			sprintf(temp,"while(%s%s%s) \n",v1,op1,v2);
		else
			sprintf(temp,"if(%s%s%s) \n",v1,op1,v2);
	}
	else if(strcmp(op,"blbs")==0)//���ڵ��෴���
	{
		if(flag_while)
			sprintf(temp,"while(%s%s%s) \n",v1,op2,v2);
		else
			sprintf(temp,"if(%s%s%s) \n",v1,op2,v2);
	}
	else
	{
		fprintf(fw,"cmpeq is wrong struct! \n");
	}
			
	extra[jump]++;//��ת��Ŀ��ָ����Ҫ��ȫ  }

	i++;//����һ��ָ��
	add_tab();//���tab��
	fprintf(fw,"%s",temp);//���뻺����
	add_tab();
	fprintf(fw,"{\n");
	num_big++;//{�ĸ�������һ��
}

/*********************************������ָ��************************************/

void group_expr(char dst[],char op[],int t1,int t2,char v1[],char v2[])
{
	if(t1==2)
		sprintf(dst,"(%s)%s",v1,op);
	else
		sprintf(dst,"%s%s",v1,op);
	if(t2==2)
		sprintf(dst,"%s(%s) ",dst,v2);
	else
		sprintf(dst,"%s%s ",dst,v2);
}

void get_expr(int now,char dst[])//��ȡ���ʽ��nowΪ�ڼ���ָ�dstΪ������ʽ
{
	char op[expr_size],fuc1[expr_size],fuc2[expr_size],temp[expr_size];
	sscanf(inst[now],"%s %s %s",op,fuc1,fuc2);
	if(strcmp(op,"ok")==0) //���ʽ�Ƿ��Ѿ�������
	{
		strcpy(dst,fuc1);
		return ;
	}
	else if(strcmp(op,"struct")==0||strcmp(op,"global")==0)//���⸨��ָ��ֱ�Ϊ�ṹ���ȫ�ֱ���
	{
		strcpy(dst,fuc1);
		return ;
	}
	else if(strcmp(op,"array")==0)//����ĸ���ָ��
	{
		int t=0;
		sscanf(inst[now],"%s %s %s %s %s %d",op,op,fuc1,temp,fuc2,&t);
		if(strcmp(fuc2,"reg")==0)
		{
			get_expr(t,fuc2);
			sprintf(dst,"%s[(%s)]",fuc1,fuc2);
		}
		else
		{ 
			sprintf(dst,"%s",op);
		}
		return ;
	}

	char v1[expr_size],v2[expr_size];
	int reg1=0,reg2=0;
	if(strcmp(op,"neg")==0)//�������� ��������
	{
		if(get_op(fuc1,v1,reg1)==2)//���ʽ
		{
			get_expr(reg1,v1);
			sprintf(dst,"-(%s)",v1);
		}
		else
		{
			sprintf(dst,"-%s",v1);
		}
		sprintf(inst[now],"ok %s ",dst);
	}
	else
	{
		int t1=get_op(fuc1,v1,reg1);
		if(t1==2)
			get_expr(reg1,v1);

		int t2=get_op(fuc2,v2,reg2);
		if(t2==2)
			get_expr(reg2,v2);        //��ȡ�����������������Ǳ��������������ʽ

		if(strcmp(op,"add")==0)
			group_expr(dst,"+",t1,t2,v1,v2);
		else if(strcmp(op,"sub")==0)
			group_expr(dst,"-",t1,t2,v1,v2);
		else if(strcmp(op,"mul")==0)
			group_expr(dst,"*",t1,t2,v1,v2);
		else if(strcmp(op,"div")==0)
			group_expr(dst,"/",t1,t2,v1,v2);
		else if(strcmp(op,"mod")==0)
			group_expr(dst,"%",t1,t2,v1,v2);
		else if(strcmp(op,"move")==0)
			group_expr(dst,"=",t2,t1,v2,v1);
		else
		{
			printf("%s is not added\n",op);
		}
		sprintf(inst[now],"ok %s ",dst); //���ok����ʾ�Ѿ�������������ֱ��ʹ��
	}
}

bool simple_expr(char op[])//�ж��Ƿ�Ϊ������ָ��
{
	if(strcmp(op,"neg")==0||strcmp(op,"move")==0)
		return true;
	if(strcmp(op,"add")==0||strcmp(op,"sub")==0)
		return true;
	if(strcmp(op,"div")==0||strcmp(op,"mul")==0)
		return true;
	if(strcmp(op,"mod")==0)
		return true;
	return false;
}

/*********************************loadָ��************************************/

void transform_load(int reg,int now)//ת��Ϊok expr
{
	char op[expr_size],fuc1[expr_size],fuc2[expr_size],temp[expr_size];
	sscanf(inst[reg],"%s %s %s",op,fuc1,fuc2);
	if(strcmp(op,"global")==0)
	{
		sprintf(inst[now],"ok %s ",fuc1);
	}
	else if(strcmp(op,"array")==0)
	{
		int t=0;
		sscanf(inst[reg],"%s %s %s %s %s %d",op,op,fuc1,temp,fuc2,&t);
		if(strcmp(fuc2,"reg")==0)
		{
			get_expr(t,fuc2);
			sprintf(inst[now],"ok %s[(%s)] ",fuc1,fuc2);
		}
		else
		{ 
			sprintf(inst[now],"ok %s ",op);
		}
	}
	else if(strcmp(op,"struct")==0)
	{
		sprintf(inst[now],"ok %s ",fuc1);
	}
	else
	{
		printf("not realize!\n");
	}
}

/*********************************��������ָ��************************************/

void translate_fuc(int &now)//�������ý���
{
	char op[expr_size],fuc1[expr_size],fuc2[expr_size];
	int i=0,end=0;
	for(i=now+1;i<num_inst;i++)//�ҵ���������������callָ��
	{
		sscanf(inst[i],"%s %s %s",op,fuc1,fuc2);
		if(strcmp(op,"call")==0)
		{
			end=i;
			int reg=0;
			sscanf(fuc1,"[%d]",&reg);
			add_tab();
			fprintf(fw,"fuction_%d(",reg);
			break;
		}
		else if(strcmp(op,"load")==0)
		{
			int reg=0;
			sscanf(fuc1,"(%d)",&reg);
			transform_load(reg,i);//loadָ���ת����ת��Ϊһ���򵥵ı��ʽ������֮���ʹ��
		}
	}
	char v1[expr_size];
	int reg=0;
	int t=0;
	for(i=now;i<end;i++)//��ӵ��ò���
	{
		sscanf(inst[i],"%s %s %s",op,fuc1,fuc2);
		if(strcmp(op,"param")==0)
		{
			t=get_op(fuc1,v1,reg);
			if(t==2)
				get_expr(reg,v1);
			fprintf(fw,"%s",v1);
			if(i==end-1)
				fprintf(fw,");\n");
			else
				fprintf(fw,",");
		}
	}
	now=end;
}