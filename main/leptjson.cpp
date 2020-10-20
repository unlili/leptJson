#include"leptjson.h"

//判断c的第一个字符是不是ch,是的话c下移,不是的话程序崩溃
#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)

//把函数之间的传递数据放到一个结构体里,方便
typedef struct { const char * json; }lept_context;

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
//
static int lept_parse_literal(lept_context * c, lept_value * v,char * str, lept_type l)
{
#if 0
	   判断c->json是不是等于str
		   是   c->json后移   修改v->type
		   不是  return LEPT_PARSE_INVALID_VALUE
		
	   case 'n': return lept_parse_literal(c, v, "null", LEPT_NULL);
	   case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);
	   case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);
		       0
	c-json null x
	str	   null
	           0
			    
#endif
		   while (*c->json == *str)
		   {
			   ++c->json;
			   ++str;
			   if (*str == '\0')
			   {
				   v->type = l;
				   return LEPT_PARSE_OK;
			   }
		   }
		   return LEPT_PARSE_INVALID_VALUE;
}

static int lept_parse_number(lept_context * c, lept_value * v)
{
	char * end;
	v->n = strtod(c->json,&end);//把字符串转换为double,多于的字符串放到end后面,

	if (c->json == end) //上面那一步没用的话 
		return LEPT_PARSE_INVALID_VALUE;

	//strtod成功 
	c->json = end;
	v->type = LEPT_NUMBER;
	return LEPT_PARSE_OK;
}


int lept_parse_value(lept_context * c,lept_value * v)
{
	switch (*c->json)
	{
		case 'n':return lept_parse_null(c,v);
		case 'f':return lept_parse_false(c, v);
		case 't':return lept_parse_true(c, v);
		default:return lept_parse_number(c, v);

		case'\0':return LEPT_PARSE_EXPECT_VALUE;


		//case 'n': return lept_parse_literal(c, v, "null", LEPT_NULL);
		//case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);
		//case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);
	}
}

int lept_parse(lept_value * v, const char * json)
{
	lept_context c;
	int ret;
	assert(v != NULL);
	c.json = json;
	v->type = LEPT_NULL;

	lept_parse_whitespace(&c);

	if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK)
	{
		lept_parse_whitespace(&c);
		if (*c.json != '\0')
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
	}

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
	return v->n;
}

