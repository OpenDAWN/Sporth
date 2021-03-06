#include "plumber.h"

int sporth_p(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    int n;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "p: creating\n");
#endif

            plumber_add_module(pd, SPORTH_P, sizeof(SPFLOAT), NULL);
            break;
        case PLUMBER_INIT:
        case PLUMBER_COMPUTE:
            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for P\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            n = (int)sporth_stack_pop_float(stack);
            if(n < 16)
                sporth_stack_push_float(stack, pd->p[n]);
            else
                sporth_stack_push_float(stack, 0);

            break;
        case PLUMBER_DESTROY:
            break;
        default:
            fprintf(stderr, "switch: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int sporth_pset(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    int n;
    SPFLOAT val;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "p: creating\n");
#endif

            plumber_add_module(pd, SPORTH_PSET, sizeof(SPFLOAT), NULL);
            break;
        case PLUMBER_INIT:
        case PLUMBER_COMPUTE:
            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for pset \n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            n = (int)sporth_stack_pop_float(stack);
            val = sporth_stack_pop_float(stack);
            if(n < 16) pd->p[n] = val;

            break;
        case PLUMBER_DESTROY:
            break;
        default:
            fprintf(stderr, "switch: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
