#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {false, true} boolean;

typedef struct _cacheSet{
    boolean valid;
    boolean dirty;
    int tag;
} cacheSet;
typedef struct _cache0{
    boolean* valid;
    int* tag;
} cache0;
typedef struct _cache1{
    int* LRU;
    cacheSet** cacheSetMember;
} cache1;


unsigned int hex2Uint(char* hexStr);


int main(int argc, char *argv[])
{
    char cacheType = argv[1][0];
    char inputStr[9];

    cache0 simCache0;
    cache1 simCache1;

    FILE *fp = NULL;


    char fileName[100] = "trace";

    int missCount = 0;
    int memWriteCount = 0;


    simCache0.valid = malloc(sizeof(boolean) * 1024);
    simCache0.tag = malloc(sizeof(int) * 1024);

    for( int i=0; i<1024; i++){
        simCache0.valid[i] = false;
    }


    simCache1.LRU = malloc(sizeof(int) * 512);
    simCache1.cacheSetMember = malloc(sizeof(cacheSet *) * 512);

    for( int i=0; i<512; i++){
        simCache1.LRU[i] = 0;
        simCache1.cacheSetMember[i] = malloc(sizeof(cacheSet) * 2);
        for( int j=0; j<2; j++ ){
            simCache1.cacheSetMember[i][j].valid = false;
            simCache1.cacheSetMember[i][j].dirty = false;
        }
    }


    strcat(fileName, argv[2]);
    strcat(fileName, ".txt");


    fp = fopen(fileName, "r");

    while( !feof(fp) ){
        unsigned int memAddr;
        char LS;
        unsigned int cacheIdx;
        unsigned int cacheTag;

        fscanf(fp, "%c %s\n",  &LS, inputStr);

        memAddr = hex2Uint(inputStr);
            

        if( cacheType == '0' ){
            cacheIdx = (memAddr >> 4) % 1024;
            cacheTag = memAddr >> 14;

            if( LS == 'L' ){
                if( !simCache0.valid[cacheIdx] || (simCache0.tag[cacheIdx] != cacheTag) ){ //cache miss
                    missCount++;
                    if( !simCache0.valid[cacheIdx] ){ //cold miss
                        simCache0.valid[cacheIdx] = true;
                        simCache0.tag[cacheIdx] = cacheTag;
                    }
                    else{ //cache conflict
                        simCache0.tag[cacheIdx] = cacheTag;
                    }
                }
            }
            else if( LS == 'S'){
                memWriteCount++;
                if( !simCache0.valid[cacheIdx] || (simCache0.tag[cacheIdx] != cacheTag) ){ //cache miss
                    missCount++;
                }
            }
        }
        else if( cacheType == '1' ){
            boolean isHit = false;
            cacheIdx = (memAddr >> 6) % 512;
            cacheTag = memAddr >> 15;

            if( LS == 'L' ){
                for( int j=0; j<2; j++ ){
                    if( simCache1.cacheSetMember[cacheIdx][j].valid && simCache1.cacheSetMember[cacheIdx][j].tag == cacheTag ){
                        isHit = true;
                        if( j == 0 )
                            simCache1.LRU[cacheIdx] = 1;
                        else                        
                            simCache1.LRU[cacheIdx] = 0;
                        break;
                    }
                }
                if( !isHit ){
                    missCount++;
                    simCache1.cacheSetMember[cacheIdx][simCache1.LRU[cacheIdx]].valid = true;
                    if( simCache1.cacheSetMember[cacheIdx][simCache1.LRU[cacheIdx]].dirty ){
                        memWriteCount++;
                        simCache1.cacheSetMember[cacheIdx][simCache1.LRU[cacheIdx]].dirty = false;
                    }
                    simCache1.cacheSetMember[cacheIdx][simCache1.LRU[cacheIdx]].tag = cacheTag;
                    if( simCache1.LRU[cacheIdx] == 0 )
                        simCache1.LRU[cacheIdx] = 1;
                    else                        
                        simCache1.LRU[cacheIdx] = 0;
                }
            }
            else if( LS == 'S' ){
                for( int j=0; j<2; j++ ){
                    if( simCache1.cacheSetMember[cacheIdx][j].valid && simCache1.cacheSetMember[cacheIdx][j].tag == cacheTag ){
                        isHit = true;
                        simCache1.cacheSetMember[cacheIdx][j].dirty = true;
                        if( j == 0 )
                            simCache1.LRU[cacheIdx] = 1;
                        else                        
                            simCache1.LRU[cacheIdx] = 0;
                        break;
                    }
                }
                if( !isHit ){
                    missCount++;
                    simCache1.cacheSetMember[cacheIdx][simCache1.LRU[cacheIdx]].valid = true;
                    if( simCache1.cacheSetMember[cacheIdx][simCache1.LRU[cacheIdx]].dirty ){
                        memWriteCount++;
                    }                    
                    simCache1.cacheSetMember[cacheIdx][simCache1.LRU[cacheIdx]].dirty = true;
                    simCache1.cacheSetMember[cacheIdx][simCache1.LRU[cacheIdx]].tag = cacheTag;
                    if( simCache1.LRU[cacheIdx] == 0 )
                        simCache1.LRU[cacheIdx] = 1;
                    else                        
                        simCache1.LRU[cacheIdx] = 0;
                }
            }
        }


    }

    fclose(fp);

    free( simCache0.valid );
    free( simCache0. tag );
    free( simCache1.LRU );
    for( int i=0; i<512; i++ ) {
        free( simCache1.cacheSetMember[i] );
    }


    printf("%d %d", missCount, memWriteCount);

    return 0;
}


unsigned int hex2Uint(char* hexStr){
    unsigned int i=0;
    unsigned int hexpow = 1;

    for( int j=0; j<8; j++ ){
        switch( hexStr[7-j] ){
            case '0':
                break;
            case '1':
                i += hexpow;
                break;
            case '2':
                i += hexpow * 2U;       
                break;         
            case '3':
                i += hexpow * 3U;
                break;
            case '4':
                i += hexpow * 4U;
                break;
            case '5':
                i += hexpow * 5U;
                break;
            case '6':
                i += hexpow * 6U;
                break;
            case '7':
                i += hexpow * 7U;
                break;
            case '8':
                i += hexpow * 8U;
                break;
            case '9':
                i += hexpow * 9U;
                break;
            case 'a':
                i += hexpow * 10U;
                break;
            case 'b':
                i += hexpow * 11U;
                break;
            case 'c':
                i += hexpow * 12U;
                break;
            case 'd':
                i += hexpow * 13U;
                break;
            case 'e':
                i += hexpow * 14U;
                break;
            case 'f':
                i += hexpow * 15U;
                break;
        }

        hexpow *= 16U;
    }

    return i;
}