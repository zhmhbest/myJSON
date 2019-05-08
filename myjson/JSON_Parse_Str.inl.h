
/*!
 * StrGetRange
 * @note 返回的length为不含null字符串长度
 */
static inline JSONParse_OnError __JSON_StrGetRange(JSONParseBuffer* const buffer, JSONBytes* const str_range) {
    str_range->probe = ('\"'==*BufferProbe) ? ++BufferProbe : BufferProbe;
    while( JSONM_BufferCanReadOne(buffer) ) {

        //转义情况
        if('\\' == *BufferProbe) {
            if( !JSONM_BufferCanRead(buffer, 1) ) {  /*Probe[1] 必须可读*/
                return JPOE_StrEndOfBackslash;
            }
            switch(BufferProbe[1]) {
            case 'x':                               /*Probe[2] Probe[3] 必须可读*/
                if( !JSONM_BufferCanRead(buffer, 3) ) {
                    return JPOE_StrEndOfX;
                }
                if( !JSONM_NumIsNumber(BufferProbe[2]) || !JSONM_NumIsNumber(BufferProbe[3]) ) {
                    return JPOE_StrErrorX;
                }
                BufferProbe+=4;     /*@Probe[4]*/
                break;
            case 'u':                               /*Probe[2] Probe[3] Probe[4] Probe[5] 必须可读*/
                if( !JSONM_BufferCanRead(buffer, 5)) {
                    return JPOE_StrEndOfU;
                }
                if(
                   !JSONM_NumIsNumber(BufferProbe[2]) || !JSONM_NumIsNumber(BufferProbe[3]) ||
                   !JSONM_NumIsNumber(BufferProbe[4]) || !JSONM_NumIsNumber(BufferProbe[5])
                ) {
                    return JPOE_StrErrorU;
                }
                BufferProbe+=6;     /*@Probe[6]*/
                break;
            default:
                BufferProbe+=2;     /*@Probe[2]*/
            }
            continue;
        }

        if('\"' == *BufferProbe) {
            //Success
            str_range->otail  = BufferProbe++;
            str_range->length = str_range->otail - str_range->probe;
            #ifdef __debug
                JSON_PrintTab(BufferDepth, stdout);
                printf("str range: success = [%d], [%c], [%c], [%c].\n", str_range->length, *str_range->probe, str_range->otail[-1], *BufferProbe);
            #endif // __debug
            return JPSI_EndOfString;
        } else {
            BufferProbe++;
        }
    }

    return JPOE_NoTail;                /*字符串没有结尾*/
}


/*!
 * StrEscape
 * @note 返回的length为含null字符串长度
 */
static inline JSONParse_OnError __JSON_StrEscape(JSONBytes* str_range, JSONBytes* str_copy) {
    unsigned char* copy = global_hooks.allocate(str_range->length + sizeof(""));
    if(NULL==copy) {
        return JPOE_AllocateError;   /*内存分配失败*/
    }
    str_copy->address = copy;

    while( str_range->probe < str_range->otail ) {

        //非转义字符
        if (*str_range->probe != '\\') {
            *copy++ = *(str_range->probe)++;
            continue;
        }

        //转义时至少有 str_range.probe[1]待转义 和 '\"'==str_range.probe[2]
        switch (str_range->probe[1]) {
            case 'b': *copy++ = '\b'; break;
            case 'f': *copy++ = '\f'; break;
            case 'n': *copy++ = '\n'; break;
            case 'r': *copy++ = '\r'; break;
            case 't': *copy++ = '\t'; break;
            case '\\': case '\'': case '\"':
                *copy++ = str_range->probe[1]; break;
            case 'x':
                *copy++ = JSONM_NumParseChar(str_range->probe[2]) * 0x10 + JSONM_NumParseChar(str_range->probe[3]);
                (str_range->probe)+=4;
                continue;
            case 'u':
                /* UTF-16 literal */
                *copy++ = JSONM_NumParseChar(str_range->probe[2]) * 0x10 + JSONM_NumParseChar(str_range->probe[3]);
                *copy++ = JSONM_NumParseChar(str_range->probe[4]) * 0x10 + JSONM_NumParseChar(str_range->probe[5]);
                (str_range->probe)+=6;
                continue;
            default:
                return JPOE_UnexpectedChar;
        }
        (str_range->probe)+=2; //break:+2; continue: skip this.

    } *copy++ = '\0';

    str_copy->length = copy - str_copy->probe;
    str_copy->otail = str_copy->probe + str_copy->length;
    copy = global_hooks.reallocate(str_copy->address, str_copy->length);
    if(NULL==copy) {
        return JPOE_AllocateError;   /*内存分配失败*/
    }
    str_copy->address = copy;

    return JPSI_EndOfString;
}

/*!
 * @ref parse_string
 */
static JSON_Boolean __JSON_ParseString(thisJSON const item, JSONParseBuffer* const buffer) {
    JSONParse_Sign errorgi;
    if(JSONM_IsNull(buffer) || JSONM_IsNull(BufferContent) || '\"'!=*BufferProbe++ ) {
        errorgi = JPOE_NullBuffer;
        goto fail;
    }

    //获得非转义前长度
    JSONBytes str_range;

    errorgi = __JSON_StrGetRange(buffer, &str_range);
    if( errorgi >= JPOE_OnError ) {
        goto fail;
    }

    //新建Bytes
    JSONBytes* str_copy = JSONM_BytesNew;
    if(NULL==str_copy) {
        errorgi = JPOE_AllocateError;
        goto fail;
    }

    //字符串转义
    errorgi = __JSON_StrEscape(&str_range, str_copy);
    if( errorgi >= JPOE_OnError ) {
        global_hooks.deallocate(str_copy);
        goto fail;
    }
    goto success;

fail:
    printf("ParseString Error = %d\n", errorgi);
    return false;
success:
    item->type = JSONType_String;
    item->VBytes = str_copy;
    #ifdef __debug
        JSON_PrintTab(BufferDepth, stdout);
        printf("str : success(%d) = #[ %s ]#\n", item->VBytes->length-1, *item->VStringArr);
        /*{
            unsigned char* t_probe = str_copy->probe;
            for(;t_probe < str_copy->otail; t_probe++) {
                printf("%c[%x]\n", *t_probe, *t_probe);
            }
        }*/
        JSON_PrintTab(BufferDepth, stdout);
        printf("---------------------\n");
    #endif // __debug
    return true;
}
