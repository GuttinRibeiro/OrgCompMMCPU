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

int alu( int a, int b, char alu_op, int *result_alu, char *zero, char *overflow) {
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
    *PCNew = PC + 4;
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
