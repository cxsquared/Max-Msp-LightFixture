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
    long outputSize;
    long *output;
    long address;
    long color[4];
    long *data;
    long dataSize;
} t_lightfixture;

///////////////////////// function prototypes
//// standard set
void *lightfixture_new(t_symbol *s, long argc, t_atom *argv);
void lightfixture_free(t_lightfixture *x);
void lightfixture_assist(t_lightfixture *x, void *b, long m, long a, char *s);

void lightfixture_bang(t_lightfixture *x);
void lightfixture_clear(t_lightfixture *x);
void lightfixture_size(t_lightfixture *x, long newSize);
void lightfixture_address(t_lightfixture *x, long newAddress);
void lightfixture_color(t_lightfixture *x, t_symbol *s, long argc, t_atom *argv);
void lightfixture_list(t_lightfixture *x, t_symbol *s, long argc, t_atom *argv);
long sanatizeLong(long input);

//////////////////////// global class pointer variable
void *lightfixture_class;

int C74_EXPORT main(void)
{
    // object initialization
    t_class *c;
    
    c = class_new("lightfixture", (method)lightfixture_new, (method)lightfixture_free, (long)sizeof(t_lightfixture), 0L /* leave NULL!! */, A_GIMME, 0);
    
    class_addmethod(c, (method)lightfixture_bang, "bang", 0);
    class_addmethod(c, (method)lightfixture_clear, "clear", 0);
    class_addmethod(c, (method)lightfixture_size, "size", A_LONG, 0);
    class_addmethod(c, (method)lightfixture_address, "address", A_LONG, 0);
    class_addmethod(c, (method)lightfixture_color, "color", A_GIMME, 0);
    class_addmethod(c, (method)lightfixture_list, "list", A_GIMME, 0);
    
    /* you CAN'T call this from the patcher */
    class_addmethod(c, (method)lightfixture_assist,	"assist", A_CANT, 0);
    
    class_register(CLASS_BOX, c); /* CLASS_NOBOX */
    lightfixture_class = c;
    
    //post("Hopefully a lightfixture object");
    return 0;
}

void lightfixture_assist(t_lightfixture *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { // inlet
        if (a == 0){
            sprintf(s, "Changing the dmx variables.");
        } else {
            sprintf(s, "Full DMX message in");
        }

    }
    else {	// outlet
        if (a == 0) {
            sprintf(s, "Full dmx message out.");
        } else {
            sprintf(s, "Light message out.");
        }
    }
}

void lightfixture_free(t_lightfixture *x)
{
    if (x->output != NULL){
        free(x->output);
    }

    if (x->data != NULL) {
        free(x->data);
    }
}

void *lightfixture_new(t_symbol *s, long argc, t_atom *argv)
{
    t_lightfixture *x = NULL;
    long i;
    
    // object instantiation
    if (x = (t_lightfixture *)object_alloc(lightfixture_class)) {
        if (argc != 2) {
            //post("Requires two inputs [adress, size]");
            return NULL;
        }
        
        //object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
        //object_post((t_object *)x, "it has %ld arguments", argc);
        
        // Checking for arguments
        for (i = 0; i < argc; i++) {
            if ((argv + i)->a_type == A_LONG) {
                if (i == 0){
                    x->address = atom_getlong(argv+i);
                } else if (i == 1){
                    x->outputSize = atom_getlong(argv+i);
                    x->dataSize = atom_getlong(argv+i);
                }
                //object_post((t_object *)x, "arg %ld: long (%ld)", i, atom_getlong(argv+i));
            } else {
                return NULL;
            }
        }
        
        x->output = (long*)malloc(sizeof(long) * x->outputSize);
        x->data = (long*)malloc(sizeof(long) * x->dataSize);
        
        int index;
        //post("output size %ld",x->outputSize);
        for (index=0; index < x->outputSize; index++) {
            x->output[index] = 0;
            x->data[index] = 0;
            //post("Index = %d", index);
        }
        
        x->outlet2 = outlet_new((t_object *) x, NULL);
        x->outlet1 = outlet_new((t_object *) x, NULL);
    }
    return (x);
}

long sanatizeLong(long input) {
    long output = input;
    if (output > 255) {
        output = 255;
    } else if (output < 0) {
        output = 0;
    }
    return output;
}

void lightfixture_clear(t_lightfixture *x) {
    int i;
    for (i=0; i < x->outputSize; i++) {
        x->output[i] = 0;
    }
}

void lightfixture_bang(t_lightfixture *x) {
    //post("Banged!");
    // Figuring out full output size
    if (x->outputSize + x->address > x->dataSize) {
        long oldSize = x->dataSize;
        x->dataSize = x->outputSize + x->address;
        long *temp = realloc(x->data, sizeof(long) * x->dataSize);
        x->data = temp;
        short i;
        for (i = oldSize; i < x->dataSize; i++) {
            x->data[i] = 0;
        }
        //post("making output data bigger");
    }
    
    Atom fullout[x->dataSize];
    Atom smallout[x->outputSize];
    int i;
    for (i=0; i < x->dataSize; i++) {
        if (i >= x->address && i < x->address+x->outputSize){
            atom_setlong(fullout+i,x->output[i - x->address]);
            atom_setlong(smallout+i-x->address, x->output[i - x->address]);
        } else {
            atom_setlong(fullout + i,x->data[i]);
        }
    }
    outlet_list(x->outlet1,0L, x->dataSize, &fullout);
    outlet_list(x->outlet2,0L, x->outputSize, &smallout);
}

void lightfixture_size(t_lightfixture *x, long newSize) {
    long diff = newSize - x->outputSize;
    x->outputSize = newSize;
    long *temp = realloc(x->output, sizeof(long) * x->outputSize);
    x->output = temp;
    
    while (diff > 0){ // I tried a for loop but just now realized that you have to initialize the i variable.
        // object_post((t_object *) x, "new size - diff: %ld", x->size - diff);
        x->output[x->outputSize-diff] = 0;
        diff--;
    }
    lightfixture_bang(x);
}

void lightfixture_address(t_lightfixture *x, long newAddress) {
    x->address = newAddress;
    lightfixture_bang(x);
}

void lightfixture_color(t_lightfixture *x, t_symbol *s, long argc, t_atom *argv) {
    long i;
    t_atom *ap;
    
    //post("message selector is %s",s->s_name);
    //post("there are %ld arguments",argc);
    
    if (argc < 4) {
        //post("Color takes 4 ints (address, r, g, b)");
        return;
    }
    
    // increment ap each time to get to the next atom
    for (i = 0, ap = argv; i < argc; i++, ap++) {
        switch (atom_gettype(ap)) {
            case A_LONG:
                //post("%ld: %ld",i+1,atom_getlong(ap));
                if (i == 0) {
                    if (atom_getlong(ap) + 3 > x->outputSize) {
                        lightfixture_size(x, atom_getlong(ap) + 3);
                    }
                    // Clear old values
                    x->output[x->color[0]] = 0;
                    x->output[x->color[0] + 1] = 0;
                    x->output[x->color[0] + 2] = 0;
                } else {
                    x->output[x->color[0]+i-1] = sanatizeLong(atom_getlong(ap));
                }
                if (i == 0) {
                    x->color[i] = atom_getlong(ap);
                } else {
                    x->color[i] = sanatizeLong(atom_getlong(ap));
                }
                break;
            default:
                //post("%ld: unknown atom type (%ld)", i+1, atom_gettype(ap));
                break;
        }
    }
    
    lightfixture_bang(x);
}

void lightfixture_list(t_lightfixture *x, t_symbol *s, long argc, t_atom *argv) {
    //post("List in ");
    if (x->dataSize < argc){
        //post("Making data bigger");
        x->dataSize = argc;
        int *temp = realloc(x->data, sizeof(long) * x->dataSize);
        x->data = temp;
    }
    
    short i;
    for (i = 0; i < argc; i++) {
        //post("Data position %ld is set too %ld", i, atom_getlong(argv+i));
        x->data[i] = sanatizeLong(atom_getlong(argv+i));
    }
    lightfixture_bang(x);
}
