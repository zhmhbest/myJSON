/*!
 * @ref parse_object
 */

static JSON_Boolean __JSON_ParseObject(thisJSON const item, JSONParseBuffer* const buffer) {
    JSONParse_Sign errorgi = JPOE_OnError;

    if('{'!=*BufferProbe) {
        errorgi = JPOE_UnexpectedChar;
        goto fail;
    }
    BufferProbe++;
    BufferDepth++;

    thisJSON head = NULL;
    thisJSON current;
    thisJSON new_item;

    //Empty Object
    __JSON_SkipWhiteSpace(buffer);
    if( JSONM_BufferCanReadOne(buffer) ) {
        if('}'==*BufferProbe) {
            goto success; //Empty
        }
    } else {
        errorgi = JPOE_InsufficientBuffer;
        goto fail;
    }

    for(;;) {
        new_item = JSONM_ItemNew;
        if( NULL==new_item ) {
            goto fail;
        }
        JSONM_ItemInitial(new_item);

        if( NULL==head ) {
            head = current = new_item;
        } else {
            JSONM_AddItemToNext(current, new_item);
            current = new_item;
        }


        //[parse Key]
        __JSON_SkipWhiteSpace(buffer);
        //printf("\tobj loop1[%c]\n",*BufferProbe);
        if(!__JSON_ParseValue(buffer, current)) {
            errorgi = JPOE_ObjKey;
            goto fail;
        }
        current->key = current->VBytes;
        current->VBytes = NULL;

        //Check :
        __JSON_SkipWhiteSpace(buffer);
        if( JSONM_BufferCanReadOne(buffer) ) {
            if( ':'!=*BufferProbe ) {
                errorgi = JPOE_UnexpectedChar;
                goto fail;
            }
            BufferProbe++;
        } else {
            errorgi = JPOE_InsufficientBuffer;
            goto fail;
        }

        //[parse Value]
        __JSON_SkipWhiteSpace(buffer);
        if(!__JSON_ParseValue(buffer, current)) {
            errorgi = JPOE_ObjValue;
            goto fail;
        }
        //JSON_PrintItem(current, stdout); system("pause");

        //IsLoop
        __JSON_SkipWhiteSpace(buffer);
        if( JSONM_BufferCanReadOne(buffer) ) {
            if( ','==*BufferProbe ) {
                BufferProbe++;
                continue;
            } else if( '}'==*BufferProbe ) {
                goto success;
                break;
            } else {
                errorgi = JPOE_UnexpectedChar;
                goto fail;
            }
        } else {
            errorgi = JPOE_InsufficientBuffer;
            goto fail;
        }
    }

fail:
    printf("ParseObject Error = %d\n", errorgi);
    __JSON_ReportIndex(buffer);
    if( NULL!=head ) {
        JSON_Remove(head);
    }
    return false;
success:
    BufferDepth--;
    BufferProbe++;
    item->type = JSONType_Object;
    item->child = head;
    #ifdef __debug
        JSON_PrintTab(BufferDepth, stdout);
        printf("obj success[%c]:\n", *BufferProbe);
    #endif // __debug
    return true;
}
