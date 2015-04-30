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
    void *outlet1;
    void *outlet2;
    long size;
    long* output;
    long address;
    long color[4];
} t_lightfixture;

///////////////////////// function prototypes
//// standard set
void *lightfixture_new(t_symbol *s, long argc, t_atom *argv);
void lightfixture_free(t_lightfixture *x);
void lightfixture_assist(t_lightfixture *x, void *b, long m, long a, char *s);

void lightfixture_bang(t_lightfixture *x);
void lightfixture_size(t_lightfixture *x, long i);

//////////////////////// global class pointer variable
void *lightfixture_class;

int C74_EXPORT main(void)
{
    // object initialization
    t_class *c;
    
    c = class_new("lightfixture", (method)lightfixture_new, (method)lightfixture_free, (long)sizeof(t_lightfixture), 0L /* leave NULL!! */, A_GIMME, 0);
    
    class_addmethod(c, (method)lightfixture_bang, "bang", 0);
    class_addmethod(c, (method)lightfixture_size, "size", A_LONG, 0);
    
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
    free(x->output);
}

void *lightfixture_new(t_symbol *s, long argc, t_atom *argv)
{
    t_lightfixture *x = NULL;
    long i;
    
    // object instantiation
    if (x = (t_lightfixture *)object_alloc(lightfixture_class)) {
        if (argc != 2) {
            return NULL;
        }
        
        object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        
        // Checking for arguments
        for (i = 0; i < argc; i++) {
            if ((argv + i)->a_type == A_LONG) {
                if (i == 0){
                    x->address = atom_getlong(argv+i);
                } else if (i == 1){
                    x->size = atom_getlong(argv+1);
                }
                object_post((t_object *)x, "arg %ld: long (%ld)", i, atom_getlong(argv+i));
            } else {
                return NULL;
            }
        }
        
        x->output = (long*)malloc(sizeof(long) * x->size);
        for (i=0; i < x->size; i++) {
            x->output[i] = 0;
        }
        x->outlet2 = outlet_new((t_object *) x, NULL);
        x->outlet1 = outlet_new((t_object *) x, NULL);
    }
    return (x);
}

void lightfixture_bang(t_lightfixture *x) {
    t_atom myList[x->size];
    short i;

    for (i=0; i < x->size; i++) {
        //object_post((t_object *) x, "output &ld is &ld", i, x->output[i]);
        atom_setlong(myList+i,x->output[i]);
    }
    outlet_list(x->outlet1,0L,x->size,&myList);
    outlet_int(x->outlet2, x->size);
}

void lightfixture_size(t_lightfixture *x, long newSize) {
    int diff = newSize - x->size;
    x->size = newSize;
    int *temp = realloc(x->output, sizeof(long) * x->size);
    x->output = temp;
    
    while (diff > 0){
        object_post((t_object *) x, "new size - diff: %ld", x->size - diff);
        x->output[x->size-diff] = 0;
        diff--;
    }
    lightfixture_bang(x);
}
