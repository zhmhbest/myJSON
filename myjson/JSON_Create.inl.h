/*
    const JSONMethods JME = *JSON_GetMethods();
    myJSON* root = NULL;
    myJSON* arr = NULL;
    myJSON* fmt = NULL;
    root = JME.CreateObject();
    JME.AddItemToObject(root, "name", JME.CreateString("Jack (\"Bee\") Nimble"), false);
    JME.AddItemToObject(root, "format", fmt = JME.CreateObject(), false);

    JME.AddItemToObject(fmt, "type", JME.CreateString("rect"), false);
    JME.AddItemToObject(fmt, "width", JME.CreateInteger(1920), false);

    JME.AddItemToObject(fmt, "height", JME.CreateInteger(1080), false);

    JME.AddItemToObject(fmt, "arr", arr=JME.CreateArray(), false);
    JME.AddItemToArray(arr, JME.CreateInteger(1920));
    JME.AddItemToArray(arr, JME.CreateFloat(3.14));
    JME.AddItemToArray(arr, JME.CreateString("aaaaaaasdd"));
    JME.AddItemToObject(fmt, "interlace", JME.CreateBoolean(1), false);
    JME.Print(root, true, stdout);
*/

/*!
 * 新建条目
 */
#define JSONM_ItemNew               (thisJSON)global_hooks.allocate(thisJSONSize)
#define JSONM_ItemInitial(_node)    memset(_node, '\0', thisJSONSize)
/*
thisJSON item = JSONM_ItemNew;
if(item) {
    JSONM_ItemInitial(item);
    item->type = ;

}
*/

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

thisJSON JSON_CreateNull(void) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_NULL;
    }
    return item;
}

thisJSON JSON_CreateBoolean(JSON_Boolean b) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_Boolean;
        item->VBoolean = b ? true : false;
    }
    return item;
}

thisJSON JSON_CreateInteger(int64_t num) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_Integer;
        item->VInteger = num;
    }
    return item;
}

thisJSON JSON_CreateFloat(double num) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_Float;
        item->VInteger = num;
    }
    return item;
}

/*□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□*/

thisJSON JSON_CreateArray(void) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_Array;
    }
    return item;
}

thisJSON JSON_CreateObject(void) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_Object;
    }
    return item;
}

thisJSON JSON_CreateString(const char* str) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_String;
        item->VBytes = JSON_BytesGetStr(str);
        if(!item->VBytes) {
            JSON_Remove(item);
            return NULL;
        }
    }
    return item;
}

thisJSON JSON_CreateRaw(const char* str) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_Raw;
        item->VBytes = JSON_BytesGetStr(str);
        if(!item->VBytes) {
            JSON_Remove(item);
            return NULL;
        }
    }
    return item;
}

/*□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□*/

thisJSON JSON_CreateArrayReference(const thisJSON child) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_Array | JSONType_IsRefValue;
        item->child = (thisJSON)cast_away_const(child);
    }
    return item;
}

thisJSON JSON_CreateObjectReference(const thisJSON child) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_Object | JSONType_IsRefValue;
        item->child = (thisJSON)cast_away_const(child);
    }
    return item;
}

thisJSON JSON_CreateStringReference(const char* str) {
    thisJSON item = JSONM_ItemNew;
    if(item) {
        JSONM_ItemInitial(item);
        item->type = JSONType_String | JSONType_IsRefValue;
        item->VBytes = JSON_BytesGetRefStr(cast_away_const(str));
    }
    return item;
}
