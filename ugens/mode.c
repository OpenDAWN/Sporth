#include "plumber.h"

int sporth_mode(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT in;
    SPFLOAT out;
    SPFLOAT freq;
    SPFLOAT q;
    sp_mode *mode;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "mode: Creating\n");
#endif

            sp_mode_create(&mode);
            plumber_add_module(pd, SPORTH_MODE, sizeof(sp_mode), mode);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "mode: Initialising\n");
#endif

            if(sporth_check_args(stack, "fff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for mode\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            q = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            mode = pd->last->ud;
            sp_mode_init(pd->sp, mode);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            if(sporth_check_args(stack, "fff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for mode\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            q = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            mode = pd->last->ud;
            mode->freq = freq;
            mode->q = q;
            sp_mode_compute(pd->sp, mode, &in, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            mode = pd->last->ud;
            sp_mode_destroy(&mode);
            break;
        default:
            fprintf(stderr, "mode: Uknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
