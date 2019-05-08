
static JSON_Boolean __JSON_PrintValue(const thisJSON item, int* depth, JSON_Boolean format, FILE* output);

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

/*!
 * 打印String
 * @ref  print_string_ptr
 */
static JSON_Boolean __JSON_PrintString(const unsigned char* const input, FILE* output) {
    //const unsigned char* const input = item->VBytes;
    const unsigned char* input_pointer;
    //空字符串
    if (input == NULL) {
        fputs("\"\"", output);
        return true;
    }
    //检查转义字符
    fputc('\"', output);
    for (input_pointer = input; *input_pointer != '\0'; input_pointer++) {
        if ((*input_pointer > 31) && (*input_pointer != '\"') && (*input_pointer != '\\')) {
            //非转义字符
            fputc(*input_pointer, output);
        } else {
            //转义字符
            fputc('\\', output);
            switch (*input_pointer) {
                case '\\': fputc('\\', output); break;
                case '\"': fputc('\"', output); break;
                case '\b': fputc('b' , output); break;
                case '\f': fputc('f' , output); break;
                case '\n': fputc('n' , output); break;
                case '\r': fputc('r' , output); break;
                case '\t': fputc('t' , output); break;
                default:
                    //fprintf(output, "u%04x", *input_pointer);
                    break;
            }
        }
    }
    fputc('\"', output);
    return true;
}

static JSON_Boolean __JSON_PrintStringValue(const thisJSON item, FILE* output) {
    return __JSON_PrintString((const unsigned char*)(item->VBytes->string),output);
}

static JSON_Boolean __JSON_PrintKeyString(const thisJSON item, FILE* output) {
    return __JSON_PrintString((const unsigned char*)(item->key->string),output);
}

/*□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□*/

/*!
 * 打印Array
 * @ref  print_array
 */
static JSON_Boolean __JSON_PrintArray(const thisJSON item, int* depth, JSON_Boolean format, FILE* output) {
    //JSON_Boolean format = true;
    thisJSON current_element = item->child;

    fputc('[', output); (*depth)++;

    while (current_element != NULL) {
        if (!__JSON_PrintValue(current_element, depth, format, output)) {
            return false;
        }

        if (current_element->next) {
            if(format) {
                fputs(", ", output);
            } else {
                fputs(",", output);
            }
        }
        current_element = current_element->next;
    }

    fputc(']', output); (*depth)--;
    return true;
}

/*□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□*/

/*!
 * 打印Object
 * @ref  print_object
 */
static JSON_Boolean __JSON_PrintObject(const thisJSON item, int* depth, JSON_Boolean format, FILE* output) {
    size_t i;
    //int depth = 0;
    //JSON_Boolean format = true;
    thisJSON current_item = item->child;

    //output_pointer
    fputc('{', output); (*depth)++;
    if(format) { fputc('\n', output); }

    /*==========================================================
     *==========================================================*/
    while (current_item) {
        //print TAB
        if (format) {
            for (i = 0; i < *depth; i++) {
                fputs("    ", output);
            }
        }
        //print key
        if (!__JSON_PrintKeyString(current_item, output)) {
            return false;
        }
        //print :
        if(format) {
            fputs(" : ", output);
        } else {
            fputs(":", output);
        }
        //print value
        if(!__JSON_PrintValue(current_item, depth, format, output)) {
            return false;
        }
        //print ,
        if(current_item->next) {
            fputc(',', output);
        }

        if(format) { fputc('\n', output); }
        //next
        current_item = current_item->next;
    }

    /*==========================================================
     *==========================================================*/

    if (format) {
        for (i = 1; i < *depth; i++) {
            fputs("    ", output);
        }
    }

    fputc('}', output); (*depth)--;

    return true;
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

static JSON_Boolean __JSON_PrintValue(const thisJSON item, int* depth, JSON_Boolean format, FILE* output) {
    if ((item == NULL) || (output == NULL)) {
        return false;
    }

    switch ((item->type) & JSONType_Dividing) {

        case JSONType_NULL:
            fputs("null", output);
            return true;

        case JSONType_Boolean:
            if(item->VBoolean) {
                fputs("true", output);
            } else {
                fputs("false", output);
            }
            return true;

        case JSONType_Integer: {
                int64_t v = item->VInteger;
                if ((v * 0) != 0) {
                    fputs("null", output);
                } else {
                    #ifdef __WINDOWS__
                        fprintf(output, "%I64d", v); //Windows
                    #else
                        fprintf(output, "%lld", v);  //Linux
                    #endif // __WINDOWS__
                }
            }
            return true;

        case JSONType_Float: {
                double v = item->VDouble;
                if ((v * 0) != 0) {
                    fputs("null", output);
                } else {
                    fprintf(output, "%f", v);
                }
            }
            return true;

        case JSONType_String:
            return __JSON_PrintStringValue(item, output);

        case JSONType_Array:
            return __JSON_PrintArray(item, depth, format,output);

        case JSONType_Object:
            return __JSON_PrintObject(item, depth, format, output);

        case JSONType_Raw:
            if ( JSONM_IsNullString(item) ) {
                return false;
            } else {
                fputs(item->VBytes->string, output);
                return true;
            }

        default:
            return false;
    }
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

#ifdef __debug
static void JSON_PrintItem(const thisJSON item, FILE* output) {
    if(JSONM_IsKeyValid(item)) {
        fprintf(output, "[%s] = ", item->key->string);
    } else {
        fprintf(output, "[-] = ");
    }
    switch(item->type & JSONType_Dividing) {
        case JSONType_Array:    fprintf(output, "(array)");   break;
        case JSONType_Object:   fprintf(output, "(object)");  break;
        case JSONType_String:   fprintf(output, "%s", item->VBytes->string); break;
        case JSONType_Raw:      fprintf(output, "%s", item->VBytes->string); break;
        case JSONType_Integer:  fprintf(output, "%I64d", item->VInteger); break;
        case JSONType_Float:    fprintf(output, "%f", item->VDouble); break;
        case JSONType_Boolean:  fprintf(output, "%s", item->VBoolean?"true":"false"); break;
        case JSONType_NULL:     fprintf(output, "(null)");    break;
        case JSONType_Invalid:  fprintf(output, "(Invalid)"); break;
        default:                fprintf(output, "(else)");    break;
    }
}
static void JSON_PrintTab(int depth, FILE* output) {
    int i = 0;
    for(;i<depth;i++)
        fprintf(output,"\t");
}
#endif // __debug

JSON_Boolean JSON_Print(const thisJSON item, JSON_Boolean format, FILE* output) {
    #ifdef __debug
        printf("JSON_Print\n");
    #endif // __debug
    int depth = 0;
    return __JSON_PrintValue(item, &depth, format, output);
}

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

static void __JSON_EnumData(const thisJSON item, int* depth, JSON_EnumCallback callback) {

    callback((JSONElement*)item, *depth); //printf("[%d]%x\n", *depth,item->type);

    int type = (item->type) & JSONType_Dividing;
    if( JSONType_Object == type || JSONType_Array == type) {
        (*depth)++;
        thisJSON childItem = item->child;
        while(childItem) {
            __JSON_EnumData(childItem, depth, callback);
            childItem = childItem->next;
        }
        (*depth)--;
    }
}

/*
//JME.EnumData(root, calltest);
void calltest (const JSONElement* item, int depth) {
    printf("[%d]", depth);
    if(item->Key != NULL) {
        printf("[%s]", item->Key->string);
    }
    if(item->Type==JSONType_String) {
        printf("=%s\n", item->VBytes->string);
    } else if(item->Type==JSONType_Integer) {
        printf("=%I64d\n", item->VInteger);
    } else if(item->Type==JSONType_Float) {
        printf("=%f\n", item->VDouble);
    } else if(item->Type==JSONType_Boolean) {
        printf("=%s\n", item->VBoolean?"true":"false");
    } else {
        printf("=Type(%d)\n", item->Type);
    }
}
*/
void JSON_EnumData(const myJSON* root, JSON_EnumCallback callback) {
    int depth = 0;
    __JSON_EnumData(root, &depth, callback);

}
