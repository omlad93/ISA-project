#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>

/*

this module includes the data structures used for performing SIMP operations
the data structre holds:
	4 integers			 - one for each register and one as indicator of immediate use
	1 one string (char*) - for the original line from the operation memory
	1 function pointer	 - for the functions corresponing with the operation

you can find in this nodule 22 functions which can be pointed by the operatin DS:
1 for each op from the ISA and additional no-op for unreconized op_codes or assignment to $0
the opcode number is commented above the function

also defined here few macros used in operations

declared here also extern FILE pointers and other DS simulation memories which are used by operation

*/


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

	int imm_used; // and indicator of using immediate

	char* inst; // the opcode line from Imem
	int (*op_code)(struct operation* op, int pc); // a function pointer according to opcode
} operation;



/* **********************************************************/
/*  ~~~~~~~~~~~~~    FUNCTION DECLARATIONS    ~~~~~~~~~~~~  */
/* **********************************************************/

/*
set the functions to be performed in the *op_code field in operation struct
*/
void* set_op_by_code(int code, operation* op);

/*
return the IO-register name by it's number
*/
char* IO_reg_name(int reg_num);

/*
 return 1 if an immediate is used in the operation 
 */
int imm_usage(operation* op);

/*
load input values to fields in the operation struct
*/
void set_operation(operation* op, int d, int t, int s, int code, char* inst);

/*
copies memory from one memory elemnt to enother
used to copy from Data Memory to Hard Disk or vise-versa
*/
void mem_copy(int *origin, int* dest);

/*
A function to make sure that mem copy between Disk & Data-Mem will go clean
if one of the pointer is set to an addresss which will cause a problem - it is shifter to the mximum address which won't
to be called before mem_copy
*/
void fix_disk_cpy(unsigned int disk_sec, unsigned int dmem_loc, int* dmem_address, int* disk_address);

/* **********************************************************/
/*  ~~~~~~~~~~~~~    SIMP OP CODES OPERATIONS ~~~~~~~~~~~~  */
/* **********************************************************/
//1
int add(operation* op, int pc);

//2
int sub(operation* op, int pc);

//3
int and(operation* op, int pc);

//4
int or (operation* op, int pc);

//5
int xor (operation* op, int pc);

//6
int mul(operation* op, int pc);

//6
int sll(operation* op, int pc);

//7
int sra(operation* op, int pc);

//8
int srl(operation* op, int pc);

//9
int beq(operation* op, int pc);

//10
int bne(operation* op, int pc);

//11
int blt(operation* op, int pc);

//12
int bgt(operation* op, int pc);

//13
int ble(operation* op, int pc);

//14
int bge(operation* op, int pc);

//15
int jal(operation* op, int pc);

//16
int lw(operation* op, int pc);

//17
int sw(operation* op, int pc);

//18
int reti(operation* op, int pc);

//19
int in(operation* op, int pc);

//20
int out(operation* op, int pc);

//21
int halt(operation* op, int pc);

//assign to $0 or unreconized behivour 
int nop(operation*op, int pc);

