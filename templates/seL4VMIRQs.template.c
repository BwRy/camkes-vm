/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

#include <assert.h>
#include <camkes/error.h>
#include <stdio.h>
#include <stdint.h>
#include <sel4/sel4.h>

/*? macros.show_includes(me.from_instance.type.includes) ?*/

/*- set config_irqs = configuration[me.to_instance.name].get(me.to_interface.name) -*/
/*- set irqs = [] -*/
/*- set irqaep_object = alloc_obj('irq_aep_obj', seL4_NotificationObject) -*/
/*- set irqaep_object_cap = alloc_cap('irq_aep_obj', irqaep_object, read=True) -*/
/*- if config_irqs is not none -*/
    /*- for irq in config_irqs -*/
        /*- set cap = alloc('irq_%d' % irq['source'], seL4_IRQControl, number=irq['source'], notification=my_cnode[irqaep_object_cap]) -*/
        /*- do irqs.append( (irq['name'], irq['source'], irq['level_trig'], irq['active_low'], irq['dest'], cap) ) -*/
    /*- endfor -*/
/*- endif -*/

int /*? me.from_interface.name ?*/_num_irqs() {
    return /*? len(irqs) ?*/;
}

const char */*? me.from_interface.name ?*/_get_irq(int irq, seL4_CPtr *irq_handler, uint8_t *source, int *level_trig, int *active_low, uint8_t *dest) {
    /*- if len(irqs) == 0 -*/
        return NULL;
    /*- else -*/
        switch (irq) {
            /*- for name, source, level_trig, active_low, dest, cap in irqs -*/
                case /*? loop.index0 ?*/:
                    *irq_handler = /*? cap ?*/;
                    *source = /*? source ?*/;
                    *level_trig = /*? level_trig ?*/;
                    *active_low = /*? active_low ?*/;
                    *dest = /*? dest ?*/;
                    return "/*? name ?*/";
            /*- endfor -*/
        default:
            return NULL;
        }
    /*- endif -*/
}
