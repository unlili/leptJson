/*
lepton n. 轻粒子（希腊的货币单位，古希腊的最小硬币）



*/

#ifndef LEPTJSON_H__
#define LEPTJSON_H__
#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<math.h>
#include<errno.h>
#include<memory>
#include<ctype.h>

//json 所有类型
typedef enum {LEPT_NULL,LEPT_FALSE,LEPT_TRUE,LEPT_NUMBER,LEPT_STRING,LEPT_ARRAY,LEPT_OBJECT} lept_type;

//解析的返回值,OK返回LEPT_PARSE_OK
enum {
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,            //一个JSON只含有空白  ""  "  "
	LEPT_PARSE_INVALID_VALUE,           //无效值 非"null" "false" "true"
	LEPT_PARSE_ROOT_NOT_SINGULAR,       //若一个值之后，在空白之后还有其他字符"false x"  "null q"
	LEPT_PARSE_NUMBER_TOO_BIG,          //
	LEPT_PARSE_MISS_QUOTATION_MARK,     //少引号
	LEPT_PARSE_INVALID_STRING_ESCAPE,    // \\w  不对的转意
	LEPT_PARSE_INVALID_STRING_CHAR,
	LEPT_PARSE_INVALID_UNICODE_SURROGATE,   //只有高代理项而欠缺低代理项，或是低代理项不在合法码点范围
	LEPT_PARSE_INVALID_UNICODE_HEX,          //\u?后不是 4 位十六进位数字
	LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,  //数组最后缺少逗号，缺少方括号
};
typedef struct lept_value lept_value;

//解析成树后的每一个节点,里面包含该节点的类型
struct lept_value{
	union 
	{
		struct { lept_value * e; size_t size; }a; /*array  size元素个数*/
		struct {	   char * s; size_t len;  }s; /*string len字符串长度*/
		double n;     //用于解析number 只有当type为LEPT_NUMBER时才有意义
	}u;
	lept_type type;
};

//string错误处理,并返回错误码
#define STRING_ERROR(ret) do { c->top = head; return ret; } while(0)

//判断c的第一个字符是不是ch,是的话c下移,不是的话程序崩溃
#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)

#define lept_init(v) do{(v)->type = LEPT_NULL;}while(0)

#define IS_DIGIT(ch) ((ch)>='0' && (ch)<='9')       //0~9
#define IS_DIGIT1T09(ch) ((ch)>='1' && (ch)<='9')   //1~9

#define IS_BIG_LETTER(ch) ((ch) >= 'A' && (ch)<='Z'  )
#define IS_LOW_LETTER(ch) ((ch) >= 'a' && (ch)<='z'  )

#define IS_LETTER(ch) (IS_BIG_LETTER(ch) || IS_LOW_LETTER(ch))

//从c里面
#define PUTC(c, ch) do { *(char*)lept_context_push(c, sizeof(char)) = (ch); } while(0)


int lept_parse(lept_value * v,const char * json); //解析JSON

lept_type lept_get_type(const lept_value* v);//访问结果

#define lept_set_null(v) lept_free(v)//go

int lept_get_boolean(const lept_value* v);//go
void lept_set_boolean(lept_value* v, int b);//go

double lept_get_number(const lept_value* v);//go
void lept_set_number(lept_value* v, double n);//go

const char* lept_get_string(const lept_value* v);//go
size_t lept_get_string_length(const lept_value* v);//go
void lept_set_string(lept_value* v, const char* s, size_t len);//go

void lept_free(lept_value * v);//go

size_t lept_get_array_size(const lept_value* v);

lept_value* lept_get_array_element(const lept_value* v, size_t index);

#endif /* LEPTJSON_H__ */
