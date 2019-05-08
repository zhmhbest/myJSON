
static const JSONMethods __JSON_methods = {
    JSON_Version,
    //==========================================================
    JSON_EnumData,
    JSON_Print,
    JSON_Parse,
    JSON_ParseFile,
    //==========================================================
    JSON_CreateNull,
    JSON_CreateBoolean,
    JSON_CreateInteger,
    JSON_CreateFloat,

    JSON_CreateArray,
    JSON_CreateObject,
    JSON_CreateString,
    JSON_CreateRaw,

    JSON_CreateArrayReference,
    JSON_CreateObjectReference,
    JSON_CreateStringReference,

    JSON_Remove,
    //==========================================================
    JSON_AddItemToArray,
    JSON_AddItemToObject
    //==========================================================
};

const JSONMethods* const
JSON_GetMethods() { return &(__JSON_methods); }

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

#if defined(__clang__) || (defined(__GNUC__)  && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
    #pragma GCC diagnostic push
#endif
#ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wcast-qual"
#endif
/* helper function to cast away const */
static void* cast_away_const(const void* ptr) {
    return (void*)ptr;
}
#if defined(__clang__) || (defined(__GNUC__)  && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
    #pragma GCC diagnostic pop
#endif

