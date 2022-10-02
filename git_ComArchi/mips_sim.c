#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUTSTR_LEN 110
#define MAX_INSTRUCTIONMEMORY_SIZE 0x10001
#define MAX_DATAMEMORY_SIZE 0x10001


typedef unsigned int UINT32;
typedef int INT32;

typedef enum {false, true} boolean;

typedef struct _pseudoRegister{
    unsigned char byte0;
    unsigned char byte1;
    unsigned char byte2;
    unsigned char byte3;
} pseudoRegister;
typedef struct _psudoRegisterSet{
    pseudoRegister mainRegister[32];
    pseudoRegister pc;
    pseudoRegister hi;
    pseudoRegister lo;
} pseudoRegisterSet;

typedef struct _pseudoDataMemory{
    unsigned char* memory;
} pseudoDataMemory;

void readRFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3 );
void readIFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3 );
void readJFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3 );

void initPseudoRegisterSet( pseudoRegisterSet* prs);
void initPseudoDataMemory( pseudoDataMemory* pdm);

int runRFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3, pseudoRegisterSet* prs, int* pseudoPC, pseudoDataMemory* pdm );
int runIFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3, pseudoRegisterSet* prs, int* pseudoPC, pseudoDataMemory* pdm );
int runJFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3, pseudoRegisterSet* prs, int* pseudoPC, pseudoDataMemory* pdm );

INT32 getPseudoRegisterVal( pseudoRegister pr );
UINT32 getPseudoRegisterValU( pseudoRegister pr );
void setPseudoRegisterVal( pseudoRegister* pr, INT32 value );
void setPseudoRegisterValU( pseudoRegister* pr, UINT32 value );


int main(void){
    char inputStr[MAX_INPUTSTR_LEN];
    char *inputToken;
    char filename[MAX_INPUTSTR_LEN];
    char runNtimes_str[MAX_INPUTSTR_LEN];
    unsigned char* instructionMemory = malloc( MAX_INSTRUCTIONMEMORY_SIZE );
    pseudoRegisterSet registerSet0;
    pseudoDataMemory dataMemory0;




    initPseudoRegisterSet( &registerSet0 );
    initPseudoDataMemory( &dataMemory0 );


    while( 1 ){
        printf("mips-sim> ");

        fgets(inputStr, MAX_INPUTSTR_LEN, stdin);

        inputToken = strtok( inputStr, " \n");

        if( strcmp(inputToken, "exit") == 0 ){
            break;
        }

        else if( strcmp(inputToken, "read") == 0 ){
            FILE *fp = NULL;
            int instNum = 0;

            inputToken = strtok( NULL, " \n");
            strcpy( filename, inputToken );

            fp = fopen(filename, "rb");

            while( 1 ){
                int byte;
                unsigned char buffer[4];
                int opcode;

                byte = fread( buffer, 1, 4, fp);
                if(byte<=0) break;

                opcode = buffer[0];
                opcode >>= 2;
                opcode &= 0x3F;

                printf("inst %d: ", instNum );
                instNum++;
                printf("%02x%02x%02x%02x ", buffer[0], buffer[1], buffer[2], buffer[3]);

                switch(opcode) {
                    case 0:
                        readRFormat( buffer[0], buffer[1], buffer[2], buffer[3] );
                        break;

                    case 8:
                    case 9:
                    case 12:
                    case 4:
                    case 5:
                    case 32:
                    case 36:
                    case 33:
                    case 37:
                    case 15:
                    case 35:
                    case 13:
                    case 40:
                    case 10:
                    case 11:
                    case 41:
                    case 43:
                    case 14:
                        readIFormat( buffer[0], buffer[1], buffer[2], buffer[3] );
                        break;

                    case 2:
                    case 3:
                        readJFormat( buffer[0], buffer[1], buffer[2], buffer[3] );
                        break;
                    
                    default:
                        printf("unknown instruction");
                    
                }

                printf("\n");
                
            }

            fclose(fp);
        }

        else if( strcmp(inputToken, "loadinst") == 0 ){
            FILE *fp = NULL;
            int byte;
            unsigned char buffer;
            int idx = 0;


            inputToken = strtok( NULL, " \n");
            strcpy( filename, inputToken );

            fp = fopen(filename, "rb");

            while(1){ 
                byte = fread( &buffer, 1, 1, fp);
                if(byte<=0) break;
                instructionMemory[idx] = buffer;
                idx++;
            }

            for( ; idx<MAX_INSTRUCTIONMEMORY_SIZE; idx++){
                instructionMemory[idx] = 0xFF;
            }

           /* for(int i=0 ; i<=100; i++){
                if( ! (i%4) && i!=0 )
                    printf("\n");
                printf("%02x", (int)instructionMemory[i]);
            }*/

            fclose(fp);
        }

        else if( strcmp(inputToken, "loaddata") == 0 ){
            FILE *fp = NULL;
            int byte;
            unsigned char buffer;
            int idx = 0;


            inputToken = strtok( NULL, " \n");
            strcpy( filename, inputToken );

            fp = fopen(filename, "rb");

            while(1){ 
                byte = fread( &buffer, 1, 1, fp);
                if(byte<=0) break;
                dataMemory0.memory[0x10000000+idx] = buffer;
                idx++;
            }

            for( ; idx<MAX_DATAMEMORY_SIZE; idx++){
                dataMemory0.memory[0X10000000+idx] = 0xFF;
            }
        }

        else if( strcmp(inputToken, "run") == 0 ){
            int runNtimes = 0;
            int pseudoPC = 0;
            unsigned char kondonoInstWa[4];
            int errorCode = 0;
            int syorishitaINSTKazu = 0;

            inputToken = strtok( NULL, " \n");
            strcpy( runNtimes_str, inputToken );

            runNtimes = atoi(runNtimes_str);

            initPseudoRegisterSet( &registerSet0 );

            for( int i=0; i<runNtimes; i++ ){
                int opcode;

                kondonoInstWa[0] = instructionMemory[pseudoPC];
                kondonoInstWa[1] = instructionMemory[pseudoPC+1];
                kondonoInstWa[2] = instructionMemory[pseudoPC+2];
                kondonoInstWa[3] = instructionMemory[pseudoPC+3];

                opcode = kondonoInstWa[0];
                opcode >>= 2;
                opcode &= 0x3F;

                switch(opcode) {
                    case 0:
                        errorCode = runRFormat(kondonoInstWa[0], kondonoInstWa[1], kondonoInstWa[2], kondonoInstWa[3], &registerSet0, &pseudoPC, &dataMemory0);
                        break;

                    case 8:
                    case 9:
                    case 12:
                    case 15:
                    case 13:
                    case 10:
                    case 11:
                    case 14:
                    case 35:
                    case 33:
                    case 37:
                    case 32:
                    case 36:
                    case 43:
                    case 41:
                    case 40:
                    case 4:
                    case 5:
                        errorCode = runIFormat(kondonoInstWa[0], kondonoInstWa[1], kondonoInstWa[2], kondonoInstWa[3], &registerSet0, &pseudoPC, &dataMemory0);
                        break;

                    case 2:
                    case 3:
                        errorCode = runJFormat(kondonoInstWa[0], kondonoInstWa[1], kondonoInstWa[2], kondonoInstWa[3], &registerSet0, &pseudoPC, &dataMemory0);
                        break;

                    default:
                        errorCode = 1;
                        printf("unknown instruction");
                        break;
                }
                
                pseudoPC += 4;
                registerSet0.pc.byte0 = pseudoPC/0x1000000;
                registerSet0.pc.byte1 = (pseudoPC/0x10000)%0x100;
                registerSet0.pc.byte2 = (pseudoPC/0x100)%0x100;
                registerSet0.pc.byte3 = pseudoPC%0x100;
                syorishitaINSTKazu++;

                if (errorCode){
                    printf("\n");
                    break;
                }

                

                
            }
            
            printf("Executed %d instructions\n", syorishitaINSTKazu);
        }

        else if( strcmp(inputToken, "registers") == 0 ){
            for(int i=0; i<32; i++){
                printf("$%d: ", i);
                printf("0x");
                printf("%02x", registerSet0.mainRegister[i].byte0);
                printf("%02x", registerSet0.mainRegister[i].byte1);
                printf("%02x", registerSet0.mainRegister[i].byte2);
                printf("%02x", registerSet0.mainRegister[i].byte3);
                printf("\n");
            }
            printf("HI: ");
            printf("0x");
            printf("%02x", registerSet0.hi.byte0);
            printf("%02x", registerSet0.hi.byte1);
            printf("%02x", registerSet0.hi.byte2);
            printf("%02x", registerSet0.hi.byte3);
            printf("\n");
            printf("LO: ");
            printf("0x");
            printf("%02x", registerSet0.lo.byte0);
            printf("%02x", registerSet0.lo.byte1);
            printf("%02x", registerSet0.lo.byte2);
            printf("%02x", registerSet0.lo.byte3);
            printf("\n");
            printf("PC: ");
            printf("0x");
            printf("%02x", registerSet0.pc.byte0);
            printf("%02x", registerSet0.pc.byte1);
            printf("%02x", registerSet0.pc.byte2);
            printf("%02x", registerSet0.pc.byte3);
            printf("\n");
        }
    }

    free(instructionMemory);
    free(dataMemory0.memory);

    return 0;
}


void readRFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3){
    int opcode, rs, rt, rd, shamt, funct;

    opcode = b0/4;
    rs = ((b0%4)<<3) + b1/32;
    rt = b1%32;
    rd = b2/8;
    shamt = ((b2%8)<<2) + b3/64;
    funct = b3%64;

    switch(funct){
        case 32: // add rd, rs, rt 
            printf("add $%d, $%d, $%d", rd, rs, rt);
            break;
        case 33: // addu rd, rs, rt
            printf("addu $%d, $%d, $%d", rd, rs, rt);
            break;
        case 36: // and rd, rs, rt
            printf("and $%d, $%d, $%d", rd, rs, rt);
            break;
        case 26: //div rs, rt
            printf("div $%d, $%d", rs, rt);
            break;
        case 27: //divu rs, rt
            printf("divu $%d, $%d", rs, rt);
            break;
        case 9: //jalr ($x), $y
            if(rd == 31)
                printf("jalr $%d", rs);
            else
                printf("jalr $%d, $%d", rd, rs);
            break;
        case 8: //jr rs
            printf("jr $%d", rs);
            break;
        case 16: //mfhi $d
            printf("mfhi $%d", rd);
            break;
        case 18: //mflo $d
            printf("mflo $%d", rd);
            break;
        case 17: //mthi $s
            printf("mthi $%d", rs);
            break;
        case 19: //mtlo $s
            printf("mtlo $%d", rs);
            break;
        case 24: // mult rs, rt
            printf("mult $%d, $%d", rs, rt);
            break;
        case 25: // multu rs, rt
            printf("multu $%d, $%d", rs, rt);
            break;
        case 39: //nor rd, rs, rt
            printf("nor $%d, $%d, $%d", rd, rs, rt);
            break;
        case 37: //or rd, rs, rt
            printf("or $%d, $%d, $%d", rd, rs, rt);
            break;
        case 0: //sll rd, rt, shamt
            printf("sll $%d, $%d, %d", rd, rt, shamt);
            break;
        case 4: //sllv rd, rt, rs
            printf("sllv $%d, $%d, $%d", rd, rt, rs);
            break;
        case 42: //slt rd, rs, rt
            printf("slt $%d, $%d, $%d", rd, rs, rt);
            break;
        case 43: //sltu rd, rs, rt
            printf("sltu $%d, $%d, $%d", rd, rs, rt);
            break;
        case 3: //sra rd, rt, shamt
            printf("sra $%d, $%d, %d", rd, rt, shamt);
            break;
        case 7: //srav rd, rt, rs
            printf("srav $%d, $%d, $%d", rd, rt, rs);
            break;
        case 2: //srl rd, rt, shamt
            printf("srl $%d, $%d, %d", rd, rt, shamt);
            break;
        case 6: //srlv rd, rt, rs
            printf("srlv $%d, $%d, $%d", rd, rt, rs);
            break;
        case 34: //sub rd, rs, rt
            printf("sub $%d, $%d, $%d", rd, rs, rt);
            break;
        case 35: //subu rd, rs, rt
            printf("subu $%d, $%d, $%d", rd, rs, rt);
            break;
        case 12: //syscall
            printf("syscall");
            break;
        case 38: //xor rd, rs, rt
            printf("xor $%d, $%d, $%d", rd, rs, rt);
            break;



        default:
        printf("unknown instruction");

    }


}


void readIFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3){
    int opcode, rs, rt, immedi;

    opcode = b0/4;
    rs = ((b0%4)<<3) + b1/32;
    rt = b1%32;

    if( !(b2 & 0x80) ) { // MSB of 16bit of immediate is 0
        immedi = (b2<<8) + b3;
    }
    else {
        immedi = ( (b2<<8) + b3 ) - (1<<16) ;
    }

    switch( opcode ) {
        case 8: // addi rt, rs, immedi
            printf("addi $%d, $%d, %d", rt, rs, immedi);
            break;
        case 9: // addiu rt, rs, immedi
            printf("addiu $%d, $%d, %d", rt, rs, immedi);
            break;
        case 12: // andi rt, rs, immedi  or  andi rt, immedi
            if( rs != rt )
                printf("andi $%d, $%d, %d", rt, rs, immedi);
            else
                printf("andi $%d, %d", rt, immedi);
            break;
        case 4: // beq rs, rt, immedi
            printf("beq $%d, $%d, %d", rs, rt, immedi);
            break;
        case 5: // bne rs, rt, immedi
            printf("bne $%d, $%d, %d", rs, rt, immedi);
            break;
        case 32: // lb rt, immedi(rs)
            if( rs )
                printf("lb $%d, %d($%d)", rt, immedi, rs);
            else
                printf("lb $%d, %d", rt, immedi);
            break;
         case 36: // lbu rt, immedi(rs)
            if( rs )
                printf("lbu $%d, %d($%d)", rt, immedi, rs);
            else
                printf("lbu $%d, %d", rt, immedi);
            break;
        case 33: // lh rt, immedi(rs)
            if( rs )
                printf("lh $%d, %d($%d)", rt, immedi, rs);
            else
                printf("lh $%d, %d", rt, immedi);
            break;
        case 37: // lhu rt, immedi(rs)
            if( rs )
                printf("lhu $%d, %d($%d)", rt, immedi, rs);
            else
                printf("lhu $%d, %d", rt, immedi);
            break;
        case 15: // lui rt, immedi
            printf("lui $%d, %d", rt, immedi);
            break;
        case 35: //lw rt, immedi(rs)
            if( rs )
                printf("lw $%d, %d($%d)", rt, immedi, rs);
            else
                printf("lw $%d, %d", rt, immedi);
            break;
        case 13: // ori rt, rs, immedi  or  ori rt, immedi
            if( rs != rt )
                printf("ori $%d, $%d, %d", rt, rs, immedi);
            else
                printf("ori $%d, %d", rt, immedi);
            break;
        case 40: //sb rt, immedi(rs)
            if( rs )
                printf("sb $%d, %d($%d)", rt, immedi, rs);
            else
                printf("sb $%d, %d", rt, immedi);
            break;
        case 10: // slti rt, rs, 100
            printf("slti $%d, $%d, %d", rt, rs, immedi);
            break;
        case 11: // sltiu rt, rs 100
            printf("sltiu $%d, $%d, %d", rt, rs, immedi);
            break;
        case 41: //sh rt, immedi(rs)
            if( rs )
                printf("sh $%d, %d($%d)", rt, immedi, rs);
            else
                printf("sh $%d, %d", rt, immedi);
            break;
        case 43: //sw rt, immedi(rs)
            if( rs )
                printf("sw $%d, %d($%d)", rt, immedi, rs);
            else
                printf("sw $%d, %d", rt, immedi);
            break;
        case 14: //xori rt, rs, immedi
             if( rs != rt )
                printf("xori $%d, $%d, %d", rt, rs, immedi);
            else
                printf("xori $%d, %d", rt, immedi);
            break;
            
    }


}

void readJFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3){
    int opcode, address;

    opcode = b0/4;

    if( !(b0%4 & 2) )
        address = ((b0%4)<<24) + (b1<<16) + (b2<<8) + b3;
    else
        address = ((b0%4)<<24) + (b1<<16) + (b2<<8) + b3  - (1<<26);
    

    switch(opcode){
        case 2:
            printf("j %d", address);
            break;
        case 3:
            printf("jal %d", address);
            break;
    }

}

void initPseudoRegisterSet( pseudoRegisterSet* prs){ 
    for(int i=0; i<32; i++){
        prs->mainRegister[i].byte0 = prs->mainRegister[i].byte1 = prs->mainRegister[i].byte2 = prs->mainRegister[i].byte3 = 0;
    }
    prs->hi.byte0 = prs->hi.byte1 = prs->hi.byte2 = prs->hi.byte3 = 0;
    prs->lo.byte0 = prs->lo.byte1 = prs->lo.byte2 = prs->lo.byte3 = 0;
    prs->pc.byte0 = prs->pc.byte1 = prs->pc.byte2 = prs->pc.byte3 = 0;
}
void initPseudoDataMemory( pseudoDataMemory* pdm){ 
    pdm->memory = malloc(sizeof(unsigned char) * (0x10000000 + MAX_DATAMEMORY_SIZE));

    for(int i=0; i<MAX_DATAMEMORY_SIZE; i++){
        pdm->memory[0x10000000+i] = 0xff;
    }    
}


int runRFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3, pseudoRegisterSet *prs, int* pseudoPC, pseudoDataMemory* pdm ){
    int opcode, rs, rt, rd, shamt, funct;
    
    INT32 rsval, rtval, rdval, hival, loval;
    UINT32 rsvalu, rtvalu, rdvalu, hivalu, lovalu;


    opcode = b0/4;
    rs = ((b0%4)<<3) + b1/32;
    rt = b1%32;
    rd = b2/8;
    shamt = ((b2%8)<<2) + b3/64;
    funct = b3%64;


    rsval = getPseudoRegisterVal( prs->mainRegister[rs] );
    rtval = getPseudoRegisterVal( prs->mainRegister[rt] );
    rdval = getPseudoRegisterVal( prs->mainRegister[rd] );
    rsvalu = getPseudoRegisterValU( prs->mainRegister[rs] );
    rtvalu = getPseudoRegisterValU( prs->mainRegister[rt] );
    rdvalu = getPseudoRegisterValU( prs->mainRegister[rd] );



    switch(funct){
        case 32: // add rd, rs, rt
            rdvalu = rsvalu + rtvalu;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
        break;
        case 33: // addu rd, rs, rt
            rdvalu = rsvalu + rtvalu;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
        case 36: // and rd, rs, rt
            rdvalu = rsvalu & rtvalu;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
         case 39: //nor rd, rs, rt
            rdvalu = ~(rsvalu | rtvalu);
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
        case 37: //or rd, rs, rt
            rdvalu = rsvalu | rtvalu;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
        case 42: //slt rd, rs, rt
            if( rsval < rtval )
                setPseudoRegisterVal( &(prs->mainRegister[rd]), 1 );
            else
                setPseudoRegisterVal( &(prs->mainRegister[rd]), 0 );
            break;
        case 43: //sltu rd, rs, rt
            if( rsvalu < rtvalu )
                setPseudoRegisterValU( &(prs->mainRegister[rd]), 1 );
            else
                setPseudoRegisterValU( &(prs->mainRegister[rd]), 0 );
            break;
        case 34: //sub rd, rs, rt
            rdvalu = rsvalu - rtvalu;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
        case 35: //subu rd, rs, rt
            rdvalu = rsvalu - rtvalu;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
        case 38: //xor rd, rs, rt
            rdvalu = rsvalu ^ rtvalu;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;

        case 0: //sll rd, rt, shamt
            rdvalu = rtvalu << shamt;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
        case 4: //sllv rd, rt, rs
            rdvalu = rtvalu << (rsvalu%32);
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
        case 3: //sra rd, rt, shamt
            rdval = rtval >> shamt;
            setPseudoRegisterVal( &(prs->mainRegister[rd]), rdval );
            break;
        case 7: //srav rd, rt, rs
            rdval = rtval >> (rsvalu%32);
            setPseudoRegisterVal( &(prs->mainRegister[rd]), rdval );
            break;
        case 2: //srl rd, rt, shamt
            rdvalu = rtvalu >> shamt;
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;
        case 6: //srlv rd, rt, rs
            rdvalu = rtvalu >> (rsvalu%32);
            setPseudoRegisterValU( &(prs->mainRegister[rd]), rdvalu );
            break;

        case 26: //div rs, rt
            hival = rsval % rtval;
            loval = rsval / rtval;            
            setPseudoRegisterVal( &(prs->hi), hival );
            setPseudoRegisterVal( &(prs->lo), loval );
            break;
        case 27: //divu rs, rt
            hivalu = rsvalu % rtvalu;
            lovalu = rsvalu / rtvalu;            
            setPseudoRegisterValU( &(prs->hi), hivalu );
            setPseudoRegisterValU( &(prs->lo), lovalu );
            break;
        case 16: //mfhi $d
            prs->mainRegister[rd].byte0 = prs->hi.byte0;
            prs->mainRegister[rd].byte1 = prs->hi.byte1;
            prs->mainRegister[rd].byte2 = prs->hi.byte2;
            prs->mainRegister[rd].byte3 = prs->hi.byte3;
            break;
        case 18: //mflo $d
            prs->mainRegister[rd].byte0 = prs->lo.byte0;
            prs->mainRegister[rd].byte1 = prs->lo.byte1;
            prs->mainRegister[rd].byte2 = prs->lo.byte2;
            prs->mainRegister[rd].byte3 = prs->lo.byte3;
            break;
        case 17: //mthi $s
            prs->hi.byte0 = prs->mainRegister[rs].byte0;
            prs->hi.byte1 = prs->mainRegister[rs].byte1;
            prs->hi.byte2 = prs->mainRegister[rs].byte2;
            prs->hi.byte3 = prs->mainRegister[rs].byte3;
            break;
        case 19: //mtlo $s
            prs->lo.byte0 = prs->mainRegister[rs].byte0;
            prs->lo.byte1 = prs->mainRegister[rs].byte1;
            prs->lo.byte2 = prs->mainRegister[rs].byte2;
            prs->lo.byte3 = prs->mainRegister[rs].byte3;
            break;
        case 24: // mult rs, rt
            hival = (int)(( (long long)rsval * (long long)rtval ) >> 32);
            loval = (int)(( (long long)rsval * (long long)rtval ) % 0x100000000);
            setPseudoRegisterVal( &(prs->hi), hival );
            setPseudoRegisterVal( &(prs->lo), loval );
            break;
        case 25: // multu rs, rt        
            hivalu = (unsigned int)(( (unsigned long long)rsvalu * (unsigned long long)rtvalu ) >> 32);
            lovalu = (unsigned int)(( (unsigned long long)rsvalu * (unsigned long long)rtvalu ) % 0x100000000);
            setPseudoRegisterValU( &(prs->hi), hivalu );
            setPseudoRegisterValU( &(prs->lo), lovalu );
            break;

        case 9: //jalr ($x), $y
            setPseudoRegisterVal( &(prs->mainRegister[rd]), (*pseudoPC)+4 );
            *pseudoPC = rdvalu-4;
            break;
        case 8: //jr rs
            *pseudoPC = rsvalu-4;
            break;

        case 12: //syscall
            if( getPseudoRegisterVal( prs->mainRegister[2] ) == 1 ){
                printf("%d", getPseudoRegisterVal( prs->mainRegister[4] ));
            }
            else if( getPseudoRegisterVal( prs->mainRegister[2] ) == 4 ){
                int dataMemoryPointer = getPseudoRegisterVal( prs->mainRegister[4] );

                for( ; pdm->memory[dataMemoryPointer] != 0 ;dataMemoryPointer++ ){
                    printf("%c", pdm->memory[dataMemoryPointer]);
                }
            }
            else if( getPseudoRegisterVal( prs->mainRegister[2] ) == 10 ){
                printf("EXIT syscall");
                return 4;
            }
            else{
                printf("Invalid syscall");
                return 5;
            }
            break;
            
        default :
            printf("unknown instruction");
            return 1;
    }

    return 0;
}
int runIFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3, pseudoRegisterSet* prs, int* pseudoPC, pseudoDataMemory* pdm ){
    int opcode, rs, rt, immedi;
    INT32 rsval, rtval;
    UINT32 rsvalu, rtvalu;
    
    int dataMemAddr;

    opcode = b0/4;
    rs = ((b0%4)<<3) + b1/32;
    rt = b1%32;

    if( !(b2 & 0x80) ) { // MSB of 16bit of immediate is 0
        immedi = (b2<<8) + b3;
    }
    else {
        immedi = ( (b2<<8) + b3 ) - (1<<16) ;
    }

    rsval = getPseudoRegisterVal( prs->mainRegister[rs] );
    rtval = getPseudoRegisterVal( prs->mainRegister[rt] );
    rsvalu = getPseudoRegisterValU( prs->mainRegister[rs] );
    rtvalu = getPseudoRegisterValU( prs->mainRegister[rt] );

    dataMemAddr = rsval + immedi;


    if( opcode == 35 || opcode == 33 || opcode == 37 || opcode == 32 || opcode == 36 || opcode == 43 || opcode == 41 || opcode == 40 ){
        if( dataMemAddr < 0x10000000 || dataMemAddr > 0x10010000 ){
            printf("Memory address out of range: 0x%08x", dataMemAddr);
            return 2;
        }
    }


    switch(opcode){
        case 8: // addi rt, rs, immedi
            rtval = rsval + immedi;
            setPseudoRegisterVal( &(prs->mainRegister[rt]), rtval );
            break;
        case 9: // addiu rt, rs, immedi
            rtval = rsval + immedi;
            setPseudoRegisterVal( &(prs->mainRegister[rt]), rtval );
            break;
        case 12: // andi rt, rs, immedi  or  andi rt, immedi
            rtvalu = rsvalu & ((b2<<8) + b3);
            setPseudoRegisterValU( &(prs->mainRegister[rt]), rtvalu );
            break;
        case 15: // lui rt, immedi
            rtvalu = ((b2<<8) + b3)<<16;
            setPseudoRegisterValU( &(prs->mainRegister[rt]), rtvalu );
            break;
        case 13: // ori rt, rs, immedi  or  ori rt, immedi            
            rtvalu = rsvalu | ((b2<<8) + b3);
            setPseudoRegisterValU( &(prs->mainRegister[rt]), rtvalu );
            break;
        case 10: // slti rt, rs, 100
            if( rsval < immedi )
                setPseudoRegisterVal( &(prs->mainRegister[rt]), 1 );
            else
                setPseudoRegisterVal( &(prs->mainRegister[rt]), 0 );            
            break;
        case 11: // sltiu rt, rs, 100
            if( rsvalu < ((b2<<8) + b3) )
                setPseudoRegisterValU( &(prs->mainRegister[rt]), 1 );
            else
                setPseudoRegisterValU( &(prs->mainRegister[rt]), 0 );  
            break;
        case 14: //xori rt, rs, immedi
            rtvalu = rsvalu ^ ((b2<<8) + b3);
            setPseudoRegisterValU( &(prs->mainRegister[rt]), rtvalu );
            break;

        case 35: //lw rt, immedi(rs)
            if( dataMemAddr % 4 ){
                printf("Memory address alignment error: 0x%08x", dataMemAddr);
                return 3;
            }
            prs->mainRegister[rt].byte0 = pdm->memory[dataMemAddr];
            prs->mainRegister[rt].byte1 = pdm->memory[dataMemAddr+1];
            prs->mainRegister[rt].byte2 = pdm->memory[dataMemAddr+2];
            prs->mainRegister[rt].byte3 = pdm->memory[dataMemAddr+3];
            break;
        case 33: // lh rt, immedi(rs)
            if( dataMemAddr % 2 ){
                printf("Memory address alignment error: 0x%08x", dataMemAddr);
                return 3;
            }

            if( (signed char)(pdm->memory[dataMemAddr]) >= 0 ) {
                prs->mainRegister[rt].byte0 = 0x0;
                prs->mainRegister[rt].byte1 = 0x0;
                prs->mainRegister[rt].byte2 = pdm->memory[dataMemAddr];
                prs->mainRegister[rt].byte3 = pdm->memory[dataMemAddr+1];
            }
            else{
                prs->mainRegister[rt].byte0 = 0xFF;
                prs->mainRegister[rt].byte1 = 0xFF;
                prs->mainRegister[rt].byte2 = pdm->memory[dataMemAddr];
                prs->mainRegister[rt].byte3 = pdm->memory[dataMemAddr+1];
            }
            break;
        case 37: // lhu rt, immedi(rs)
            if( dataMemAddr % 2 ){
                printf("Memory address alignment error: 0x%08x", dataMemAddr);
                return 3;
            }
            prs->mainRegister[rt].byte0 = 0x0;
            prs->mainRegister[rt].byte1 = 0x0;
            prs->mainRegister[rt].byte2 = pdm->memory[dataMemAddr];
            prs->mainRegister[rt].byte3 = pdm->memory[dataMemAddr+1];
            break;
        case 32: // lb rt, immedi(rs)
            if( (signed char)(pdm->memory[dataMemAddr]) >= 0 ) {
                prs->mainRegister[rt].byte0 = 0x0;
                prs->mainRegister[rt].byte1 = 0x0;
                prs->mainRegister[rt].byte2 = 0x0;
                prs->mainRegister[rt].byte3 = pdm->memory[dataMemAddr];
            }
            else{
                prs->mainRegister[rt].byte0 = 0xFF;
                prs->mainRegister[rt].byte1 = 0xFF;
                prs->mainRegister[rt].byte2 = 0xFF;
                prs->mainRegister[rt].byte3 = pdm->memory[dataMemAddr];
            }
            break;
        case 36: // lbu rt, immedi(rs)
            prs->mainRegister[rt].byte0 = 0x0;
            prs->mainRegister[rt].byte1 = 0x0;
            prs->mainRegister[rt].byte2 = 0x0;
            prs->mainRegister[rt].byte3 = pdm->memory[dataMemAddr];
            break;
        case 43: //sw rt, immedi(rs)
            if( dataMemAddr % 4 ){
                printf("Memory address alignment error: 0x%08x", dataMemAddr);
                return 3;
            }
            pdm->memory[dataMemAddr] = prs->mainRegister[rt].byte0;
            pdm->memory[dataMemAddr+1] = prs->mainRegister[rt].byte1;
            pdm->memory[dataMemAddr+2] = prs->mainRegister[rt].byte2;
            pdm->memory[dataMemAddr+3] = prs->mainRegister[rt].byte3;
            break;
        case 41: //sh rt, immedi(rs)
            if( dataMemAddr % 2 ){
                printf("Memory address alignment error: 0x%08x", dataMemAddr);
                return 3;
            }
            pdm->memory[dataMemAddr] = prs->mainRegister[rt].byte2;
            pdm->memory[dataMemAddr+1] = prs->mainRegister[rt].byte3;
            break;            
        case 40: //sb rt, immedi(rs)        
            pdm->memory[dataMemAddr] = prs->mainRegister[rt].byte3;
            break;

        case 4: // beq rs, rt, immedi
            if( rsval == rtval ){
                *pseudoPC = (*pseudoPC) + immedi*4;
            }
            break;
        case 5: // bne rs, rt, immedi
            if( rsval != rtval ){
                *pseudoPC = (*pseudoPC) + immedi*4;
            }
            break;
    }

    return 0;
}
int runJFormat( unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3, pseudoRegisterSet* prs, int* pseudoPC, pseudoDataMemory* pdm ){
    int opcode, address;

    opcode = b0/4;

    if( !(b0%4 & 2) )
        address = ((b0%4)<<24) + (b1<<16) + (b2<<8) + b3;
    else
        address = ((b0%4)<<24) + (b1<<16) + (b2<<8) + b3  - (1<<26);
    

    switch(opcode){
        case 2: //j
            *pseudoPC = (((*pseudoPC)>>28)<<28) + (address<<2) -4;
            break;
        case 3: //jal
            setPseudoRegisterVal( &(prs->mainRegister[31]), (*pseudoPC)+4 );
            *pseudoPC = (((*pseudoPC)>>28)<<28) + (address<<2) -4;
            break;
    }

    return 0;
}


INT32 getPseudoRegisterVal( pseudoRegister pr){
    INT32 result;
    UINT32 uValue;

    uValue = pr.byte0 * 0x1000000 + pr.byte1 * 0x10000 + pr.byte2 * 0x100 + pr.byte3;

    if(pr.byte0 & 0x80){
        uValue = ~uValue;
        uValue ++;
        result = -1 * (INT32)uValue;
    }
    else
        result = uValue;    

    return result;
}
UINT32 getPseudoRegisterValU( pseudoRegister pr){
    UINT32 result;

    result = pr.byte0 * 0x1000000 + pr.byte1 * 0x10000 + pr.byte2 * 0x100 + pr.byte3;

    return result;
}
void setPseudoRegisterVal( pseudoRegister* pr, INT32 value ){
    UINT32 result;

    result = value;
    
    pr->byte0 = result / 0x1000000;
    pr->byte1 = (result / 0x10000) % 0x100;
    pr->byte2 = (result / 0x100) % 0x100;
    pr->byte3 = result % 0x100;
}
void setPseudoRegisterValU( pseudoRegister* pr, UINT32 value ){
    pr->byte0 = value / 0x1000000;
    pr->byte1 = (value / 0x10000) % 0x100;
    pr->byte2 = (value / 0x100) % 0x100;
    pr->byte3 = value % 0x100;
}