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

/*#include <mmcpu/cpu.h>
#include <mmcpu/mask.h>
*/

#include "mask.h"
#include "cpu.h"

/* Students, you are required to implemented the functions bellow.
   Please, refere to cpu.h for further information. */

//Constantes para controle do tipo de instrução:
#define R_Type_Op 0b00000000000000000000000000000000
#define Beq_Op    0b00010000000000000000000000000000
#define Jump_Op   0b00001000000000000000000000000000
#define LW_Op     0b10001100000000000000000000000000
#define SW_Op     0b10101100000000000000000000000000
//Quando um opcode inválido é recebido, sc recebe:
#define ERROR_OP  0b00000000000000000000111111111111

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

/* TODOLIST:
1- Checar as posições de acesso à memória e aos registradores;
2- Criar máscaras de erros para 1;
3- Checar saída de erro da ULA;
4- Descobrir para que serve o overflow da ULA;
5- Descobrir para que serve o IRnew no acesso à memória;
6- Em todos os splits antes de acessos à posição em vetores, testar a inclusão de shifts;
7- Criar e utilizar saídas de erro;
8- Descobrir como printar um valor em binário.
*/

int alu(int a, int b, char alu_op, int *result_alu, char *zero, char *overflow) {
  //Caso a entrada de operaçãos seja inválida:
  if(alu_op == ALU_OP_Err) {
    printf("[ALU] Operação inválida!\n");
    return 1;
  }

  //Caso a operação seja de soma, o resultado é rs(a) + rt(b):
  if(alu_op == ALU_OP_Add) {
    printf("[ALU] Operação: soma\n");
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
    printf("[ALU] Operação: subtração\n");
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
    printf("[ALU] Operação: AND\n");
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
    printf("[ALU] Operação: OR\n");
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
    printf("[ALU] Operação: SLT\n");
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
//ULA ok por hora

void control_unit(int IR, short int *sc) {
  //Checando se o registrador de instrução está vazio:
  if(IR == -1) {
    printf("[CU] IR vazio\n");
    *sc = enable_Instruction_Fetch;
    return;
  }

  //Se o sinal de controle já corresponde ao de um jump, a operação foi finalizada no ciclo anterior:
  if(*sc == (enable_PCWriteCond | enable_PCSource1)) {
    printf("[CU] Jump finalizada\n");
    *sc = enable_Instruction_Fetch;
    return;
  }

  //Se o sinal de controle já corresponde ao de uma Beq, a operação foi finalizada no ciclo anterior:
  if(*sc == ((enable_ALUSrcA | enable_ALUOp0) | (enable_PCSource0 | enable_PCWriteCond))) {
    printf("[CU] Beq finalizada\n");
    *sc = enable_Instruction_Fetch;
    return;
  }

  //Se o sinal de controle corresponde ao de acesso à memória por uma R-Type ou por uma SW, a operação foi finalizada no ciclo anterior:
  if((*sc == (enable_IorD | enable_MemWrite)) || (*sc == (enable_RegDst | enable_RegWrite))) {
    printf("[CU] Tipo-R ou SW finalizada\n");
    *sc = enable_Instruction_Fetch;
    return;
  }

  //Se o sinal de controle corresponde ao de uma escrita vinda da memória, a operação de LW foi finalizada no ciclo anterior:
  if(*sc == (enable_RegWrite | enable_MemtoReg)) {
    printf("[CU] SW finalizada\n");
    *sc = enable_Instruction_Fetch;
    return;
  }

  //Se o sinal de controle corresponde ao de acesso à memória por uma LW, ativar a escrita vinda da memória no ciclo atual:
  if(*sc == (enable_IorD | enable_MemRead)) {
    printf("[CU] LW: ativar escrita à memória\n");
    *sc = enable_RegWrite | enable_MemtoReg;
    return;
  }

  //Se o sinal de controle já corresponde ao de uma LW ou de uma SW, ativar o acesso à memória no ciclo atual:
  if(*sc == (enable_ALUSrcA | enable_ALUSrcB1)) {
    //LW op: 0x23 | SW op: 0x2b
    int opcode = IR & split_cop;
    if(opcode == LW_Op) {
      printf("[CU] LW: ativar acesso à memória\n");
      *sc = enable_IorD | enable_MemRead;
    }
    else if (opcode == SW_Op) {
      printf("[CU] SW: ativar acesso à memória\n");
      *sc = enable_IorD | enable_MemWrite;
    }
    else {
      printf("[CU] LW ou SW: opcode inválido!\n");
      *sc = ERROR_OP;
    }
    return;
  }

  //Se o sinal de controle já corresponde ao de uma R-Type, ativar o acesso à memória no ciclo atual:
  if(*sc == (enable_ALUSrcA | enable_ALUOp1)) {
    printf("[CU] RT: ativar acesso à memória\n");
    *sc = enable_RegDst | enable_RegWrite;
    return;
  }

  //Se o sinal de controle era de encontrar uma instrução no ciclo anterior, realizar a decodificação:
  if(*sc == enable_Instruction_Fetch) {
    printf("[CU] Ativar decodificação da instrução\n");
    *sc = enable_Decode_Register;
    return;
  }

  //Se a instrução já foi decodificada, descobrir o tipo:
  if(*sc == enable_Decode_Register) {
    int opcode = IR & split_cop;
    if(opcode == R_Type_Op) {
      //R-Type: add, sub, and, or, slt
      printf("[CU] Instrução do tipo R\n");
      *sc = enable_ALUSrcA | enable_ALUOp1;
      return;
    }
    if(opcode == Beq_Op) {
      //beq
      printf("[CU] Instrução Beq\n");
      *sc = enable_ALUSrcA | enable_ALUOp0;
      *sc = *sc | enable_PCSource0;
      *sc = *sc | enable_PCWriteCond;
      return;
    }
    if(opcode == Jump_Op) {
      //Jump
      printf("[CU] Instrução Jump\n");
      *sc = enable_PCWriteCond | enable_PCSource1;
      return;
    }
    if(opcode == LW_Op || opcode == SW_Op) {
      //lw ou sw
      printf("[CU] Instrução LW ou SW\n");
      *sc = enable_ALUSrcA | enable_ALUSrcB1;
      return;
    }
  }
}
//Ok

void instruction_fetch(short int sc, int PC, int ALUOUT, int IR, int* PCnew, int* IRnew, int* MDRnew) {
  if(sc == enable_Instruction_Fetch && PC < MAX) {
    *IRnew = memory[PC];
    printf("[IF] Nova palavra: %d\n", *IRnew);
    if(*IRnew == 0) {
      //loop = 0;
      return;
    }
    *PCnew = PC++; //PC+1 por estarmos trabalhando com vetores na main
  }
  else {
    //loop = 0;
  }
  return;
}
//IF ok

void decode_register(short int sc, int IR, int PC, int A, int B, int *Anew, int *Bnew, int *ALUOUTnew) {
  if(sc == enable_Decode_Register) {
    *Anew = reg[(split_rs & IR) >> 21];
    *Bnew = reg[(split_rt & IR) >> 16];
    *ALUOUTnew = (split_immediate & IR) << 2; //Conferir se é equivalente a ALUOut = PC + ext(IR[15-0] << 2)
    printf("[DR] A: %d | B: %d | ALUOut: %d\n", *Anew, *Bnew, *ALUOUTnew);
    return;
  }
}
//DR ok

void exec_calc_end_branch(short int sc, int A, int B, int IR, int PC, int ALUOUT, int *ALUOUTnew, int *PCnew) {
  //Se a instrução já tem o tipo definido, definir a operação:

  //R-Type:
  if(sc == (enable_ALUSrcA | enable_ALUOp1)) {
    printf("[EX] Instrução do tipo R: ");
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
      printf("soma\n");
      alu_op = ALU_OP_Add;
    }
    else if (operation == ALU_OPERATION_SUB) {
      printf("subtração\n");
      alu_op = ALU_OP_Sub;
    }
    else if (operation == ALU_OPERATION_AND) {
      printf("AND\n");
      alu_op = ALU_OP_And;
    }
    else if (operation == ALU_OPERATION_OR) {
      printf("OR\n");
      alu_op = ALU_OP_Or;
    }
    else if (operation == ALU_OPERATION_SLT) {
      printf("SLT\n");
      alu_op = ALU_OP_Slt;
    }
    else {
      printf("ERRO!\n");
      alu_op = ALU_OP_Err;
    }

    alu(A, B, alu_op, ALUOUTnew, &zero, &overflow);
    //Não sei o que fazer com o zero e com o oveflow...
    return;
  }

  //Jump:
  if(sc == (enable_PCWriteCond | enable_PCSource1)) {
    printf("[EX] Instrução Jump\n");
    *PCnew = (PC & split_cop) | ((IR & split_jump_address) << 2); //Conferir precedência
    return;
  }

  //Beq:
  if(sc == ((enable_ALUSrcA | enable_ALUOp0) | (enable_PCSource0 | enable_PCWriteCond))) {
    printf("[EX] Instrução Beq\n");
    //Primeiro, subtrair A e B na ULA:
    char overflow, zero;
    alu(A, B, ALU_OPERATION_SUB, ALUOUTnew, &zero, &overflow);
    if(zero == 1) {
      //Caso A-B=0 => A=B, computar PC = PC + 4 + (offset << 2):
      alu(PC, (IR & split_immediate) << 2, ALU_OP_Add, ALUOUTnew, &zero, &overflow);
      *PCnew = *ALUOUTnew;
      //Checar se as operações são feitas separadamente dentro da ULA ou se podem ser feitas em duas chamadas.
    }
    return;
  }

  //LW e SW:
  if(sc == (enable_ALUSrcA | enable_ALUSrcB1)) {
    printf("[EX] Instrução LW ou SW\n");
    //ALUOut = A + ext(IR[15-0]);
    char overflow, zero;
    alu(A, IR & split_immediate, ALU_OP_Add, ALUOUTnew, &zero, &overflow);
  }
}
//Exec escrita conforme slide

void write_r_access_memory(short int sc, int IR, int MDR, int AMUOUT, int PC, int *MDRnew, int *IRnew) {
  //*IRnew ????

  //Se o sinal de controle indica uma LW:
  if(sc == (enable_IorD | enable_MemRead)) {
    //MDR = Mem[ALUOut]
    printf("[LW] MDR = Mem[%d]\n", AMUOUT);
    *MDRnew =  memory[AMUOUT];
    return;
  }

  //Se o sinal de controle indica uma SW:
  if(sc == (enable_IorD | enable_MemWrite)) {
    //Mem[ALUOut] = B (IR)
    printf("[SW] Mem[%d] = %d\n", AMUOUT, IR);
    memory[AMUOUT] = IR;
    return;
  }

  //Se o sinal de controle indica uma R-type:
  if(sc == (enable_RegDst | enable_RegWrite)) {
    //Reg[IR[15-11]] = ALUOut
    printf("[SW] Reg[IR[%d]] = %d\n", (IR & split_rd) >> 11, AMUOUT);
    reg[(IR & split_rd) >> 11] = AMUOUT;
    return;
  }
}
//MEM feita a menos dos itens listados

void write_ref_mem(short int sc, int IR, int MDR, int ALUOUT) {
  if(sc == (enable_RegWrite | enable_MemtoReg)) {
    //Reg[IR[20-16] = MDR
    printf("[WB] Reg[%d] = %d\n", (split_rt & IR) >> 16, MDR);
    reg[(split_rt & IR) >> 16] = MDR;
    return;
  }
}
//WB ok

/* This is your simulator main function. Normally, it should just call
   the simulator's start function, which will execute the simulation loop.
   You may make additions to the main function if you need.*/

int main (int argc, char **argv) {

  mmcpu_start ();

  return 0;
}
