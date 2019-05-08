#ifndef myJSON__h
#define myJSON__h

#ifdef __cplusplus
extern "C"
{
#endif
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * 通用编译环境
 */
#if !defined(__WINDOWS__) && ( defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32) )
    #define __WINDOWS__
#endif

#if !defined(__SYSENV32__) && ( defined(WIN32) || defined(_WIN32) || __SIZEOF_POINTER__ == 4 )
    #define __SYSENV32__
#endif

#include <stdint.h>

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

typedef enum {false = 0, true = 1} JSON_Boolean;
typedef enum {
    JSONType_Invalid  = 0,     //0000

    JSONType_NULL     = 0x01,  //0001
    JSONType_Boolean  = 0x02,  //0010
    JSONType_Integer  = 0x04,  //0100
    JSONType_Float    = 0x08,  //1000

    JSONType_Bytes    = 0x10,  //0000 0001 0000
    JSONType_String   = 0x20,  //0000 0010 0000
    JSONType_Raw      = 0x40,  //0000 0100 0000

    JSONType_Array    = 0x80,  //0000 1000 0000
    JSONType_Object   = 0x100, //0001 0000 0000

    //分解线
    JSONType_Dividing = 0x1FF, //0001 1111 1111

    //Reference Value 不需要 free
    JSONType_IsRefValue    = 0x200, //0010 0000 0000
    JSONType_IsConstKey    = 0x400  //0100 0000 0000
} JSONType;

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * @def  隐式定义myJSON
 * @file JSON_Bytes.inl
 */
typedef struct JSONCell myJSON;

/*!
 * JSONBytes
 */
typedef struct {
    union {
        void*       address;
        const void* Lbound;
        const char* string;
        unsigned char* probe;
    };
    union {
        const void* Ubound;
        unsigned char* otail; //OpenTail
    };
    size_t length; //length = Ubound - Lbound
} JSONBytes;

/*!
 * @note JSONCell的结构镜像
 * @file myJSON.h
 */
typedef struct {
    const void*const seat[3];
    const int Type;
    const JSONBytes*const Key;
    union {
        const JSONBytes*const VBytes;
        const int64_t VInteger;
        const double VDouble;
        const JSON_Boolean VBoolean;
    };
} JSONElement;
typedef void(*JSON_EnumCallback)(const JSONElement*, int);

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

typedef struct {
    const char* (*Version)(void);
    //==========================================================
    void (*EnumData)(const myJSON*, JSON_EnumCallback callback);
    JSON_Boolean (*Print)(const myJSON*, JSON_Boolean, FILE*);
    //==========================================================
    myJSON* (*Parse)(const char* str_json, size_t str_size);
    myJSON* (*ParseFile)(const char* filename);
    //==========================================================
    myJSON* (*CreateNull)(void);
    myJSON* (*CreateBoolean)(JSON_Boolean);
    myJSON* (*CreateInteger)(int64_t num);
    myJSON* (*CreateFloat)(double num);

    myJSON* (*CreateArray)(void);
    myJSON* (*CreateObject)(void);
    myJSON* (*CreateString)(const char*);
    myJSON* (*CreateRaw)(const char*);

    myJSON* (*CreateArrayReference)(const myJSON*);
    myJSON* (*CreateObjectReference)(const myJSON*);
    myJSON* (*CreateStringReference)(const char*);

    void (*Remove)(myJSON* item);
    //==========================================================
    JSON_Boolean (*AddItemToArray)(myJSON*, myJSON*);
    JSON_Boolean (*AddItemToObject)(myJSON* const, const char* const, myJSON* const, const JSON_Boolean);
    //==========================================================

} JSONMethods;
/*!
 * @note 返回JSON操作方法集
 * @file myJSON.c
 */
extern const JSONMethods* const
    JSON_GetMethods();

/*
//JSONMethodsEntity
const JSONMethods* const JMER = JSON_GetMethods();
const JSONMethods JME = *JSON_GetMethods();
*/

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
#ifdef __cplusplus
}
#endif

#endif // myJSON__h
