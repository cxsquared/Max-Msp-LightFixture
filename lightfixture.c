//
//  lightfixture.c
//  lightfixture
//
//  Created by Cody Claborn on 4/29/15.
//
//

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

typedef struct _lightfixture
{
    t_object ob;			// the object itself (must be first)
} t_lightfixture;

///////////////////////// function prototypes
//// standard set
void *lightfixture_new(t_symbol *s, long argc, t_atom *argv);
void lightfixture_free(t_lightfixture *x);
void lightfixture_assist(t_lightfixture *x, void *b, long m, long a, char *s);

//////////////////////// global class pointer variable
void *lightfixture_class;

int C74_EXPORT main(void)
{
    // object initialization
    t_class *c;
    
    c = class_new("lightfixture", (method)lightfixture_new, (method)lightfixture_free, (long)sizeof(t_lightfixture), 0L /* leave NULL!! */, A_GIMME, 0);
    
    /* you CAN'T call this from the patcher */
    class_addmethod(c, (method)lightfixture_assist,	"assist", A_CANT, 0);
    
    class_register(CLASS_BOX, c); /* CLASS_NOBOX */
    lightfixture_class = c;
    
    post("Hopefully a lightfixture object");
    return 0;
}

void lightfixture_assist(t_lightfixture *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { // inlet
        sprintf(s, "I am inlet %ld", a);
    }
    else {	// outlet
        sprintf(s, "I am outlet %ld", a);
    }
}

void lightfixture_free(t_lightfixture *x)
{
    ;
}

/*
 A_GIMME signature =
	t_symbol	*s		objectname
	long		argc	num additonal args
	t_atom		*argv	array of t_atom structs
 type = argv->a_type
 if (type == A_LONG) ;
 else if (type == A_FLOAT) ;
 else if (type == A_SYM) ;
 */
/*
	t_symbol {
 char *s_name;
 t_object *s_thing;
	}
 */
void *lightfixture_new(t_symbol *s, long argc, t_atom *argv)
{
    t_lightfixture *x = NULL;
    long i;
    
    // object instantiation
    if (x = (t_lightfixture *)object_alloc(lightfixture_class)) {
        object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        
        for (i = 0; i < argc; i++) {
            if ((argv + i)->a_type == A_LONG) {
                object_post((t_object *)x, "arg %ld: long (%ld)", i, atom_getlong(argv+i));
            } else if ((argv + i)->a_type == A_FLOAT) {
                object_post((t_object *)x, "arg %ld: float (%f)", i, atom_getfloat(argv+i));
            } else if ((argv + i)->a_type == A_SYM) {
                object_post((t_object *)x, "arg %ld: symbol (%s)", i, atom_getsym(argv+i)->s_name);
            } else {
                object_error((t_object *)x, "forbidden argument");
            }
        }
    }
    return (x);
}
