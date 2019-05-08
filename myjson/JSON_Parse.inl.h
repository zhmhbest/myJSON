
typedef struct {
    const unsigned char* content;
    JSONBytes; //probe otail length
    size_t depth;
} JSONParseBuffer;
#define JSONM_BufferInitial(_Ebuff, _content, _length)  \
    (_Ebuff).content = (const unsigned char*)_content;  \
    (_Ebuff).Lbound  = (_Ebuff).content;                \
    (_Ebuff).Ubound  = (_Ebuff).content + _length;      \
    (_Ebuff).length  =  _length;                        \
    (_Ebuff).depth = 0;

/* OpenTail = URMP(Unreachable Max Probe) : 总满足：probe＜此值。
 * Tail                                   : 总满足：probe≤此值。
#define JSONM_BufferURMP(_buff)     ( (_buff->content) + (_buff->length)     )
#define JSONM_BufferTail(_buff)     ( (_buff->content) + (_buff->length) - 1 )
 */
#define JSONM_BufferURMP(_buff)     (_buff->otail)
#define JSONM_BufferTail(_buff)     (_buff->otail-1)

#define JSONM_BufferUsedSize(_buff)         ( (_buff->probe)          - (_buff->content) )
#define JSONM_BufferUseableSize(_buff)      ( JSONM_BufferURMP(_buff) - (_buff->probe)   )
/* UseableSize = length - ( probe - content ); *
 *             = OpenTail - probe;             */

#define JSONM_BufferIsEmpty(_buff)          ( (_buff->probe) == (_buff->content) )
#define JSONM_BufferIsFull(_buff)           ( (_buff->probe) == JSONM_BufferTail(_buff) )
#define JSONM_BufferIsOverflow(_buff)       ( (_buff->probe) >= JSONM_BufferURMP(_buff) )

//!Buffer探针 是否到头。
/*
 * 剩余可操作单元
 */
#define JSONM_BufferIsRemaining(_buff)                  ( JSONM_BufferUseableSize(_buff) > 0     )
#define JSONM_BufferIsAdequate( _buff, _size)           ( JSONM_BufferUseableSize(_buff) > _size )
#define JSONM_BufferCanReadOne(_buff)                   JSONM_BufferIsRemaining(_buff)              /*当前Probe是否可用*/
#define JSONM_BufferCanRead(_buff, _size)               JSONM_BufferIsAdequate( _buff, _size)       /*除当前Probe外还需要Size个*/

//!Buffer探针 前进、比较
#define JSONM_BufferForward(_buff, _size)               ( (_buff->probe) += _size )
#define JSONM_BufferCompareStr(_buff, _str, _strsize)   ( 0 == strncmp((const char*)(_buff->probe), _str, _strsize) )
#define JSONM_BufferCompareCha(_buff, _cha)             ( _cha == *(_buff->probe) )

//!Check && Compare String
#define JSONM_BufferCkCS(_buff, _str, _strsize)         ( JSONM_BufferCanRead(_buff, _strsize) && JSONM_BufferCompareStr(_buff, _str, _strsize) )
//!Check && Compare Char
#define JSONM_BufferCkCC(_buff, _cha)                   ( JSONM_BufferCanReadOne(_buff)        && JSONM_BufferCompareCha(_buff, _cha) )

//!Number
#define JSONM_NumLowerCase(_E)                      ( _E | 32 )
#define JSONM_NumBelongTo(_E, _rangeL, _rangeR)     ( _E >= _rangeL && _E <= _rangeR )
#define JSONM_NumIsNumber(_E)                       ( JSONM_NumBelongTo(_E, '0', '9') || JSONM_NumBelongTo(_E, 'a', 'f') || JSONM_NumBelongTo(_E, 'A', 'F') )
#define JSONM_NumParseChar(_E)                      ( JSONM_NumBelongTo(_E, '0', '9') ? (_E - '0') : (JSONM_NumLowerCase(_E) + (10 - 'a')) )


#define BufferContent   (buffer->content)
#define BufferLength    (buffer->length)
#define BufferProbe     (buffer->probe)
#define BufferDepth     (buffer->depth)

typedef enum {
    JPSI_WeightBin  =  2,
    JPSI_WeightOct  =  8,
    JPSI_WeightDec  = 10,
    JPSI_WeightHex  = 16,

    JPSI_Sign = -100,
    JPSI_PositiveNumber,
    JPSI_NegativeNumber,
    JPSI_EndOfNumber,
    JPSI_NextDecimal,
    JPSI_NextPower,

    JPSI_EndOfString,

    //==============================
    JPOE_OnError            = 500,
    JPOE_AllocateError,
    JPOE_NullBuffer,
    JPOE_InsufficientBuffer,
    JPOE_UnexpectedChar,
    JPOE_NoTail,

    JPOE_ObjKey,
    JPOE_ObjValue,

    JPOE_ArrValue,

    JPOE_StrEndOfBackslash,
    JPOE_StrEndOfX,
    JPOE_StrErrorX,
    JPOE_StrEndOfU,
    JPOE_StrErrorU,

} JSONParse_Sign, JSONParse_OnError;
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * @ref buffer_skip_whitespace
 */
static JSON_Boolean __JSON_SkipWhiteSpace(JSONParseBuffer* buffer) {
    if(JSONM_IsNull(buffer) || JSONM_IsNull(buffer->content)) {
        return false;
    }
    while( JSONM_BufferCanReadOne(buffer) && *BufferProbe <= 32 ) {
        JSONM_BufferForward(buffer, 1);
    }
    return true;
}

/*!
 * Skip the UTF-8 BOM (byte order mark) if it is at the beginning of a buffer
 * @ref skip_utf8_bom
 */
static JSON_Boolean __JSON_SkipUtf8Bom(JSONParseBuffer* buffer) {
    if( JSONM_IsNull(buffer) || JSONM_IsNull(buffer->content) || !JSONM_BufferIsEmpty(buffer) ) {
        return false;
    }
    if(JSONM_BufferCanRead(buffer, 4) && JSONM_BufferCompareStr(buffer, "\xEF\xBB\xBF",3)) {
        JSONM_BufferForward(buffer, 3);
    }
    return true;
}


/*!
 * 报告错误位置
 */
static void __JSON_ReportIndex(JSONParseBuffer* buffer) {
    int error_line = 1;
    const unsigned char* error_probe = BufferContent;
    const unsigned char* error_begin = error_probe;
    while(error_probe <= BufferProbe) {
        if('\r' == *error_probe) {
            if('\n' == error_probe[1]) {
                error_begin = error_probe; error_line++; error_probe++;
            } else {
                error_begin = error_probe; error_line++;
            }
        } else if('\n' == *error_probe) {
            error_begin = error_probe; error_line++;
        }
        error_probe++;
    }
    printf("Error Line:%d; Row:%d;\n",
           error_line,
           (BufferProbe-error_begin)
    );
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

static JSON_Boolean __JSON_ParseValue(JSONParseBuffer* const buffer, thisJSON const item);

#include "JSON_Parse_Num.inl.h"
//static JSON_Boolean private_c(ParseNumber)(thisJSON const item, JSONParseBuffer* const buffer_input);

#include "JSON_Parse_Str.inl.h"
//static JSON_Boolean private_c(ParseString)(thisJSON const item, JSONParseBuffer* const buffer_input);

#include "JSON_Parse_Arr.inl.h"
//static JSON_Boolean private_c(ParseArray)(thisJSON const item, JSONParseBuffer* const buffer_input);

#include "JSON_Parse_Obj.inl.h"
//static JSON_Boolean private_c(ParseObject)(thisJSON const item, JSONParseBuffer* const buffer_input);

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * @ref parse_value
 */
static JSON_Boolean __JSON_ParseValue(JSONParseBuffer* const buffer, thisJSON const item) {
    if(JSONM_IsNull(buffer) || JSONM_IsNull(buffer->content)) {
        return false;
    }
    //null
    if( JSONM_BufferCkCS(buffer, "null", 4) ) {
        item->type = JSONType_NULL;
        JSONM_BufferForward(buffer, 4);
        return true;
    }
    //Boolean : true false
    if( JSONM_BufferCkCS(buffer, "false", 5) ) {
        item->type = JSONType_Boolean;
        item->VBoolean = false;
        JSONM_BufferForward(buffer, 5);
        return true;
    }
    if( JSONM_BufferCkCS(buffer, "true", 4) ) {
        item->type = JSONType_Boolean;
        item->VBoolean = true;
        JSONM_BufferForward(buffer, 4);
        return true;
    }
    //number
    if(
       JSONM_BufferCkCC(buffer, '-') ||
       ( *BufferProbe >= '0' && *BufferProbe <= '9' )
    ) {
        return __JSON_ParseNumber(item, buffer);
    }
    //string
    if( JSONM_BufferCkCC(buffer, '\"') ) {
        return __JSON_ParseString(item, buffer);
    }
    //array
    if( JSONM_BufferCkCC(buffer, '[') ) {
        return __JSON_ParseArray(item, buffer);
    }
    //object
    if( JSONM_BufferCkCC(buffer, '{') ) {
        return __JSON_ParseObject(item, buffer);
    }
    return false;
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * JSON_Parse
 * @ref cJSON_ParseWithOpts
 */
thisJSON JSON_Parse(const char* str_json, size_t str_size) {
    if(NULL == str_json) {
        goto fail;
    }

    //定义buffer
    JSONParseBuffer buffer;
    JSONM_BufferInitial(buffer, str_json, str_size);

    //定义item
    thisJSON item = JSONM_ItemNew;
    if (item == NULL) {
        goto fail;
    }
    JSONM_ItemInitial(item);

    __JSON_SkipUtf8Bom(&buffer);
    __JSON_SkipWhiteSpace(&buffer);
    //ParseValue
    if(!__JSON_ParseValue(&buffer, item)) {
        goto fail;
    }
    return item;

//==========================================
fail:
    if (NULL != str_json) {
        __JSON_ReportIndex(&buffer);
        if (NULL != item) {
            JSON_Remove(item);
        }
    }
    return NULL;
}

thisJSON JSON_ParseFile(const char* filename) {
    size_t filesize; char* content;
    FILE* fp = fopen(filename, "rb+");
        if( JSONM_IsNull(fp) ) {
            return NULL;
        }
        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        content  = global_hooks.allocate(filesize + 1);
        rewind(fp);
        fread(content, sizeof(char), filesize, fp);
        content[filesize] = '\0';
    fclose(fp);
    //------------------------------------------
    #ifdef __debug
        printf("filename=%s\n", filename);
        printf("filesize=%d\n", filesize);
        //printf("filecontent=%s\n", content);
    #endif // __debug
    thisJSON dump = JSON_Parse(content, filesize);
    global_hooks.deallocate(content);
    return dump;
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

#undef BufferContent
#undef BufferLength
#undef BufferProbe
#undef BufferDepth
