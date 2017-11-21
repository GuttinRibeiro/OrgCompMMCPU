/* code-c - Student's code for mmcpu

   Copyright 2017  [Your Name]   [<your email>]

   This file is part of [Your Project]

   [Your Project] is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <mmcpu/cpu.h>

/* Students, you are required to implemented the functions bellow.
   Please, refere to cpu.h for further information. */

//Constantes para controle do tipo de instrução:
#define R_Type_Op 0b0000000000000000
#define Beq_Op    0b0001000000000000
#define Jump_Op   0b0000100000000000
#define LW_Op     0b1000110000000000
#define SW_Op     0b1010110000000000

//Máscaras para controlar qual operação é executada:
#define enable_Instruction_Fetch 0b100101000000100
#define enable_Decode_Register   0b000000000011000

//Máscaras específicas para definir a operação a ser realizada na ULA:
#define split_significant_function_field 0b00001111
#define ALU_OPERATION_ADD                0b00000000
#define ALU_OPERATION_SUB                0b00000010
#define ALU_OPERATION_AND                0b00000100
#define ALU_OPERATION_OR                 0b00000101
#define ALU_OPERATION_SLT                0b00001010

//Máscaras para definir o valor de alu_op:
#define ALU_OP_Add                       0b00000010
#define ALU_OP_Sub                       0b00000110
#define ALU_OP_And                       0b00000000
#define ALU_OP_Or                        0b00000001
#define ALU_OP_Slt                       0b00000111
#define ALU_OP_Err                       0b11111111

//Máscaras para operações dentro da ULA:
#define MAX_SUM_VALUE 0b00000000000000000000000000011111
#define MIN_SUB_VALUE 0b11111111111111111111111111100000
#define ZERO          0b00000000000000000000000000000000

int alu(int a, int b, char alu_op, int *result_alu, char *zero, char *overflow) {
  //Caso a entrada de operaçãos seja inválida:
  if(alu_op == ALU_OP_Err) {
    return 1;
  }

  //Caso a operação seja de soma, o resultado é rs(a) + rt(b):
  if(alu_op == ALU_OP_Add) {
    if(a + b == ZERO) {
      *zero = 1;
      *overflow = 0;
      *result_alu = ZERO;
    }
    else {
      *zero = 0;
      if(a + b > MAX_SUM_VALUE) {
        //Conferir:
        *overflow = 1;
        *result_alu = (a + b) - MAX_SUM_VALUE;
      }
      else {
        *overflow = 0;
        *result_alu = a + b;
      }
    }
  }
  //Caso a operação seja de subtração, o resultado é rs(a) - rt(b):
  else if(alu_op == ALU_OP_Sub) {
    if(a - b == ZERO) {
      *zero = 1;
      *overflow = 0;
      *result_alu = ZERO;
    }
    else {
      *zero = 0;
      if(a - b < MIN_SUB_VALUE) {
        //Conferir:
        *overflow = 1;
        *result_alu = (a - b) - MIN_SUB_VALUE;
      }
      else {
        *overflow = 0;
        *result_alu = a - b;
      }
    }
  }
  //Caso a operação desejada seja um and:
  else if(alu_op == ALU_OP_And) {
    *overflow = 0;
    if(a == ZERO || b == ZERO) {
      *zero = 1;
      *result_alu = ZERO;
    }
    else {
      *zero = 0;
      *result_alu = a & b;
    }
  }
  //Caso a operação desejada seja um or:
  else if(alu_op == ALU_OP_Or) {
    *overflow = 0;
    if(a == ZERO && b == ZERO) {
      *zero = 1;
      *result_alu = ZERO;
    }
    else {
      *zero = 0;
      *result_alu = a | b;
    }
  }
  //Caso a operação desejada seja Set on Less Than, rd(result_alu) = 1 se rs(a) < rt(b). No mais, rd = 0:
  else {
    *overflow = 0;
    if(a < b) {
      *zero = 0;
      *result_alu = 1;
    }
    else {
      *zero = 1;
      *result_alu = 0;
    }
  }
  return 0;
}


void control_unit(int IR, short int *sc) {
  //Checando se o registrador de instrução está vazio:
  if(IR == -1) {
    *sc = enable_Instruction_Fetch;
    return;
  }

  //Se o sinal de controle era de encontrar uma instrução no ciclo anterior, realizar a decodificação:
  if(*sc == enable_Instruction_Fetch) {
    *sc = enable_Decode_Register;
    return;
  }

  //Se a instrução já foi decodificada, descobrir o tipo:
  if(*sc == enable_Decode_Register) {
    short int opcode = IR & split_cop;
    if(opcode == R_Type_Op) {
      //R-Type: add, sub, and, or, slt
      *sc = enable_ALUSrcA | enable_ALUOp1;
      return;
    }
    if(opcode == Beq_Op) {
      //beq
      *sc = enable_ALUSrcA | enable_ALUOp0;
      *sc = *sc | enable_PCSource0;
      *sc = *sc | enable_PCWriteCond;
      return;
    }
    if(opcode == Jump_Op) {
      //Jump
      *sc = enable_PCWriteCond | enable_PCSource1;
      return;
    }
    if(opcode == LW_Op || opcode == SW_Op) {
      //lw ou sw
      *sc = enable_ALUSrcA | enable_ALUSrcB1;
      return;
    }
  }

  not_implemented();
}


void instruction_fetch(short int sc, int PC, int ALUOUT, int IR, int* PCnew, int* IRnew, int* MDRnew) {
  if(sc == enable_Instruction_Fetch) {
    *IRNew = memory[PC];
    *PCNew = PC++;
    return;
  }

  not_implemented();
}


void decode_register(short int sc, int IR, int PC, int A, int B, int *Anew, int *Bnew, int *ALUOUTnew) {
  if(sc == enable_Decode_Register) {
    *Anew = split_rs & IR;
    *Bnew = split_rt & IR;
    *ALUOUTnew = (split_immediate & IR) << 2;
    return;
  }

  not_implemented();
}


void exec_calc_end_branch(short int sc, int A, int B, int IR, int PC, int ALUOUT, int *ALUOUTnew, int *PCnew) {
  //Se a instrução já tem o tipo definido, definir a operação:
  if(sc == enable_ALUSrcA | enable_ALUOp1) {
    //R-Type:
    char alu_op, overflow, zero;

    char operation = IR;
    operation = operation & split_significant_function_field;
    /* Valores de campo de função para cada operação:
       0x20: add;
       0x22: sub;
       0x24: and;
       0x25: or;
       0x2a: slt.
    */
    if(operation == ALU_OPERATION_ADD) {
      alu_op = ALU_OP_Add;
    }
    else if (operation == ALU_OPERATION_SUB) {
      alu_op = ALU_OP_Sub;
    }
    else if (operation == ALU_OPERATION_AND) {
      alu_op = ALU_OP_And;
    }
    else if (operation == ALU_OPERATION_OR) {
      alu_op = ALU_OP_Or;
    }
    else if (operation == ALU_OPERATION_SLT) {
      alu_op = ALU_OP_Slt;
    }
    else {
      alu_op = ALU_OP_Err;
    }

    alu(A, B, alu_op, ALUOUTnew, &zero, &overflow);
    return;
  }


  not_implemented();
}


void write_r_access_memory(short int sc, int IR, int MDR, int AMUOUT, int PC, int *MDRnew, int *IRnew) {
  not_implemented();
}


void write_ref_mem(short int sc, int IR, int MDR, int ALUOUT) {
  not_implemented();
}


/* This is your simulator main function. Normally, it should just call
   the simulator's start function, which will execute the simulation loop.
   You may make additions to the main function if you need.*/

int main (int argc, char **argv) {

  mmcpu_start ();

  return 0;
}
