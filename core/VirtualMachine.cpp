//
// Created by Peter on 2020/12/8.
//

#include "VirtualMachine.h"

void VirtualMachine::interpret()
{

    const static int ret_addr = 0, dynamic_link = 1, static_link = 2;
    PCode ir;
    int ip = 1, sp = 0, bp = 0;
    int stack[1000] = { 0 };
    int sp_stack[100];  // Old SP
    int sp_top = 0;
    while (ip<pc)
    {
        ir = code[ip++];
        switch (ir.op)
        {
            case LIT:
                stack[sp++] = ir.a;
                break;
            case LOD:
            {
                int outer_bp = bp;
                while (ir.l--)
                    outer_bp = stack[outer_bp + static_link];
                stack[sp++] = stack[outer_bp + ir.a];
                break;
            }
            case STO:
            {
                int outer_bp = bp;  // previous
                while (ir.l--)
                    outer_bp = stack[outer_bp + static_link];
                stack[outer_bp + ir.a] = stack[sp - 1];
                break;
            }
            case CAL:
            {
                stack[sp + ret_addr] = ip;
                stack[sp + dynamic_link] = bp;
                int outer_bp = bp;
                while (ir.l--)
                    outer_bp = stack[outer_bp + static_link];
                stack[sp + static_link] = outer_bp;
                ip = ir.a;
                bp = sp;
                break;
            }
            case INT:
            {
                sp_stack[sp_top++] = sp;
                sp += ir.a;
                break;
            }
            case JMP:
            {
                ip = ir.a;
                break;
            }
            case JPC:
            {
                if (stack[sp - 1] == 0)
                    ip = ir.a;
                break;
            }
            case OPR:
            {
                switch (ir.a)
                {
                    case OP_RET:
                    {
                        ip = stack[bp + ret_addr];
                        bp = stack[bp + dynamic_link];
                        sp = sp_stack[--sp_top];
                        if (sp_top <= 0)
                        {
                            printf("program exited normally!\n");
                            return;
                        }
                        break;
                    }
                    case OP_NEG:
                    {
                        stack[sp - 1] = !stack[sp - 1];
                        break;
                    }
                    case OP_PLUS:
                    {
                        stack[sp - 2] = stack[sp - 2] + stack[sp - 1];
                        sp--;
                        break;
                    }
                    case OP_MINUS:
                    {
                        stack[sp - 2] = stack[sp - 2] - stack[sp - 1];
                        sp--;
                        break;
                    }
                    case OP_TIMES:
                    {
                        stack[sp - 2] = stack[sp - 2] * stack[sp - 1];
                        sp--;
                        break;
                    }
                    case OP_SLASH:
                    {
                        stack[sp - 2] = stack[sp - 2] / stack[sp - 1];
                        sp--;
                        break;
                    }
                    case OP_ODD:
                    {
                        stack[sp - 1] = (stack[sp - 1] % 2 == 1) ? 1 : 0;
                        break;
                    }
                    case OP_EQU:
                    {
                        stack[sp - 2] = (stack[sp - 2] == stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_NEQ:
                    {
                        stack[sp - 2] = (stack[sp - 2] != stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_LES:
                    {
                        stack[sp - 2] = (stack[sp - 2] < stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_GEQ:
                    {
                        stack[sp - 2] = (stack[sp - 2] >= stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_GTR:
                    {
                        stack[sp - 2] = (stack[sp - 2] > stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_LEQ:
                    {
                        stack[sp - 2] = (stack[sp - 2] <= stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_WRITE:
                    {
                        cout << stack[sp - 1];
                        break;
                    }
                    case OP_LN:
                    {
                        cout << endl;
                        break;
                    }
                    case OP_READ:
                    {
                        cout << "Please input a number:" << endl;
                        cin >> stack[sp++];
                        break;
                    }
                    default:
                    {
                        printf("Unexpected Operation!\n");
                        return;
                    }
                }
                break;
            }
            default:
                printf("Unexpected Instruction!\n"); return;
        }
    }
}
