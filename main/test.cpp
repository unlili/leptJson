



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"

/*-----------------下面的是测试用的,以后delete-----------------*/
#include<iostream>
using namespace std;
static int tes = 0;

/*-----------------上面的是测试用的,以后delete-----------------*/

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        ++test_count;\
        if (equality)\
            ++test_pass;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")  //%d会替换format
#define EXPECT_EQ_DOUBLE(expect,actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")
#define EXPECT_EQ_STRING(expect,actual,alength) EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength) == 0, expect, actual, "%s")
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false", "%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false", "true", "%s")


#if defined(_MSC_VER)
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%Iu")
#else
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%zu")
#endif

//这个宏可以用来修改test_parse_invalid_value的四句重复语句
#define TEST_ERROR(error,json)\
		do{\
			lept_value v;\
			v.type = LEPT_FALSE;\
			EXPECT_EQ_INT(error,lept_parse(&v,json));\
			EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));\
		}while(0)


//这个宏用于测试解析number
#define TEST_NUMBER(expect, json)\
    do {\
        lept_value v;\
		lept_init(&v);\
        EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, json));\
        EXPECT_EQ_INT(LEPT_NUMBER, lept_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect, lept_get_number(&v));\
		lept_free(&v);\
    } while(0)

#define TEST_STRING(expect,json)\
	do {\
		lept_value v;\
		lept_init(&v);\
		EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, json));\
        EXPECT_EQ_INT(LEPT_STRING, lept_get_type(&v));\
        EXPECT_EQ_STRING(expect,lept_get_string(&v),lept_get_string_length(&v));\
		lept_free(&v);\
	}while(0)

#define TEST_ROUNDTRIP(json)\
	do {\
		lept_value v;\
		char * json2;\
		size_t length;\
		lept_init(&v);\
		EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, json));\
		EXPECT_EQ_INT(LEPT_STRINGIFY_OK, lept_stringify(&v, &json2, &length));\
		EXPECT_EQ_STRING(json, json2, length);\
		lept_free(&v);\
        free(json2);\
	}while(0)

//-------------------------每一种测试函数-------------------------


static void test_parse_object() {
	lept_value v;
	size_t i;

	lept_init(&v);
	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, " { } "));
	EXPECT_EQ_INT(LEPT_OBJECT, lept_get_type(&v));

	EXPECT_EQ_SIZE_T(0, lept_get_object_size(&v));
	lept_free(&v);

	lept_init(&v);
	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v,
		" { "
		"\"n\" : null , "
		"\"f\" : false , "
		"\"t\" : true , "
		"\"i\" : 123 , "
		"\"s\" : \"abc\", "
		"\"a\" : [ 1, 2, 3 ],"
		"\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
		" } "
		));
	EXPECT_EQ_INT(LEPT_OBJECT, lept_get_type(&v));
	EXPECT_EQ_SIZE_T(7, lept_get_object_size(&v));
	EXPECT_EQ_STRING("n", lept_get_object_key(&v, 0), lept_get_object_key_length(&v, 0));
	EXPECT_EQ_INT(LEPT_NULL, lept_get_type(lept_get_object_value(&v, 0)));
	EXPECT_EQ_STRING("f", lept_get_object_key(&v, 1), lept_get_object_key_length(&v, 1));
	EXPECT_EQ_INT(LEPT_FALSE, lept_get_type(lept_get_object_value(&v, 1)));
	EXPECT_EQ_STRING("t", lept_get_object_key(&v, 2), lept_get_object_key_length(&v, 2));
	EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(lept_get_object_value(&v, 2)));
	EXPECT_EQ_STRING("i", lept_get_object_key(&v, 3), lept_get_object_key_length(&v, 3));
	EXPECT_EQ_INT(LEPT_NUMBER, lept_get_type(lept_get_object_value(&v, 3)));
	EXPECT_EQ_DOUBLE(123.0, lept_get_number(lept_get_object_value(&v, 3)));
	EXPECT_EQ_STRING("s", lept_get_object_key(&v, 4), lept_get_object_key_length(&v, 4));
	EXPECT_EQ_INT(LEPT_STRING, lept_get_type(lept_get_object_value(&v, 4)));
	EXPECT_EQ_STRING("abc", lept_get_string(lept_get_object_value(&v, 4)), lept_get_string_length(lept_get_object_value(&v, 4)));
	EXPECT_EQ_STRING("a", lept_get_object_key(&v, 5), lept_get_object_key_length(&v, 5));
	EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(lept_get_object_value(&v, 5)));
	EXPECT_EQ_SIZE_T(3, lept_get_array_size(lept_get_object_value(&v, 5)));
	for (i = 0; i < 3; i++) {
		lept_value* e = lept_get_array_element(lept_get_object_value(&v, 5), i);
		EXPECT_EQ_INT(LEPT_NUMBER, lept_get_type(e));
		EXPECT_EQ_DOUBLE(i + 1.0, lept_get_number(e));
	}
	EXPECT_EQ_STRING("o", lept_get_object_key(&v, 6), lept_get_object_key_length(&v, 6));
	{
		lept_value* o = lept_get_object_value(&v, 6);
		EXPECT_EQ_INT(LEPT_OBJECT, lept_get_type(o));
		for (i = 0; i < 3; i++) {
			lept_value* ov = lept_get_object_value(o, i);
			EXPECT_TRUE('1' + i == lept_get_object_key(o, i)[0]);
			EXPECT_EQ_SIZE_T(1, lept_get_object_key_length(o, i));
			EXPECT_EQ_INT(LEPT_NUMBER, lept_get_type(ov));
			EXPECT_EQ_DOUBLE(i + 1.0, lept_get_number(ov));
		}
	}
	lept_free(&v);
}

static void test_parse_array() {
	size_t i, j;
	lept_value v;
	lept_init(&v);

	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "[]"));
	EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(&v));
	EXPECT_EQ_SIZE_T(0, lept_get_array_size(&v));

	lept_parse(&v, "[ null , false , true , 123 , \"abc\" ]");
	EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(&v));
	lept_value * ele0 = lept_get_array_element(&v,0);
	EXPECT_EQ_INT(LEPT_NULL, lept_get_type(ele0));
	lept_value * ele1 = lept_get_array_element(&v, 1);
	EXPECT_EQ_INT(LEPT_FALSE, lept_get_type(ele1));
	lept_value * ele2 = lept_get_array_element(&v, 2);
	EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(ele2));
	lept_value * ele3 = lept_get_array_element(&v, 3);
	EXPECT_EQ_INT(LEPT_NUMBER, lept_get_type(ele3));

	lept_value * ele4 = lept_get_array_element(&v, 4);
	EXPECT_EQ_STRING("abc", lept_get_string(ele4), lept_get_string_length(ele4));
	EXPECT_EQ_INT(LEPT_STRING, ele4->type);
	lept_set_string(ele4, "Go?ahead", sizeof("Go?ahead")-1);
	EXPECT_EQ_STRING("Go?ahead", lept_get_string(ele4), lept_get_string_length(ele4));
	lept_free(&v);

	lept_parse(&v, "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
	EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(&v));
	EXPECT_EQ_SIZE_T(4, lept_get_array_size(&v));
	for (i = 0; i < 4; i++) {
		lept_value* a = lept_get_array_element(&v, i);
		EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(a));
		EXPECT_EQ_SIZE_T(i, lept_get_array_size(a));
		for (j = 0; j < i; j++) {
			lept_value* e = lept_get_array_element(a, j);
			EXPECT_EQ_INT(LEPT_NUMBER, lept_get_type(e));
			EXPECT_EQ_DOUBLE((double)j, lept_get_number(e));
		}
	}

	lept_free(&v);/*不想写释放了*/lept_free(ele0);lept_free(ele1);lept_free(ele2);lept_free(ele3);

}
//测试"" "  "
static void test_parse_expect_value() {
	lept_value v;

	v.type = LEPT_FALSE;
	EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, lept_parse(&v, ""));
	EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));

	v.type = LEPT_FALSE;
	EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, lept_parse(&v, " "));
	EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}
//测试无效的值
static void test_parse_invalid_value()
{

	TEST_ERROR(LEPT_PARSE_INVALID_VALUE,"nul");

	TEST_ERROR(LEPT_PARSE_INVALID_VALUE,"   dfgsdf");
	/* invalid number */

	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+0");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+1");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, ".123");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "1.");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "INF");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "inf");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "NAN");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nan");

	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "00.1");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "-00.1");

	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "0123");


	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "[1,]");
	TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "[\"a\", nul]");


}
//测试一个值之后还有空格和东西
static void test_parse_root_not_singular() 
{
	TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "null x");

	TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x0");
	TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x123");

}

static void test_parse_number_too_big()
{

	TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "1e309");
	TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "-1e309");

	/* the smallest number > 1 */
	TEST_NUMBER(1.0000000000000002, "1.0000000000000002");
	/* minimum denormal */
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324");
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	/* Max subnormal double */
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	/* Min normal positive double */
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	/* Max double */
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");

}
//测试"true"
static void test_parse_true()
{
	lept_value v;
	v.type = LEPT_FALSE;
	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "true"));
	EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(&v));
}
//测试"false"
static void test_parse_false()
{
	lept_value v;
	v.type = LEPT_TRUE;
	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "false"));
	EXPECT_EQ_INT(LEPT_FALSE, lept_get_type(&v));
}
//测试"null"
static void test_parse_null()
{
	/*
	lept_parse 会修改v.type的类型
	lept_get_type 会获得v.type
	*/
	lept_value v;
	v.type = LEPT_TRUE;
	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "null"));
	EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}
//测试number
static void test_parse_number()
{

	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5,"1.5" );
	TEST_NUMBER(-1.5,"-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

	TEST_NUMBER(1.79769e+308, "1.79769e+308"); //double 最大值
	TEST_NUMBER(2.22507e-308, "2.22507e-308"); //double 最小值

}

static void test_parse_string() {
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");

	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

	TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
	TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
	TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
	TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}
//测试字符串
static void test_access_string()
{
	lept_value v;
	lept_init(&v);
	lept_set_string(&v,"",0);
	EXPECT_EQ_STRING("",lept_get_string(&v),lept_get_string_length(&v));
	lept_set_string(&v, "hello", 5);
	EXPECT_EQ_STRING("hello", lept_get_string(&v), lept_get_string_length(&v));
	lept_free(&v);
}


//字符串缺少引号
static void test_parse_missing_quotation_mark() {
	//TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"");
	TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {

	TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
}

static void test_parse_invalid_string_char() {
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x01\"");
	TEST_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
}

static void test_access_null() {
	lept_value v;
	lept_init(&v);
	lept_set_string(&v, "a", 1);
	lept_set_null(&v);
	EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
	lept_free(&v);
}

static void test_access_boolean() {
	lept_value v;
	lept_init(&v);
	lept_set_string(&v, "a", 1);
	lept_set_boolean(&v,1);
	EXPECT_TRUE(lept_get_boolean(&v));
	lept_set_boolean(&v, 0);
	EXPECT_FALSE(lept_get_boolean(&v));
	lept_free(&v);

}

static void test_access_number() {
	lept_value v;
	lept_init(&v);
	lept_set_string(&v, "a", 1);
	lept_set_number(&v,2.3333);
	EXPECT_EQ_DOUBLE(2.3333,lept_get_number(&v));
	lept_free(&v);
}
//\uXXXX 后不是 4 位十六进位数字
static void test_parse_invalid_unicode_hex() {
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u00/0\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
}

//只有高代理项而欠缺低代理项，或是低代理项不在合法码点范围
static void test_parse_invalid_unicode_surrogate() {
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
	TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
}

static void test_parse_miss_comma_or_square_bracket() 
{

	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");

}

static void test_parse_miss_key() 
{
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{1:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{true:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{false:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{null:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{[]:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{{}:1,");
	TEST_ERROR(LEPT_PARSE_MISS_KEY, "{\"a\":1,");
}

static void test_parse_miss_colon() 
{
	TEST_ERROR(LEPT_PARSE_MISS_COLON, "{\"a\"}");
	TEST_ERROR(LEPT_PARSE_MISS_COLON, "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket() 
{
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1 \"b\"");
	TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
}

static void test_stringify_number() {
	TEST_ROUNDTRIP("0");
	TEST_ROUNDTRIP("-0");
	TEST_ROUNDTRIP("1");
	TEST_ROUNDTRIP("-1");
	TEST_ROUNDTRIP("1.5");
	TEST_ROUNDTRIP("-1.5");
	TEST_ROUNDTRIP("3.25");
	TEST_ROUNDTRIP("1e+20");
	TEST_ROUNDTRIP("1.234e+20");
	TEST_ROUNDTRIP("1.234e-20");

	TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
	TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
	TEST_ROUNDTRIP("-4.9406564584124654e-324");
	TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
	TEST_ROUNDTRIP("-2.2250738585072009e-308");
	TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
	TEST_ROUNDTRIP("-2.2250738585072014e-308");
	TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
	TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void test_stringify_string() {
	TEST_ROUNDTRIP("\"\"");
	TEST_ROUNDTRIP("\"Hello\"");
	TEST_ROUNDTRIP("\"Hello\\nWorld\"");
	TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
	TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void test_stringify_array() {
	TEST_ROUNDTRIP("[]");
	TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void test_stringify_object() {
	TEST_ROUNDTRIP("{}");
	TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}


static void test_stringify() {
	TEST_ROUNDTRIP("null");
	TEST_ROUNDTRIP("false");
	TEST_ROUNDTRIP("true");
	/* 往返（roundtrip）测试 */
	test_stringify_number();
	test_stringify_string();
	test_stringify_array();
	test_stringify_object();
}

static void test_find()
{
	lept_value o,*v;
	size_t index;
	lept_init(&o);
	lept_parse(&o, "{\"name\":\"Milo\", \"gender\":\"M\"}");
	index = lept_find_object_index(&o, "name", 4);


	cout << index << endl;

	if (index != LEPT_KEY_NOT_EXIST) {
		lept_value* v = lept_get_object_value(&o, index);
		printf("%s\n", lept_get_string(v));
	}

	if ((v = lept_find_object_value(&o, "name", 4)) != NULL)
		printf("%s\n", lept_get_string(v));

	lept_free(&o);

	lept_value a, b;
	lept_parse(&a, "{\"name\":\"Miloaa\", \"gender\":\"M\"}");
	lept_parse(&b, "{\"name\":\"Milo\", \"gender\":\"M\"}");
	if (lept_is_equal(&a, &b))
		cout << "相等" << endl;
	else
		cout << "不相等" << endl;

	lept_parse(&a, "{\"name\":\"Milo\", \"gender\":\"M\"}");
	lept_parse(&b, "{\"name\":\"Milo\", \"gender\":\"M\"}");
	if (lept_is_equal(&a, &b))
		cout << "相等" << endl;
	else
		cout << "不相等" << endl;

	lept_parse(&a, "{ \"nae\":\"Milo\" }");
	lept_parse(&b, "{ \"name\":\"Milo\" }");
	if (lept_is_equal(&a, &b))
		cout << "相等" << endl;
	else
		cout << "不相等" << endl;
}

static void test_set_object()
{
	const char* json = "{\"a\":[1,2],\"b\":3}";//创建一个json
	char *out;       
	lept_value v;
	lept_init(&v);
	lept_parse(&v, json);//解析json到v

	lept_copy(
		lept_find_object_value(&v, "b", 1),
		lept_find_object_value(&v, "a", 1));

	printf("%s\n", out = lept_stringify(&v, NULL)); /* {"a":[1,2],"b":[1,2]} */

	free(out);

	lept_parse(&v, json);
	lept_move(
		lept_find_object_value(&v, "b", 1),
		lept_find_object_value(&v, "a", 1));
	printf("%s\n", out = lept_stringify(&v, NULL)); /* {"a":null,"b":[1,2]} */
	free(out);

	lept_parse(&v, json);
	lept_swap(
		lept_find_object_value(&v, "b", 1),
		lept_find_object_value(&v, "a", 1));
	printf("%s\n", out = lept_stringify(&v, NULL)); /* {"a":3,"b":[1,2]} */
	free(out);

	lept_free(&v);
}

/* ... */
static void test_parse() {

	test_parse_null();	
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_string();
	test_parse_array();
	test_parse_object();

	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();

	test_parse_number_too_big();
	test_parse_missing_quotation_mark();

	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();
	test_parse_invalid_unicode_hex();/*!!!!!!!!!!!!!!!!!*/
	test_parse_invalid_unicode_surrogate();	

	test_parse_miss_comma_or_square_bracket();

	//test_parse_miss_key();
	//test_parse_miss_colon();
	//test_parse_miss_comma_or_curly_bracket();
}


static void test_access()
{
	test_access_null();
	test_access_boolean();
	test_access_number();
	test_access_string();
}


int main() {

#ifdef _WINDOWS
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	//test_parse();
	//test_stringify();
	//test_access();
	//printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);

	test_find();

	//test_parse_array();
	//printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);

	//static const char hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	//char a = '1';
	//cout << (int)(unsigned char)(a) << endl;
	//cout << (a >> 4) << endl;
	//cout << hex_digits[a >> 4] << endl;

	system("pause");
	return main_ret;
}
