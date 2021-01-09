#include "opp.h"

/*
this module implemnts the header opp.c

you can find in this nodule 22 functions which can be pointed by the operatin DS:
1 for each op from the ISA and additional no-op for unreconized op_codes or assignment to $0
the opcode number is commented above the function

OUT & IN operation also simulate IO behivour
*/

/*
set the functions to be performed in the *op_code field in operation struct
*/
void* set_op_by_code(int code, operation* op) {

	//if operations should do nothing (on assignment to $0 )
	if (((code <= 8) || (code == 16) || (code == 19))) {
		if (op->rd == 0) {
			return &nop;
		}
	}
	else if ((op->rd > 15) || (op->rs > 15) || (op->rt > 15)) {
		return &nop;
	}

	
	//else go by opcode
	switch (code) {
	case(0):
		return &add;
	case(1):
		return &sub;
	case(2):
		return &and;
	case(3):
		return &or ;
	case(4):
		return &xor;
	case(5):
		return &mul;
	case(6):
		return &sll;
	case(7):
		return &sra;
	case(8):
		return &srl;
	case(9):
		return &beq;
	case(10):
		return &bne;
	case(11):
		return &blt;
	case(12):
		return &bgt;
	case(13):
		return &ble;
	case(14):
		return &bge;
	case(15):
		return &jal;
	case(16):
		return &lw;
	case(17):
		return &sw;
	case(18):
		return &reti;
	case(19):
		return &in;
	case(20):
		return &out;
	case(21):
		return &halt;
	//if unknown opcode -> do nop
	default:
		return &nop;
	}
}

/*
return the IO-register name by it's number
*/
char* IO_reg_name(int reg_num) {
	switch (reg_num) {
	case(0):
		return "irq0enable";
	case(1):
		return "irq1enable";
	case(2):
		return "irq2enable";
	case(3):
		return "irq0status";
	case(4):
		return "irq1status";
	case(5):
		return "irq2status";
	case(6):
		return "irqhandler";
	case(7):
		return "irqreturn";
	case(8):
		return "clks";
	case(9):
		return "leds";
	case(10):
		return "reserved";
	case(11):
		return "timerenable";
	case(12):
		return "timercurrent";
	case(13):
		return "timermax";
	case(14):
		return "diskcmd";
	case(15):
		return "disksector";
	case(16):
		return "diskbuffer";
	case(17):
		return("diskstatus");
	case(18):
		return("monitorcmd");
	case(19):
		return("monitorx");
	case(20):
		return("monitory");
	case(21): 
		return("monitordata");
	default:
		return "";
	}
}

/*
 return 1 if an immediate is used in the operation
 */
int imm_usage(operation* op) {
	if (op->rs == 1 || op->rd == 1 || op->rt == 1) {
		return 1;
	}
	else {
		return  0;
	}
}

/*
load input values to fields in the operation struct
*/
void set_operation(operation* op, int d, int t, int s,int code, char* inst) {
	op->rd = d;
	op->rt = t;
	op->rs = s;
	op->op_code = set_op_by_code(code, op);
	op->inst = inst;
	op->imm_used = imm_usage(op);

}

/*
copies memory from one memory elemnt to enother
used to copy from Data Memory to Hard Disk or vise-versa
*/
void mem_copy(int *origin,int* dest ) {
	int j;
	for (j = 0; j < 128; j++) { //copy sector
		dest[j] = origin[j];
	}
}

/*
A function to make sure that mem copy between Disk & Data-Mem will go clean
if one of the pointer is set to an addresss which will cause a problem - it is shifter to the mximum address which won't
to be called before mem_copy
*/
void fix_disk_cpy(unsigned int disk_sec, unsigned int dmem_loc, int* dmem_address, int* disk_address) {
	int max_dmem_to_cpy = 4096 - 128;
	if (dmem_loc > max_dmem_to_cpy) {
		dmem_address = &Data_Mem[max_dmem_to_cpy];
	}
	if (disk_sec > 127) {
		disk_address = Disk[127];
	}
}

/* **********************************************************/
/*  ~~~~~~~~~~~~~    SIMP OP CODES OPERATIONS ~~~~~~~~~~~~  */
/* **********************************************************/
//1
int add(operation* op, int pc) {
	REG[op->rd] = REG[op->rs] + REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);

}

//2
int sub(operation* op, int pc) {
	REG[op->rd] = REG[op->rs] - REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);

}

//3
int and(operation* op, int pc) {
	REG[op->rd] = REG[op->rs] & REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);

}

//4
int or (operation* op, int pc) {
	REG[op->rd] = REG[op->rs] | REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);

}

//5
int xor (operation* op, int pc) {
	REG[op->rd] = REG[op->rs] ^ REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);

}

//6
int mul(operation* op, int pc) {
	REG[op->rd] = REG[op->rs] * REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);

}

//6
int sll(operation* op, int pc) {
	REG[op->rd] = REG[op->rs] << REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);
}

//7
int sra(operation* op, int pc) {
	REG[op->rd] = REG[op->rs] >> REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}return (pc + 1);
}

//8
int srl(operation* op, int pc) {
	REG[op->rd] = (int)REG[op->rs] >> (int)REG[op->rt];
	if (op->imm_used) {
		return pc + 2;
	}return (pc + 1);
}

//9
int beq(operation* op, int pc) {
	if (REG[op->rs] != REG[op->rt]) {
		if (op->imm_used) {
			return pc + 2;
		}
		return (pc + 1);
	}

	return (REG[op->rd]& LSB_9BIT);
}

//10
int bne(operation* op, int pc) {
	if (REG[op->rs] == REG[op->rt]) {
		if (op->imm_used) {
			return pc + 2;
		}
		return (pc + 1);
	}

	return (REG[op->rd]& LSB_9BIT);
}

//11
int blt(operation* op, int pc) {

	if (REG[op->rs] >= REG[op->rt]) {
		if (op->imm_used) {
			return pc + 2;
		}
		return (pc + 1);
	}

	return  (REG[op->rd] & LSB_9BIT);
}

//12
int bgt(operation* op, int pc) {
	if (REG[op->rs] <= REG[op->rt]) {
		if (op->imm_used) {
			return pc + 2;
		}
		return (pc + 1);
	}

	return (REG[op->rd]&LSB_9BIT);
}

//13
int ble(operation* op, int pc) {
	if (REG[op->rs] > REG[op->rt]) {
		if (op->imm_used) {
			return pc + 2;
		}
		return (pc + 1);
	}
	return (REG[op->rd]&LSB_9BIT);
}

//14
int bge(operation* op, int pc) {
	if (REG[op->rs] < REG[op->rt]) {
		if (op->imm_used) {
			return pc + 2;
		}
		return (pc + 1);
	}
	return (REG[op->rd]&LSB_9BIT);
}

//15
int jal(operation* op,  int pc) {
	if (op->imm_used) {
		REG[RA] = pc + 2;
	}
	else {
		REG[RA] = pc + 1;
	}
	return (REG[op->rd]&LSB_9BIT);
}

//16
int lw(operation* op,  int pc) {
	//char value[MAX_LINE + 1];
	int memory_line = (REG[op->rs] + REG[op->rt]);
	if (memory_line > 4095 || memory_line <0) {
		memory_line = 4095; //do not allow higher addresses 
	}	REG[op->rd] = Data_Mem[memory_line];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);
}

//17
int sw(operation* op,  int pc) {
	int memory_line = (REG[op->rs] + REG[op->rt]);
	if (memory_line > 4095 || memory_line <0) {
		memory_line = 4095; //do not allow higher addresses 
	}
	Data_Mem[memory_line] = REG[op->rd];
	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);

}

//18
int reti(operation* op,  int pc) {
	pc = IOs[7];
	already_irq = 0;
	IOs[3] = 0; IOs[4] = 0; IOs[5] = 0;
	return pc;
}

//19
int in(operation* op,  int pc) {
	int io_address = REG[op->rs] + REG[op->rt];
	//int x, y, shade;
	REG[op->rd] = IOs[io_address];

	//read from monitor_cmd
	if ((op->rs + op->rd) == 18) {
		REG[op->rd] = 0;
	}
	//update HwRT
	fprintf(HwRT, "%u %s %s %08X\n",
		REG[8], "READ", IO_reg_name(io_address), REG[op->rd]);

	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);
}

//20
int out(operation* op, int pc) {
	int x,y,shade;
	int *dest, *orgn;
	
	int io_address = REG[op->rs] + REG[op->rt];
	IOs[io_address] = REG[op->rd];

	//update HwRT
	fprintf(HwRT, "%u %s %s %08X\n",
		IOs[8], "WRITE", IO_reg_name(io_address), REG[op->rd]);

	//MONITOR WRITE
	if ((io_address == 18) && REG[op->rd]) {
		x = IOs[19]; y = IOs[20]; shade = IOs[21];
		monitor[y][x] = shade;
	}
	//LEDS WRITE
	else if (io_address == 9) {
		fprintf(Leds, "%03d %08X\n",IOs[8], REG[op->rd]); //+1
	
	}
	//DISK WRITE
	else if (io_address == 14) {
		if (!IOs[17]) { // if disk is free
			if (IOs[14] == 1) { // if sholut write
				dest = Disk[IOs[15]];
				orgn = &Data_Mem[IOs[16]];
				
				fix_disk_cpy(IOs[15], IOs[16], orgn, dest);
				mem_copy(orgn, dest);
			}
			else  if (IOs[14] == 2) { // if shold read			
				dest = &Data_Mem[IOs[16]];
				orgn = Disk[IOs[15]];

				fix_disk_cpy(IOs[15], IOs[16], dest, orgn);
				mem_copy(orgn, dest);
			}
		}
	}

	if (op->imm_used) {
		return pc + 2;
	}
	return (pc + 1);
}



//21
int halt(operation* op, int pc) {
	printf("  > reached halt function [pc=%d]\n\t", pc);
	return -1;

}

//assign to $0
int nop(operation*op,  int pc) {

	return pc + 1;
}

