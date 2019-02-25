#include "kexpr.h"
#include "property.h"

int ke_sml(sml_t *sml, kexpr_t *kexpr, int64_t *_i, double *_r, char **_p, int *ret_type)
{
	token_t *p, *q, *e;
	int err = 0;
	*_i = 0, *_r = 0., *ret_type = 0;
	sml->kexpr = kexpr;
	sml->stack = (token_t**)ke_malloc_memory(sml, kexpr->n * sizeof(token_t *));
	token_t **stack = sml->stack;
	struct token_s ** fields = sml->fields;
	struct token_s ** tokens = sml->tokens;

	token_t **tokpp = NULL;
	token_t *tokp = NULL;
	register int top = 0;

	int n = kexpr->n;
	for (int itok = 0; itok < n; ++itok) {
		sml->top = top;
		// put into the stack all values type
		tokpp = &tokens[itok];
		while ((*tokpp)->ttype > 5) {
			stack[top++] = *tokpp++;
			++itok;
		}
		tokp = *tokpp;
		sml->tok_idx = itok;
		switch (tokp->ttype) {
		case KET_CMD:
			top = (tokp->f.defcmd)(sml, tokp, top, &itok);
			break;
		case KET_VCMD:
			itok = (tokp->f.defvcmd)(sml, tokp, top, itok);
			break;
		case KET_OP:
			if (tokp->op == KEO_NOP) continue;
			if (tokp->op == KEO_LET) {
				if (!stack[top - 2]->propset) {
					q = stack[--top];
					p = stack[--top];
					if (q->vtype == KEV_INT)  p->i = q->i, p->r = (double)p->i;
					else if (q->vtype == KEV_REAL) p->r = q->r, p->i = (int64_t)p->r;
					else { ke_set_val(sml, p, q); }
				}
				else {
					e = stack[top - 2];
					fields[e->ifield]->n_args = e->n_args;
					stack[top - 2] = fields[e->ifield];
					sml->top = top;	sml->tokp = e;
					ke_poperty_set(sml);
					top = sml->top;
				}
			}
			else {
				if (tokp->n_args == 2) {
					q = stack[--top];
					p = stack[--top];
					stack[top] = ke_get_out(sml);
					tokp->f.builtin(p, q, stack[top]);
					++top;
				}
				else {
					p = stack[--top];
					stack[top] = ke_get_out(sml);
					tokp->f.builtin(p, 0, stack[top]);
					++top;
				}
			}
			break;
		case KET_FUNC:
			sml->top = top;	sml->tokp = tokp;
			(tokp->f.deffunc)(sml);
			top = sml->top;
			break;
		case KET_PROP:
			if (tokp->propget) {
				// we need ifield, because for a propget, the tokp is not a pointer to a real fields
				// it's only a normal token with ifield pointing to the real field to manager
				// it's a false record to deal with propget. 
				stack[top++] = fields[tokp->ifield];
				sml->top = top;	sml->tokp = tokp;
				ke_poperty_get(sml);
				top = sml->top;
			}
			else {
				stack[top++] = tokp;
			}
			break;
		default:
			stack[top++] = tokp;
			break;
		}
	}

#ifdef DEBUG
	ke_print_stack(sml, tokp, top);
#endif // DEBUG

	*ret_type = stack[0]->vtype;
	*_i = stack[0]->i, *_r = stack[0]->r, *_p = stack[0]->obj.s;

#ifdef DEBUG
	printf("= ");
	ke_print_one(sml, tokp);
	printf("\n");
#endif // DEBUG

	ke_free_memory(sml, sml->stack);
	return err;
}
