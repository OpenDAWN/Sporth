#include <stdlib.h>
#include "string.h"
#include "plumber.h"
#include "poly.h"

typedef struct {
    poly_data poly;
    poly_cluster clust;
    sp_ftbl *ft;
    uint32_t max_params;
    uint32_t max_voices;
    uint32_t *dur;
} sporth_poly_d;

int sporth_poly(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    sporth_poly_d *poly;
    poly_voice *voice;
    poly_event *evt;
    uint32_t nvoices;
    char *ftname;
    char *file;
    uint32_t n, p;
    int id;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "poly: Creating\n");
#endif
            poly = malloc(sizeof(sporth_poly_d));
            plumber_add_module(pd, SPORTH_POLY, sizeof(sporth_poly_d), poly);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "poly: Initialising\n");
#endif

            if(sporth_check_args(stack, "ffss") != SPORTH_OK) {
                fprintf(stderr,"Invalid arguments for poly\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            poly = pd->last->ud;

            ftname = sporth_stack_pop_string(stack);
            file = sporth_stack_pop_string(stack);
            poly->max_params = (uint32_t)sporth_stack_pop_float(stack);
            poly->max_voices = (uint32_t)sporth_stack_pop_float(stack);

            poly->dur = malloc(sizeof(uint32_t) * poly->max_voices);

            poly_init(&poly->poly);
            if(poly_binary_parse(&poly->poly, file, pd->sp->sr) != 0) {
                fprintf(stderr, "Could not read file %s\n", file);
                stack->error++;
                return PLUMBER_NOTOK;
            }
            poly_end(&poly->poly);
            poly_cluster_init(&poly->clust, poly->max_voices);
            sp_ftbl_create(pd->sp, &poly->ft, 
                    1 + poly->max_voices * (2 + poly->max_params));
            memset(poly->ft->tbl, 0, poly->ft->size * sizeof(SPFLOAT));
            poly->ft->tbl[0] = poly->max_params;
            plumber_ftmap_add(pd, ftname, poly->ft);
            sporth_stack_push_float(stack, 0);
            free(ftname);
            free(file);
            break;
        case PLUMBER_COMPUTE:
            sporth_stack_pop_float(stack);
            sporth_stack_pop_float(stack);
            poly = pd->last->ud;

            poly_compute(&poly->poly);

            for(n = 0; n < poly->max_voices; n++) {
                poly->ft->tbl[1 + n * (poly->max_params + 2)] = 0.0;
            }

            poly_itr_reset(&poly->poly);
            for(n = 0; n < poly_nevents(&poly->poly); n++) {
                evt = poly_itr_next(&poly->poly);    
                if(!poly_cluster_add(&poly->clust, &id)) {
                    poly->dur[id] = evt->p[0] * pd->sp->sr;
                    poly->ft->tbl[1 + id * (poly->max_params + 2)] = 1.0;
                    poly->ft->tbl[2 + id * (poly->max_params + 2)] = evt->p[0];
                    for(p = 1; p < evt->nvals; p++) {
                        poly->ft->tbl[2 + id * (poly->max_params + 2) + p] = evt->p[p];
                    }
                }
            }
            poly_cluster_reset(&poly->clust);
            nvoices = poly_cluster_nvoices(&poly->clust);

            for(n = 0; n < nvoices; n++) {
                voice = poly_next_voice(&poly->clust);
                poly->dur[voice->val] -= 1;
            }
           
            poly_cluster_reset(&poly->clust);
            for(n = 0; n < nvoices; n++) {
                voice = poly_next_voice(&poly->clust);
                if(poly->dur[voice->val] <= 0) {
                    poly_cluster_remove(&poly->clust, voice->val);
                }
            }
            break;
        case PLUMBER_DESTROY:
            poly = pd->last->ud;
            poly_cluster_destroy(&poly->clust);
            poly_destroy(&poly->poly);
            free(poly->dur);
            free(poly);
            break;
        default:
            fprintf(stderr, "poly: Uknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int sporth_polyget(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    sporth_poly_d *poly; 
    char *ftname;
    int voice;
    uint32_t param;
    uint32_t max_p;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "polyget: Creating\n");
#endif
            poly = malloc(sizeof(sporth_poly_d));
            plumber_add_module(pd, SPORTH_POLYGET, 0, poly);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "polyget: Initialising\n");
#endif
            poly = pd->last->ud;
            if(sporth_check_args(stack, "ffs") != SPORTH_OK) {
                fprintf(stderr,"Invalid arguments for polyget\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            ftname = sporth_stack_pop_string(stack);
            voice = (int)sporth_stack_pop_float(stack);
            param = (uint32_t)sporth_stack_pop_float(stack);

            if(plumber_ftmap_search(pd, ftname, &poly->ft) == PLUMBER_NOTOK) {
                stack->error++;
                return PLUMBER_NOTOK;
            } 
            free(ftname);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            poly = pd->last->ud;
            param = (uint32_t)sporth_stack_pop_float(stack);
            voice = (uint32_t)sporth_stack_pop_float(stack);

            max_p = (uint32_t)poly->ft->tbl[0] + 2;
            sporth_stack_push_float(stack, poly->ft->tbl[1 + (max_p * voice) + param]);
            break;
        case PLUMBER_DESTROY:
            poly = pd->last->ud;
            free(poly);
            break;
        default:
            fprintf(stderr, "polyget: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

