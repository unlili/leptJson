/*
lepton n. 轻粒子（希腊的货币单位，古希腊的最小硬币）



*/

#ifndef LEPTJSON_H__
#define LEPTJSON_H__

#define _CRT_SECURE_NO_DEPRECATE

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
	LEPT_PARSE_MISS_KEY,                       //缺少key
	LEPT_PARSE_MISS_COLON,                       //缺少冒号 :
	LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET,  //对象之间缺少逗号，缺少花括号
	LEPT_STRINGIFY_OK,                        //字符串化成功

};
typedef struct lept_value lept_value;
typedef struct lept_member lept_member;

//解析成树后的每一个节点,里面包含该节点的类型
struct lept_value{
	union 
	{
		struct { lept_member* m; size_t size; }o; /*json object */
		struct { lept_value * e; size_t size; }a; /*json array  size元素个数*/
		struct {	   char * s; size_t len;  }s; /*json string len字符串长度*/
		double n;     //用于解析number 只有当type为LEPT_NUMBER时才有意义
	}u;
	lept_type type;
};
/*一个json对象*/
struct lept_member { /*花括号里面是键值对，每一个键值对都是一个 json对象 { "name":"runoob", "alexa" : 10000, "site" : null }*/
	char* k; size_t klen;   /* member key string, key string length */
	lept_value v;           /* member value */
};

#define LEPT_KEY_NOT_EXIST ((size_t)-1)//lept_find_object_index() key not exist

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

//向c里面压一个字符ch
#define PUTC(c, ch) do { *(char*)lept_context_push(c, sizeof(char)) = (ch); } while(0)
//向c里面的栈压len长度的s
#define PUTS(c, s, len)     memcpy(lept_context_push(c, len), s, len)

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

size_t lept_get_object_size(const lept_value* v);/*获得对象个数*/

const char* lept_get_object_key(const lept_value* v, size_t index);/*获得对象的key*/

size_t lept_get_object_key_length(const lept_value* v, size_t index);/*获得对象key字符串长度*/

lept_value* lept_get_object_value(const lept_value* v, size_t index);/*获得对象的value*/

int lept_stringify(const lept_value* v,  char** json ,size_t* length);

size_t lept_find_object_index(const lept_value* v, const char* key, size_t klen);//input key output value index

lept_value* lept_find_object_value(const lept_value* v, const char* key, size_t klen);//通过key查找返回value   key:value

int lept_is_equal(const lept_value* lhs, const lept_value* rhs);

lept_value * lept_set_object_value(lept_value* v, const char * key, size_t klen);//通过key修改value

void lept_copy(lept_value* dst, const lept_value* src);/* 返回新增键值对的指针 */

void lept_move(lept_value* dst, lept_value* src);

void lept_swap(lept_value* lhs, lept_value* rhs);

#endif /* LEPTJSON_H__ */
