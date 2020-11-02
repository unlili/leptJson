#include"leptjson.h"

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256   //初始化栈大小
#endif

#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


//把函数之间的传递数据放到一个结构体里,方便
typedef struct { 
	const char * json; 
	/*动态创建一个堆栈*/

	char * stack; 
	size_t capacity, top; //堆栈容量,栈顶的位置
}lept_context;

int lept_parse_value(lept_context * c, lept_value * v);//声明


//push  *(char *)lept_context_push(c, 1byte)
static void * lept_context_push(lept_context * c, size_t size)
{
	/*返回数据起始的指针   'E'
	capacity = 256
	top = 1
				|   | 
				|   |
				|   |  <- top
			    |   |
		     	|   |
		         ___
	*/
	void * ret;
	assert(size > 0);

	if (c->top + size >= c->capacity)
	{
		if (c->capacity == 0)//第一次初始化capacity为256
			c->capacity = LEPT_PARSE_STACK_INIT_SIZE;

		while (c->top + size >= c->capacity)
			c->capacity += c->capacity >> 1;//capacity不够的话 *= 1.5

		c->stack = (char*)realloc(c->stack,c->capacity);//给c->stack重新分配c->capacity(byte)的内存,返回手地址
	}
	ret = c->stack + c->top;
	c->top += size;
	return ret;
}
//pop  弹出c->stack + (c->top -= size)的指针
static void * lept_context_pop(lept_context * c, size_t size)
{
	assert(c->top >= size);
	return c->stack + (c->top -= size);
}

//解析p内的4位十六进数字，存储为码点u  把p内的十六进制变成二进制就行, _my 我自己写的
static const char * lept_parse_hex4_my(const char* p, unsigned * u)
{
	char ch;  //ABCD11
	int tempV, ff = 0;
	for (size_t i = 0; i < 4; ++i)
	{
		ch = *p++;
		if ((ch >= 'a' && ch <= 'f')||(ch >= 'A' && ch <= 'F')) tempV = ch - (isupper(ch) ? 55 : 87);
		else if (IS_DIGIT(ch)) tempV = ch - '0';
		else return NULL;


		ff += (int)(tempV * pow(16, 3 - i));
	}
	*u = ff;
	return p;
}
//叶神版本
static const char* lept_parse_hex4(const char* p, unsigned* u) {
	int i;
	*u = 0;
	for (i = 0; i < 4; i++) {
		char ch = *p++;
		*u <<= 4; // *= 16
		if (ch >= '0' && ch <= '9')       *u |= ch - '0';
		else if (ch >= 'A' && ch <= 'F')  *u |= ch - 'A' + 10;
		else if (ch >= 'a' && ch <= 'f')  *u |= ch - 'a' + 10;
		else return NULL;
	}
	return p;
}

//解析成 UTF-8 给码点加上前缀
static int lept_encode_utf8(lept_context * c, unsigned  u)
{
	/* &0xff 是为了为了避免一些编译器的警告误判*/
	if ( u <= 0x007F) 
	{ 
		PUTC(c, u & 0xFF);
	}
	else if ( u <= 0x07FF)
	{
		PUTC(c, 0xC0 | ((u>>6) & 0xFF));
		PUTC(c, 0x80 | (u      & 0x3F));
	}
	else if ( u <= 0xFFFF)
	{
		PUTC(c,(0xE0 | ((u >> 12) & 0xFF)));  /* 0xE0 = 11100000 */
		PUTC(c,(0x80 | ((u >> 6) & 0x3F)));   /* 0x80 = 10000000 */
		PUTC(c,(0x80 | (u & 0x3F)));          /* 0x3F = 00111111 */
	}
	else 
	{
		assert(u<= 0x10FFFF);
		PUTC(c, 0xF0 | ((u >> 18) & 0x07));
		PUTC(c, 0x80 | ((u >> 12) & 0x3F));
		PUTC(c, 0x80 | ((u >>  6) & 0x3F));
		PUTC(c, 0x80 | ((u      ) & 0x3F));
	}
	return 0;
}

static int lept_parse_string(lept_context * c, lept_value * v)
{
	unsigned u,u2;
	size_t head = c->top;   //0
	size_t	len;//string长度

	const char* p;
	EXPECT(c, '\"');//c is \" ?
	p = c->json;
	for (;;)
	{
		char ch = *p++;

		switch (ch)
		{
		case '\"':
			len = c->top - head;//
			lept_set_string(v, (const char*)lept_context_pop(c, len), len);
			c->json = p;      //把后面的字符串在给c->json
			return LEPT_PARSE_OK;
		case '\0':
			STRING_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK);
		case '\\':  /* ==> \ */
			switch(*p++)\
			{
				case '\"':PUTC(c, '\"'); break;
				case '\\':PUTC(c, '\\'); break;
				case '/' :PUTC(c, '/');  break;
				case 'b' :PUTC(c, '\b'); break;
				case 'f' :PUTC(c, '\f'); break;
				case 'n' :PUTC(c, '\n'); break;
				case 'r' :PUTC(c, '\r'); break;
				case 't' :PUTC(c, '\t'); break;
				case 'u' :

					if (!(p = lept_parse_hex4(p, &u))) 
						STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);

					if (u >= 0xD800 && u <= 0xDBFF)
					{
						if (p[0] != '\\' || p[1] != 'u') 
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						if(!(p = lept_parse_hex4(p+=2,&u2))) 
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
						if(u2<= 0xDC00 || u2 >= 0xDFFF) 
							STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
						u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
					}
				
					lept_encode_utf8(c, u);
					break;
				default:
					STRING_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE);//before return LEPT_PARSE_INVALID_STRING_ESCAPE; /*  \\x  \\w  */
			}
			break;
		default:
			if ((unsigned char)ch < 0x20) {
				STRING_ERROR(LEPT_PARSE_INVALID_STRING_CHAR);//before return LEPT_PARSE_INVALID_STRING_CHAR;
			}
			PUTC(c, ch);
		}
	}
}

void lept_parse_whitespace(lept_context * c)
{
	const char * p = c->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		++p;
	c->json = p;
}//过滤whitespace  去掉空格

static int lept_parse_null(lept_context * c, lept_value * v)
{
	EXPECT(c,'n');
	if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
		return LEPT_PARSE_INVALID_VALUE;
	c->json += 3;
	v->type = LEPT_NULL;
	return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context * c, lept_value * v)
{
	EXPECT(c, 'f');
	if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
		return LEPT_PARSE_INVALID_VALUE;
	c->json += 4;
	v->type = LEPT_FALSE;
	return LEPT_PARSE_OK;
}

static int lept_parse_true(lept_context * c, lept_value * v)
{
	EXPECT(c, 't');
	if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e' )
		return LEPT_PARSE_INVALID_VALUE;
	c->json += 3;
	v->type = LEPT_TRUE;
	return LEPT_PARSE_OK;
}

static int lept_parse_literal(lept_context * c, lept_value * v,const char * literal, lept_type type)
{
/*
             0 
	c->json null0x

	literal	null
         	0
*/
		   EXPECT(c, literal[0]);
		   ++literal;
		   while (*c->json == *literal)
		   {
			   ++c->json;
			   ++literal;
			   if (*literal == '\0')
			   {
				   v->type = type;
				   return LEPT_PARSE_OK;
			   }
		   }
		   return LEPT_PARSE_INVALID_VALUE;
}

static int lept_parse_number(lept_context * c, lept_value * v)
{
	const char * p = c->json;
	if (*p == '-') ++p;

	if (*p == '0')
	{
		++p;
		if(IS_DIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;//00 01 02
		if (IS_LETTER(*p)) return LEPT_PARSE_ROOT_NOT_SINGULAR;//0x
	}
	else
	{
		if (!IS_DIGIT1T09(*p))		return LEPT_PARSE_INVALID_VALUE;
		for (++p; IS_DIGIT(*p); ++p);//0123
	}

	if (*p == '.')
	{
		++p;
		if (!IS_DIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;//123.x
		for (p++; IS_DIGIT(*p); p++);
	}
	if (*p == 'e' || *p == 'E') {

		++p;
		if (*p == '+' || *p == '-') ++p;
		if (!IS_DIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
		for (p++; IS_DIGIT(*p); p++);
	}

	errno = 0;

	v->u.n = strtod(c->json,NULL);//把字符串转换为double,多于的字符串放到end后面,
	if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
		return LEPT_PARSE_NUMBER_TOO_BIG;

	c->json = p;
	v->type = LEPT_NUMBER;
	return LEPT_PARSE_OK;
}

void lept_free(lept_value * v)
{/*卧槽 妙啊 叶大 牛逼 2020年11月2日17:49:48*/
	size_t i;
	assert(v != NULL);
	switch (v->type)
	{
		case LEPT_STRING:
			free(v->u.s.s);
			break;
		case LEPT_ARRAY:
			for (i = 0; i < v->u.a.size; i++)
				lept_free(&v->u.a.e[i]);
			free(v->u.a.e);
			break;
		default: break;
	}
	v->type = LEPT_NULL;
}

void lept_set_string(lept_value * v, const char* s, size_t len)
{
	assert(v != NULL && (s != NULL || len == 0));
	lept_free(v);
	v->u.s.s = (char*)malloc(len + 1);//创建
	memcpy(v->u.s.s,s,len);           //内存复制字符串
	v->u.s.s[len] = '\0';             //补0
	v->u.s.len = len;                 //修改长度
	v->type = LEPT_STRING;            //修改类型
}

static int lept_parse_array(lept_context* c, lept_value* v)
{
	//逗号和逗号之间也是可以有空格的  ["aaaa" , 1.111]  is ok 
	size_t size = 0;//array size
	int ret;
	EXPECT(c,'[');
	lept_parse_whitespace(c);
	if (*c->json == ']')
	{
		c->json++;
		v->type = LEPT_ARRAY;
		v->u.a.size = 0;
		v->u.a.e = NULL;
		return LEPT_PARSE_OK;
	}

	for (;;)
	{
		lept_value e;
		lept_init(&e);
		if ((ret = lept_parse_value(c, &e)) != LEPT_PARSE_OK)
			break;
		memcpy(lept_context_push(c, sizeof(lept_value)), &e, sizeof(lept_value));
		++size;
		lept_parse_whitespace(c);

		//lept_value* e = (lept_value*)lept_context_push(c, sizeof(lept_value));
		//lept_init(e);
		//size++;
		//if ((ret = lept_parse_value(c, e)) != LEPT_PARSE_OK)
		//	return ret;

		if (*c->json == ',')
			do { c->json++; lept_parse_whitespace(c); } while (0);
		else if (*c->json == ']')
		{
			c->json++;
			v->type = LEPT_ARRAY;
			v->u.a.size = size;
			size *= sizeof(lept_value);//需要移动多少个字节 sizeof(lept_value) * size
			memcpy(v->u.a.e = (lept_value*)malloc(size), lept_context_pop(c, size), size);
			return LEPT_PARSE_OK;
		}
		else
		{
			ret = LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
			break;
		}	
	}
	for (size_t i = 0; i < size; i++)
		//每次从栈上弹出sizeof(lept_value)大小内存 转成lept_value* 然后lept_free();
		lept_free(   (lept_value*)lept_context_pop(c, sizeof(lept_value))   );
	return ret;
}

int lept_parse_value(lept_context * c,lept_value * v)
{
	switch (*c->json)
	{
		case 'n':return lept_parse_null(c,v);
	    case 'f':return lept_parse_false(c, v);
		case 't':return lept_parse_true(c, v);
		case '"':  return lept_parse_string(c, v);
		case '[': return lept_parse_array(c,v);

		default:return lept_parse_number(c, v);//0 

		case'\0':return LEPT_PARSE_EXPECT_VALUE;
		//case 'n': return lept_parse_literal(c, v, "null", LEPT_NULL);
		//case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);
		//case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);
	}
}

//lept_parse(&v,"0x0")
int lept_parse(lept_value * v, const char * json)
{
	lept_context c;
	int ret;
	assert(v != NULL);
	c.json = json;
	 
	c.stack = NULL;          /*初始化栈*/
	c.capacity = c.top = 0;  /*初始化栈*/

	lept_init(v);//改v->type为LEPT_NULL

	lept_parse_whitespace(&c);//删除空格
	if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK)
	{
		lept_parse_whitespace(&c);
		if (*c.json != '\0')
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
	}

	//assert(c.top == 0);
	free(c.stack);
	return ret;
}

lept_type lept_get_type(const lept_value* v)
{
	assert(v != NULL);
	return v->type;
}

double lept_get_number(const lept_value* v)
{
	//使用者应确保类型正确，才调用此API,使用断言来保证
	assert(v != NULL && v->type == LEPT_NUMBER);
	return v->u.n;
}

void lept_set_number(lept_value* v, double n)
{
	lept_free(v);
	v->u.n = n;
	v->type = LEPT_NUMBER;
}

const char * lept_get_string(const lept_value* v)
{
	//使用者应确保类型正确，才调用此API,使用断言来保证
	assert(v != NULL && v->type == LEPT_STRING);
	return v->u.s.s;
}

size_t lept_get_string_length(const lept_value* v)
{
	assert(v != NULL && v->type == LEPT_STRING);
	return v->u.s.len;
}

int lept_get_boolean(const lept_value* v)
{
	assert(v != NULL && (v->type == LEPT_FALSE || v->type == LEPT_TRUE));
	return v->type == LEPT_TRUE;

}

void lept_set_boolean(lept_value* v, int b)
{
	lept_free(v);
	v->type =( b ? LEPT_TRUE : LEPT_FALSE);
}

size_t lept_get_array_size(const lept_value* v)
{
	assert(v != NULL && v->type == LEPT_ARRAY);
	return v->u.a.size;
}

lept_value* lept_get_array_element(const lept_value* v, size_t index)
{
	assert(v != NULL && v->type == LEPT_ARRAY);
	assert(index < v->u.a.size);
	return &(v->u.a.e[index]);  //&v->u.a.e[index]
}
