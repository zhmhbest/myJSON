#include <stdio.h>
#include <stdlib.h>
#include "myJSON.h"

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

int main(void) {
    const JSONMethods JME = *JSON_GetMethods();
    myJSON* root = JME.ParseFile("./json/test_input.json");
    JME.EnumData(root, calltest);
    FILE* foutput = fopen("./json/test_output.json", "wb+");
    JME.Print(root, true, foutput);
    fclose(foutput);
    JME.Remove(root);
    printf("END\n");
    return 0;
}
