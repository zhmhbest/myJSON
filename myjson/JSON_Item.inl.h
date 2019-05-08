
#define JSONM_AddItemToNext(_this, _item) _this->next = _item; _item->prev = _this;

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * AddItemToArray
 * @param array
 * @param item
 */
JSON_Boolean JSON_AddItemToArray(thisJSON array, thisJSON item) {

    if(JSONM_IsNull(item) || JSONM_IsNull(array)) {
        return false;
    }

    if(JSONM_IsNull(array->child)) {
        // list is empty, start new one
        (array->child) = item;
    } else {
        // append to the end
        thisJSON child = (array->child);
        while (child->next) {
                child = child->next;
        }
        JSONM_AddItemToNext(child, item);
    }

    return true;
}
#define JSONM_AddItemToChild JSON_AddItemToArray

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * AddItemToObject
 * @param object
 * @param keyname
 * @param item
 * @param IsConstantKey
 */
JSON_Boolean JSON_AddItemToObject(thisJSON const object, const char* const keyname, thisJSON const item, const JSON_Boolean IsConstantKey) {
    JSONBytes* new_key = NULL;
    int new_type = JSONType_Invalid;

    if(JSONM_IsNull(object) || JSONM_IsNull(keyname) || JSONM_IsNull(item)) {
        return false;
    }

    if(IsConstantKey) {
        //添加引用而不是拷贝
        new_key  = JSON_BytesGetRefStr(keyname);
        new_type = item->type | JSONType_IsConstKey;
    } else {
        //拷贝一份
        new_key = JSON_BytesGetStr(keyname);
        if(JSONM_IsNull(new_key)) {
            return false;
        }
        new_type = item->type & ~JSONType_IsConstKey;
    }

    //删除原来的item>key
    if( JSONM_IsKeyValid(item) ) {
        if( JSONM_IsNotDeepKey(item) ) {
            JSON_BytesDrop(item->key);
        } else {
            JSON_BytesDropDeep(item->key);
        }
    }

    item->key   = new_key;
    item->type  = new_type;

    return JSONM_AddItemToChild(object, item);
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * 删除条目
 */
void JSON_Remove(thisJSON item) {
    if(JSONM_IsNull(item)) return;
    thisJSON next;
    while (item != NULL) {
        if (!JSONM_IsRefValue(item) && !JSONM_IsChildless(item)) {
            /*非引用 且 有子*/
            JSON_Remove(item->child);
        }
        next = item->next;
        //=====================================================
        #ifdef __debug
            JSON_PrintItem(item,stdout);
        #endif // __debug

        if( JSONM_IsBytesValid(item) ) {    /*Bytes Type*/
            if( JSONM_IsNotDeepBytes(item) ) {
                /*引用 或 空串*/
                JSON_BytesDrop(item->VBytes);
                #ifdef __debug
                    printf(", bytes");
                #endif // __debug
            } else {
                /*副本 且 非空*/
                JSON_BytesDropDeep(item->VBytes);
                #ifdef __debug
                    printf(", bytes & value");
                #endif // __debug
            }
        }

        if( JSONM_IsKeyValid(item) ) {
            if( JSONM_IsNotDeepKey(item) ) {
                JSON_BytesDrop(item->key);
                #ifdef __debug
                    printf(", key");
                #endif // __debug
            } else {
                JSON_BytesDropDeep(item->key);
                #ifdef __debug
                    printf(", key & value");
                #endif // __debug
            }
        }

        global_hooks.deallocate(item);
        #ifdef __debug
            printf(", item\n");
        #endif // __debug
        //=====================================================
        item = next;
    }
}
