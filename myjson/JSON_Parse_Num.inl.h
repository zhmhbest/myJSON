
/*!CanRead(1) 判断正负
 */
static inline JSONParse_OnError __JSON_NumCheckNegative(JSONParseBuffer* const buffer) {
    if( JSONM_BufferCanReadOne(buffer) ) {
        switch(*BufferProbe) {
        case '-':
            BufferProbe++; return JPSI_NegativeNumber;
        case '+':
            BufferProbe++; return JPSI_PositiveNumber;
        default:
            return JPSI_PositiveNumber;
        }
    } else {
        return JPOE_InsufficientBuffer;    /*空间不够*/
    }
}

/*!CanRead(1+1) 判断进制
 */
static inline JSONParse_OnError __JSON_NumCheckWeight(JSONParseBuffer* const buffer) {

    if( !JSONM_BufferCanReadOne(buffer) ) {
        return JPOE_InsufficientBuffer;      /*空间不够*/
    }

    unsigned char value = JSONM_NumLowerCase(BufferProbe[0]);
    if( JSONM_NumBelongTo(value, '1', '9') ) {
        return JPSI_WeightDec;               /*Weight:10*/
    }

    /*Head:0*/
    if( '0' == value ) {
        if( !JSONM_BufferCanRead(buffer, 1) ) {
            return JPOE_InsufficientBuffer;  /*空间不够*/
        }

        value = JSONM_NumLowerCase(BufferProbe[1]);
        if('b' == value) {
            BufferProbe+=2;
            return JPSI_WeightBin;          /*Weight:2*/
        } else if('x' == value) {
            BufferProbe+=2;
            return JPSI_WeightHex;          /*Weight:16*/
        } else if(JSONM_NumBelongTo(value, '0', '7')) {
            BufferProbe+=1;
            return JPSI_WeightOct;          /*Weight:8*/
        } else {
            //'.' ',' ';'
            return JPSI_WeightDec;          /*Weight:10*/
        }
    }

    return  JPOE_UnexpectedChar;            /*一位出错*/
}

/*!CanRead(1+1) 获取整数部分
 */
static inline JSONParse_OnError __JSON_NumGetInteger(JSONParseBuffer* const buffer, int weight, int64_t* number_integer) {
    unsigned char value;

    for(*number_integer = 0; JSONM_BufferCanReadOne(buffer); BufferProbe++) {
        value = JSONM_NumLowerCase(*BufferProbe);

        if( JSONM_NumBelongTo(value, '0', '9') ) {
            value -= '0';
            if(value>=weight) goto super_weight;
            (*number_integer) = (*number_integer)*weight + value;
        } else if( JSONM_NumBelongTo(value, 'a', 'f') ) {
            value += (10 - 'a');
            if(value>=weight) goto super_weight;
            (*number_integer) = (*number_integer)*weight + value;
        } else {
            if('.'==value) {
                //BufferProbe++;
                return JPSI_NextDecimal;    /*抵达小数区域*/
            } else {
                return JPSI_EndOfNumber;    /*抵达非整数区域*/
            }
        }
    }
    return JPOE_InsufficientBuffer;         /*空间不够*/

super_weight:
    //'e' + (10 - 'a') = 14;
    if( 10==weight && 14==value ) {
        //BufferProbe++;
        return JPSI_NextPower;              /*抵达指数区域*/
    } else {
        return JPOE_UnexpectedChar;         /*进制超越*/
    }
}

/*!
 * @ref parse_number
 */
static JSON_Boolean __JSON_ParseNumber(thisJSON const item, JSONParseBuffer* const buffer) {
    JSONParse_Sign errorgi;
    int64_t numd;
    int64_t numf = 0; int lenf = 0;
    int64_t nump = 0; int negp = 0;

    if(JSONM_IsNull(buffer) || JSONM_IsNull(BufferContent)) {
        errorgi = JPOE_NullBuffer;
        goto fail;
    }

    //处理正负 判断进制情况
    int flagNegative = __JSON_NumCheckNegative(buffer);
    int flagHeadType = __JSON_NumCheckWeight(buffer);
    #ifdef __debug
        JSON_PrintTab(BufferDepth, stdout);
        printf("num Negative[%c]:%d\n", *BufferProbe, JPSI_NegativeNumber==flagNegative);
        JSON_PrintTab(BufferDepth, stdout);
        printf("num HeadType[%c]:%d\n", *BufferProbe, flagHeadType);
    #endif // __debug
    if(flagHeadType >= JPOE_OnError) {
        errorgi = flagHeadType;
        goto fail;
    }

    //获取整数部分
    //int64_t numd = 0;
    errorgi = __JSON_NumGetInteger(buffer, flagHeadType, &numd);
    #ifdef __debug
        JSON_PrintTab(BufferDepth, stdout);
        printf("num Integer[%c](%d):%I64d\n", *BufferProbe, errorgi, numd);
    #endif // __debug
    if(errorgi >= JPOE_OnError) {
        goto fail;
    }


    //获取小数部分
    //int64_t numf = 0; int lenf = 0;
    if(JPSI_NextDecimal == errorgi) {
        BufferProbe++;
        const unsigned char* mark = BufferProbe;
        errorgi = __JSON_NumGetInteger(buffer, flagHeadType, &numf);
        lenf = BufferProbe - mark;
        #ifdef __debug
            JSON_PrintTab(BufferDepth, stdout);
            printf("num Decimal[%c](%d):%I64d, len=%d\n", *BufferProbe, errorgi, numf, lenf);
        #endif // __debug
        if(errorgi >= JPOE_OnError) {
            goto fail;
        }
    }

    //获取指数部分
    //int64_t nump = 0; int negp = 0;
    if(JPSI_NextPower == errorgi) {
        BufferProbe++;
        negp = __JSON_NumCheckNegative(buffer);
        if(negp >= JPOE_OnError) {
            errorgi = negp;
            goto fail;
        }
        errorgi = __JSON_NumGetInteger(buffer, flagHeadType, &nump);
        #ifdef __debug
            JSON_PrintTab(BufferDepth, stdout);
            printf("num Power  [%c](%d):%I64d, neg=%d\n", *BufferProbe, errorgi, nump, JPSI_NegativeNumber==negp);
        #endif // __debug
        if(errorgi >= JPOE_OnError) {
            goto fail;
        }
    }

    //合成
    if(JPSI_EndOfNumber == errorgi) {
        goto success;
    }

    /*
        "arr":[3e5, 6e-5, 0, 3.141592, 2.66978e3, 077, 3.14e-5, 0b1111, 0xff, 2, 3, 1]
    */

fail:
    printf("ParseNumber Error = %d\n", errorgi);
    return false;
success:
    #define CaculateValue       ( numd + (numf*pow(0.1,lenf)) )
    #define CaculateWithPow     ( JPSI_NegativeNumber == negp )         ? ( CaculateValue*pow(0.1,nump) ) : ( CaculateValue*pow(10,nump) )
    #define CaculateWithPowNeg  ( JPSI_NegativeNumber == flagNegative ) ? -CaculateWithPow : CaculateWithPow

    if ( lenf>0  && ( 0==negp || JPSI_NegativeNumber==negp || nump<lenf ) ) {
        item->type = JSONType_Float;
        item->VDouble = CaculateWithPowNeg;
    } else if( 0==lenf && JPSI_NegativeNumber==negp ) {
        item->type = JSONType_Float;
        item->VDouble = CaculateWithPowNeg;
    } else {
        item->type = JSONType_Integer;
        item->VInteger = CaculateWithPowNeg;
    }

    #ifdef __debug
        JSON_PrintTab(BufferDepth, stdout);
        if(JSONType_Integer==item->type) {
            printf("num success[%c]Integer:%I64d\n", *BufferProbe, item->VInteger);
        } else {
            printf("num success[%c]Double:%f\n", *BufferProbe, item->VDouble);
        }
        JSON_PrintTab(BufferDepth, stdout);
        printf("---------------------\n");
    #endif // __debug
    return true;

    #undef CaculateWithALL
    #undef CaculateWithPow
    #undef CaculateValue
}
