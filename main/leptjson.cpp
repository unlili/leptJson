#include"leptjson.h"

//�ж�c�ĵ�һ���ַ��ǲ���ch,�ǵĻ�c����,���ǵĻ��������
#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)

//�Ѻ���֮��Ĵ������ݷŵ�һ���ṹ����,����
typedef struct {
	const char* json;
}lept_context;

//����whitespace  ȥ���ո�
void lept_parse_whitespace(lept_context * c)
{
	const char * p = c->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		++p;
	c->json = p;
}

static int lept_parse_null(lept_context * c, lept_value * v)
{
	EXPECT(c,'n');
	if (c->json[1] != 'u' || c->json[2] != 'l' || c->json[2] != 'l')
		return LEPT_PARSE_INVALID_VALUE;
	c->json += 3;
	v->type = LEPT_NULL;
	return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context * c, lept_value * v)
{
	EXPECT(c, 'f');
	if (c->json[1] != 'a' || c->json[2] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
		return LEPT_PARSE_INVALID_VALUE;
	c->json += 4;
	v->type = LEPT_FALSE;
	return LEPT_PARSE_OK;
}

static int lept_parse_true(lept_context * c, lept_value * v)
{
	EXPECT(c, 't');
	if (c->json[1] != 'r' || c->json[2] != 'u' || c->json[2] != 'e' )
		return LEPT_PARSE_INVALID_VALUE;
	c->json += 3;
	v->type = LEPT_TRUE;
	return LEPT_PARSE_OK;
}

int lept_parse_value(lept_context * c,lept_value * v)
{
	switch (*c->json)
	{
		case'\0':return LEPT_PARSE_EXPECT_VALUE;
		case 'n':return lept_parse_null(c,v);
		case 'f':return lept_parse_false(c, v);
		case 't':return lept_parse_true(c, v);

		default:return LEPT_PARSE_INVALID_VALUE;
	}
}

int lept_parse(lept_value * v, const char * json)
{
	lept_context c;
	assert(v != NULL);

	c.json = json;
	v->type = LEPT_NULL;
	lept_parse_whitespace(&c);
	return lept_parse_value(&c, v);
}



lept_type lept_get_type(const lept_value* v)
{

}