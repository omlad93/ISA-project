#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>

#define SRL_MASK 0x7FFFFFFF
#define LSB_9BIT 0x000001FF
#define RA 15
#define MAX_LINE 500

extern unsigned int IOs[22];
extern int REG[16];
extern FILE			*HwRT, *Leds;
extern int			monitor[288][352];
extern int			Disk[128][128];
extern int			Data_Mem[4096];
extern char			Op_Mem[1024][6];
extern int			already_irq;

/* **********************************************************/
/*  ~~~~~~~~~~~~~~~    OPERATION  STRUCT    ~~~~~~~~~~~~~~  */
/* **********************************************************/

typedef struct operation {
	int rd; //rd register
	int rs; //rs register
	int rt; //rt register

	int imm_used;

	char* inst; // the opcode line from Imem
	int (*op_code)(struct operation* op, int pc); // a function pointer according to opcode
} operation;



/* **********************************************************/
/*  ~~~~~~~~~~~~~    FUNCTION DECLARATIONS    ~~~~~~~~~~~~  */
/* **********************************************************/
void* set_op_by_code(int code, operation* op);

char* IO_reg_name(int reg_num);



void set_operation(operation *op, int d, int t, int s, int code, char *inst);



//SIMP op_codes 

//1
int add(operation *op, int *registers, int pc);

//2
int sub(operation *op, int *registers, int pc);

//3
int and(operation* op, int *registers, int pc);

//4
int or (operation* op, int* registers, int pc);

//5
int xor(operation* op, int* registers, int pc);

//6
int mul(operation* op, int* registers, int pc);

//6
int sll(operation* op, int* registers, int pc);

//7
int sra(operation* op, int* registers, int pc);

//8
int srl(operation* op, int* registers, int pc);

//9
int beq(operation* op, int* registers, int pc);

//10
int bne(operation* op, int* registers, int pc);

//11
int blt(operation* op, int* registers, int pc);

//12
int bgt(operation* op, int* registers, int pc);

//13
int ble(operation* op, int* registers, int pc);

//14
int bge(operation* op, int* registers, int pc);

//15
int jal(operation* op, int* registers, int pc);

//16
int lw(operation* op, int* registers, int pc);

//17
int sw(operation* op, int* registers, int pc);

//18
int reti(operation* op, int* registers, int pc);

//19
int in(operation* op, int* registers, int pc);

//20
int out(operation* op, int* registers, int pc);

//21
int halt(operation* op, int* registers, int pc);

//just increment pc
int nop(operation*op, int* registers, int pc);