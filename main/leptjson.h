/*
lepton n. �����ӣ�ϣ���Ļ��ҵ�λ����ϣ������СӲ�ң�



*/
#ifndef LEPTJSON_H__
#define LEPTJSON_H__
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>

//json ��������
typedef enum {LEPT_NULL,LEPT_FALSE,LEPT_TRUE,LEPT_NUMBER,LEPT_STRING,LEPT_ARRAY,LEPT_OBJECT} lept_type;

//�����ķ���ֵ,OK����LEPT_PARSE_OK
enum {
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,       //һ��JSONֻ���пհ�
	LEPT_PARSE_INVALID_VALUE,      //
	LEPT_PARSE_ROOT_NOT_SINGULAR   //��һ��ֵ֮���ڿհ�֮���������ַ�
};

//�����������ÿһ���ڵ�,��������ýڵ������
typedef struct {
	lept_type type;
}lept_value;

int lept_parse(lept_value * v,const char * json); //�ǽ���JSON

lept_type lept_get_type(const lept_value* v);//���ʽ��

#endif