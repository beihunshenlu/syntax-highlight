#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define bool int
#define true 1
#define false 0
#define uint32_t int

enum
{
    TK_NOTYPE = 256, OPERATOR, BRACKETS, TK_EQ, TK_NUMBER, TK_VARIABLE, TK_NEWLINE, TK_TAB, KEY_WORD
};

//记录下关键字，匹配时按照变量匹配，显示颜色时关键字统一为蓝色
char Key_Word[50][10] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};

//结构体中记录正则表达式以及类型，类型由前面的枚举变量指出
static struct rule
{
    char *regex;
    int token_type;
} rules[] =
{
	{" +", TK_NOTYPE},
    	{"[\n]+", TK_NEWLINE},
    	{"[\t]+", TK_TAB},
    	{"\\+", OPERATOR},                
    	{"[0-9]+", TK_NUMBER},        
	{"-", OPERATOR},                   
	{"\\*", OPERATOR},                 
	{"\\/", OPERATOR},                 
	{"\\(", BRACKETS},                 
	{"\\)", BRACKETS},
	{"[a-zA-Z_][a-zA-Z_0-9]*", TK_VARIABLE},
	{"#", OPERATOR},
	{"<", OPERATOR},
	{">", OPERATOR},
	{"\\.", OPERATOR},
	{"\\\\", OPERATOR},
	{"\\{", OPERATOR},
	{"\\}", OPERATOR},
	{"\\\"", OPERATOR},
	{"\\!", OPERATOR},
	{";", OPERATOR},
	{"\\[", OPERATOR},
	{"\\]", OPERATOR},
	{"\\,", OPERATOR},
	{"\\=", OPERATOR},
	{"\\&", OPERATOR},
	{"\\|", OPERATOR},
	{"\\:", OPERATOR},
	{"'", OPERATOR},
	{"\\%", OPERATOR},
	{"\\^", OPERATOR},
	{"\\$", OPERATOR},
	{"@", OPERATOR},
	{"\\?", OPERATOR},
	{"\\~", OPERATOR},
	{"`", OPERATOR},
	{"==", TK_EQ}        
};

//NR_REGEX记录了由多少条正则匹配的规则
#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

//Token结构体记录从文本中匹配出来的字符串以及字符串的类型，类型也是由枚举变量指出
typedef struct token
{
    int type;
    char str[100];
} Token;

Token tokens[9000];

//nr_token记录了匹配了多少个token
int nr_token;

static regex_t re[NR_REGEX];

//利用regcomp()函数进行初始化操作，以方便接下来使用regexec()函数进行进一步操作
void init_regex()
{
    int i;
    char error_msg[128];
    int ret;
    for (i = 0; i < NR_REGEX; i ++)
    {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0)
        {
            regerror(ret, &re[i], error_msg, 128);
	    printf("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
	    assert(0);
        }
    }
}


//此函数中将匹配到的字符串记录到tokens结构体变量中
static bool make_token(char e[3000])
{
    int position = 0;
    int i;
    regmatch_t pmatch;
    nr_token = 0;
    while (e[position] != '\0')
    {
        for (i = 0; i < NR_REGEX; i ++)
        {
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
            {
                char *substr_start = e + position;
                int substr_len = pmatch.rm_eo;
                position += substr_len;
                switch (rules[i].token_type)
                {
                case TK_NOTYPE:
                case OPERATOR:
                case TK_NUMBER:
                case BRACKETS:
		case TK_VARIABLE:
		case TK_NEWLINE:
		case TK_TAB:
                default:
                    tokens[nr_token].type = rules[i].token_type;
                    if(substr_len > 90)
                    {

                        printf("The current matching substring is too long!!\n");
                        assert(0);
                    }
                    else
                    {
			int j;
                        for(j = 0; j < substr_len; j ++)
                        {
                            tokens[nr_token].str[j] = substr_start[j];
                        }
                        tokens[nr_token].str[substr_len] = '\0';
                        nr_token ++;
                    }
		    //由于匹配出的TK_VARIABLE中可能由保留字，所以需要把保留字识别出来，把类型换成KEY_WORD
		    if((substr_len <= 90) && (tokens[nr_token - 1].type == TK_VARIABLE))
		    {
	                int j;
			for(j = 0; j < 32; j ++)
			{
				if(strcmp(tokens[nr_token - 1].str, Key_Word[j]) == 0)
				{
					tokens[nr_token - 1].type = KEY_WORD;
				}
			}
		    }
                break;
                }
            break;
            }
        }

        if (i == NR_REGEX)
        {
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }
    return true;
}

uint32_t expr(char e[3000], bool *success)
{
    int i;
	if (!make_token(e))
    {
        *success = false;
        return 0;
    }
    *success = true;
    return 0;
}

//读出a.c文件中的内容到ch字符串中
void read_file(char ch[3000])
{
	FILE *fp;
	int length = 0;
	int i;
	if((fp = fopen("a.c", "r")) == NULL)
		return ;
	while(!feof(fp))
	{
		fscanf(fp, "%c", &ch[length]);
		length ++;
	}
	printf("length: %d\n", length);
	fclose(fp);
}

//对匹配出的不同类型按照不同的颜色输出
void syntax_light()
{
	int i;
	for(i = 0; i < nr_token; i ++)
	{
		switch(tokens[i].type)
		{
			case TK_NOTYPE:
				printf("%s", tokens[i].str);
                		break;
			case TK_NEWLINE:
				printf("%s", tokens[i].str);
                		break;
			case TK_TAB:
				printf("%s", tokens[i].str);
                		break;
            		case OPERATOR:
				printf("\033[32m%s\033[0m", tokens[i].str);
				break;
            		case TK_NUMBER:
				printf("\033[31m%s\033[0m", tokens[i].str);
				break;
            		case BRACKETS:
				printf("\033[91m%s\033[0m", tokens[i].str);
				break;
			case TK_VARIABLE:
				printf("\033[97m%s\033[0m", tokens[i].str);
				break;
			case KEY_WORD:
				printf("\033[34m%s\033[0m", tokens[i].str);
				break;
            		default:
				assert(0);
		}
	}
}

int main()
{
	init_regex();
	bool success = 0;
	char text[9000];
	read_file(text);
	expr(text, &success);
	syntax_light();
	return 0;
}
