/*!
 * @ref parse_array
 */

static JSON_Boolean __JSON_ParseArray(thisJSON const item, JSONParseBuffer* const buffer) {
    JSONParse_Sign errorgi = JPOE_OnError;
    if('['!=*BufferProbe) {
        errorgi = JPOE_UnexpectedChar;
        goto fail;
    }

    BufferProbe++;
    BufferDepth++;

    thisJSON head = NULL;
    thisJSON current;
    thisJSON new_item;

    //Empty Array
    __JSON_SkipWhiteSpace(buffer);
    if( JSONM_BufferCanReadOne(buffer) ) {
        if(']'==*BufferProbe) {
            goto success;
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

        __JSON_SkipWhiteSpace(buffer);
        //printf("\tarr loop[%c]\n",*BufferProbe);
        if(!__JSON_ParseValue(buffer, current)) {
            errorgi = JPOE_ObjValue;
            goto fail;
        }

        //IsLoop
        __JSON_SkipWhiteSpace(buffer);
        if( JSONM_BufferCanReadOne(buffer) ) {
            if( ','==*BufferProbe ) {
                BufferProbe++;
                continue;
            } else if( ']'==*BufferProbe ) {
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
    printf("ParseArray Error = %d\n", errorgi);
    __JSON_ReportIndex(buffer);
    if( NULL!=head ) {
        JSON_Remove(head);
    }
    return false;

success:
    BufferDepth--;
    BufferProbe++;

    item->type = JSONType_Array;
    item->child = head;
    #ifdef __debug
        JSON_PrintTab(BufferDepth, stdout);
        printf("arr success[%c]:\n", *BufferProbe);
    #endif // __debug
    return true;
}






