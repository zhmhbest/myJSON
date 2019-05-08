
/*!
 * 内存操作
 */
#define internal_malloc     malloc
#define internal_free       free
#define internal_realloc    realloc
typedef struct {
    void* __cdecl (*allocate)  (size_t size);
    void  __cdecl (*deallocate)(void *pointer);
    void* __cdecl (*reallocate)(void *pointer, size_t size);
} internal_hooks;
static internal_hooks
    global_hooks = { internal_malloc, internal_free, internal_realloc };
//#define IHOOKS_DEF const internal_hooks* const
//#define GHOOKS_GET     &global_hooks

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * JSONBytes
 */
/*
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
    size_t length; //Ubound - Lbound
} JSONBytes;
*/

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * Bytes Get
 */
#define JSONM_BytesNew  (JSONBytes*)global_hooks.allocate(sizeof(JSONBytes))
static inline JSONBytes* JSON_BytesGetRef(const void* add, size_t len) {
    JSONBytes* bytes = JSONM_BytesNew;
    if(NULL==bytes) {
        return NULL;
    }
    bytes->Lbound = add;
    bytes->Ubound = add + len;
    bytes->length = len;
    return bytes;
}
#define JSON_BytesGetRefStr(_str) JSON_BytesGetRef(_str, strlen(_str)+sizeof(""))

static inline JSONBytes* JSON_BytesGet(const void* add, size_t len) {
    void* copy = global_hooks.allocate(len);
    if(NULL==copy) {
        return NULL;
    }
    memcpy(copy, add, len); //add ==> copy
    //=========================================
    JSONBytes* bytes = JSONM_BytesNew;
    if(NULL==bytes) {
        global_hooks.deallocate(copy);
        return NULL;
    }
    bytes->Lbound = add;
    bytes->Ubound = add + len;
    bytes->length = len;
    return bytes;
}
#define JSON_BytesGetStr(_str) JSON_BytesGet(_str, strlen(_str)+sizeof(""))

/*!
 * Bytes Drop
 * @return JSONBytes.Lbound
 */
static inline void* JSON_BytesDrop(JSONBytes* bytes) {
    if(NULL==bytes) {
        return NULL;
    }
    void* copy = bytes->address;
    global_hooks.deallocate(bytes);
    return copy;
}
#define JSON_BytesDropDeep(_Jbytes) global_hooks.deallocate(JSON_BytesDrop(_Jbytes));

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*
 * Get Copy

static inline void* JSON_GetCopy(const void* add, size_t len) {
    void* copy = global_hooks.allocate(len);
    if(NULL==copy) {
        return NULL;
    }
    memcpy(copy, add, len); //add ==> copy
    return copy;
}
#define JSON_GetCopyStr(_str) JSON_GetCopy(_str, strlen(_str)+sizeof(""))
 */

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * JSONCell
 */
struct JSONCell {
    struct JSONCell *next, *prev;
    struct JSONCell *child;
    JSONType    type;
    JSONBytes*  key; //char* keyname;
    union {
        /*需要free*/
        JSONBytes*      VBytes;
        char**          VStringArr; //Pointer String
        /*不需要free*/
        int64_t         VInteger;
        double          VDouble;
        JSON_Boolean    VBoolean;
    };
};

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * 简化类型
 */
#define thisJSON            struct JSONCell*
#define thisJSONSize        sizeof(struct JSONCell)

/*!
 * 判断类型
 */
#define JSONM_IsNull(_ref)                  ( NULL == _ref )
#define JSONM_IsRefValue(_node)             ( (_node)->type & JSONType_IsRefValue )
#define JSONM_IsConstKey(_node)             ( (_node)->type & JSONType_IsConstKey )
#define JSONM_IsChildless(_node)            ( NULL == (_node)->child )
#define JSONM_IsNotChildless(_node)         ( NULL != (_node)->child )

#define JSONM_IsKeyValid(_node)             ( NULL != (_node)->key )
    //须首判断 JSONM_IsKeyValid
    #define JSONM_IsNotDeepKey(_node)       ( JSONM_IsConstKey(_node) || NULL == ((_node)->key)->string )

#define JSONM_IsNullString(_node)           ( (_node->type) == JSONType_String && ( NULL == ((_node)->VBytes) || NULL == ((_node)->VBytes)->string ) )
#define JSONM_IsBytesType(_node)            ( (_node->type & JSONType_Dividing) >= JSONType_Bytes && (_node->type & JSONType_Dividing) <= JSONType_Raw )
#define JSONM_IsBytesValid(_node)           ( JSONM_IsBytesType(_node) && NULL != (_node)->VBytes )
    //须首判断 JSONM_IsBytesValid
    #define JSONM_IsNotDeepBytes(_node)     ( JSONM_IsRefValue(_node) || NULL == ((_node)->VBytes)->string )
