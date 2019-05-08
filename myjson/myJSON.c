#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <limits.h>
//#include <ctype.h>
//#include <stddef.h>
//#include <stdint.h>
#include "myJSON.h"

/*!
 * 编译环境
 */

//#define __debug

#if !defined(__WINDOWS__) && ( defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32) )
    #define __WINDOWS__
#endif

#if !defined(__SYSENV32__) && ( defined(WIN32) || defined(_WIN32) || __SIZEOF_POINTER__ == 4 )
    #define __SYSENV32__
#endif

/*!
 * Keywords
 */
static void* cast_away_const(const void*);
#define cast(_t,_v)  (_t)(_v)

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*
 * [Bytes]
 */
#include "JSON_Bytes.inl.h"

/*
 * [版本情况]
 */
const char* JSON_Version() {
    return "1.0.0";
}

/*
 * [数据打印]
 */
#include "JSON_Print.inl.h"

/*
 * [操作对象]
 */
#include "JSON_Item.inl.h"
#include "JSON_Create.inl.h"

/*
 * [解析文本]
 */
#include "JSON_Parse.inl.h"

/*
 * [方法集]
 */

#include "JSON_Methods.inl.h"
