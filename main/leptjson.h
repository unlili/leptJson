/*
lepton n. 轻粒子（希腊的货币单位，古希腊的最小硬币）



*/
#ifndef LEPTJSON_H__
#define LEPTJSON_H__
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>

//json 所有类型
typedef enum {LEPT_NULL,LEPT_FALSE,LEPT_TRUE,LEPT_NUMBER,LEPT_STRING,LEPT_ARRAY,LEPT_OBJECT} lept_type;

//解析的返回值,OK返回LEPT_PARSE_OK
enum {
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,       //一个JSON只含有空白
	LEPT_PARSE_INVALID_VALUE,      //
	LEPT_PARSE_ROOT_NOT_SINGULAR   //若一个值之后，在空白之后还有其他字符
};

//解析成树后的每一个节点,里面包含该节点的类型
typedef struct {
	lept_type type;
}lept_value;

int lept_parse(lept_value * v,const char * json); //是解析JSON

lept_type lept_get_type(const lept_value* v);//访问结果

#endif