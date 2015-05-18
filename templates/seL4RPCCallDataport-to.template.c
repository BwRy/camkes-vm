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
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <camkes/marshal.h>
#include <camkes/error.h>
#include <camkes/tls.h>
#include <sel4/sel4.h>
#include <camkes/dataport.h>
#include <utils/util.h>

/*? macros.show_includes(me.to_instance.type.includes) ?*/
/*? macros.show_includes(me.to_interface.type.includes, '../static/components/' + me.to_instance.type.name + '/') ?*/

/*- include 'seL4MultiSharedData-to.template.c' -*/

/*- set base = 'get_base()' -*/

static seL4_Word /*? me.to_interface.name ?*/_badge = 0;

seL4_Word /*? me.to_interface.name ?*/_get_badge(void) {
    return /*? me.to_interface.name ?*/_badge;
}

static void *get_base() {
    void *base = /*? me.to_interface.name ?*/_buf(/*? me.to_interface.name ?*/_badge);
    assert(base);
    return base;
}

/*- set methods_len = len(me.to_interface.type.methods) -*/
/*- set instance = me.to_instance.name -*/
/*- set interface = me.to_interface.name -*/
/*- set threads = list(range(1, 2 + len(me.to_instance.type.provides) + len(me.to_instance.type.uses) + len(me.to_instance.type.emits) + len(me.to_instance.type.consumes))) -*/

/* Interface-specific error handling */
/*- set error_handler = '%s_error_handler' % me.to_interface.name -*/
/*- include 'error-handler.c' -*/

/*- for m in me.to_interface.type.methods -*/
    extern
    /*- if m.return_type -*/
        /*- if m.return_type.array -*/
            /*- if isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
                char **
            /*- else -*/
                /*? show(m.return_type) ?*/ *
            /*- endif -*/
        /*- elif isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
            char *
        /*- else -*/
            /*? show(m.return_type) ?*/
        /*- endif -*/
    /*- else -*/
        void
    /*- endif -*/
    /*? me.to_interface.name ?*/_/*? m.name ?*/(
      /*- if m.return_type and m.return_type.array -*/
          /*- set ret_sz = c_symbol('ret_sz') -*/
          size_t * /*? ret_sz ?*/
          /*- if len(m.parameters) > 0 -*/
              ,
          /*- endif -*/
      /*- endif -*/
      /*- for p in m.parameters -*/
        /*- if p.direction == 'in' -*/
          /*- if p.array -*/
            size_t /*? p.name ?*/_sz,
            /*- if isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
              char **
            /*- else -*/
              const /*? show(p.type) ?*/ *
            /*- endif -*/
          /*- elif isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
            const char *
          /*- else -*/
            /*? show(p.type) ?*/
          /*- endif -*/
          /*? p.name ?*/
        /*- else -*/
          /*? assert(p.direction in ['refin', 'out', 'inout']) ?*/
          /*- if p.array -*/
            /*- if p.direction == 'refin' -*/
              const
            /*- endif -*/
            size_t * /*? p.name ?*/_sz,
            /*- if isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
              char ***
            /*- else -*/
              /*? show(p.type) ?*/ **
            /*- endif -*/
          /*- elif isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
            char **
          /*- else -*/
            /*- if p.direction == 'refin' -*/
              const
            /*- endif -*/
            /*? show(p.type) ?*/ *
          /*- endif -*/
          /*? p.name ?*/
        /*- endif -*/
        /*- if not loop.last -*/
          ,
        /*- endif -*/
      /*- endfor -*/
      /*- if (m.return_type is none or not m.return_type.array) and len(m.parameters) == 0 -*/
        void
      /*- endif -*/
    );

/*- set name = m.name -*/
/*- set function = '%s_unmarshal_inputs' % m.name -*/
/*- set buffer = base -*/
/*- set size = 'PAGE_SIZE_4K' -*/
/*- set input_parameters = filter(lambda('x: x.direction in [\'refin\', \'in\', \'inout\']'), m.parameters) -*/
/*- set allow_trailing_data = true -*/
/*- include 'unmarshal-inputs.c' -*/

/*- set function = '%s_marshal_outputs' % m.name -*/
/*- set output_parameters = filter(lambda('x: x.direction in [\'out\', \'inout\']'), m.parameters) -*/
/*- set return_type = m.return_type -*/
/*- include 'marshal-outputs.c' -*/

/*- if m.return_type -*/
  /*- if m.return_type.array -*/
    /*- set name = '%s_ret_sz_to' % m.name -*/
    /*- set type = 'size_t' -*/
    /*- include 'thread_local.c' -*/
    /*- if isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
      /*- set name = '%s_ret_to' % m.name -*/
      /*- set type = 'char**' -*/
      /*- include 'thread_local.c' -*/
    /*- else -*/
      /*- set name = '%s_ret_to' % m.name -*/
      /*- set type = '%s*' % show(m.return_type) -*/
      /*- include 'thread_local.c' -*/
    /*- endif -*/
  /*- elif isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
    /*- set name = '%s_ret_to' % m.name -*/
    /*- set type = 'char*' -*/
    /*- include 'thread_local.c' -*/
  /*- else -*/
    /*- set name = '%s_ret_to' % m.name -*/
    /*- set type = show(m.return_type) -*/
    /*- include 'thread_local.c' -*/
  /*- endif -*/
/*- endif -*/
/*- for p in m.parameters -*/
  /*- if p.array -*/
    /*- set name = '%s_%s_sz_to' % (m.name, p.name) -*/
    /*- set type = 'size_t' -*/
    /*- include 'thread_local.c' -*/
    /*- if isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
      /*- set name = '%s_%s_to' % (m.name, p.name) -*/
      /*- set type = 'char**' -*/
      /*- include 'thread_local.c' -*/
    /*- else -*/
      /*- set name = '%s_%s_to' % (m.name, p.name) -*/
      /*- set type = '%s*' % show(p.type) -*/
      /*- include 'thread_local.c' -*/
    /*- endif -*/
  /*- elif isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
    /*- set name = '%s_%s_to' % (m.name, p.name) -*/
    /*- set type = 'char*' -*/
    /*- include 'thread_local.c' -*/
  /*- else -*/
    /*- set name = '%s_%s_to' % (m.name, p.name) -*/
    /*- set type = show(p.type) -*/
    /*- include 'thread_local.c' -*/
  /*- endif -*/
/*- endfor -*/

/*- endfor -*/

/*- set ep = alloc('ep', seL4_EndpointObject, read=True, write=True) -*/

/*- set call_tls_var = c_symbol('call_tls_var_to') -*/
/*- set array = False -*/
/*- set name = call_tls_var -*/
/*- if methods_len <= 1 -*/
  /*- set type = 'unsigned int' -*/
  /*- include 'thread_local.c' -*/
/*- elif methods_len <= 2 ** 8 -*/
  /*- set type = 'uint8_t' -*/
  /*- include 'thread_local.c' -*/
/*- elif methods_len <= 2 ** 16 -*/
  /*- set type = 'uint16_t' -*/
  /*- include 'thread_local.c' -*/
/*- elif methods_len <= 2 ** 32 -*/
  /*- set type = 'uint32_t' -*/
  /*- include 'thread_local.c' -*/
/*- elif methods_len <= 2 ** 64 -*/
  /*- set type = 'uint64_t' -*/
  /*- include 'thread_local.c' -*/
/*- else -*/
  /*? raise(Exception('too many methods in interface %s' % me.to_interface.name)) ?*/
/*- endif -*/

/*- include 'array-typedef-check.c' -*/

int /*? me.to_interface.name ?*/__run(void) {
    /*# Check any typedefs we have been given are not arrays. #*/
    /*- include 'call-array-typedef-check.c' -*/

    /*- set info = c_symbol('info') -*/
    seL4_MessageInfo_t /*? info ?*/ = seL4_Wait(/*? ep ?*/, & /*? me.to_interface.name ?*/_badge);
    while (1) {
        /*- if not options.fcall_leave_reply_cap or len(me.to_instance.type.provides + me.to_instance.type.uses + me.to_instance.type.consumes + me.to_instance.type.mutexes + me.to_instance.type.semaphores) > 1 -*/
            /* We need to save the reply cap because the user's implementation may
             * perform operations that overwrite or discard it.
             */
            /*- set result = c_symbol() -*/
            /*- set cnode = alloc_cap('cnode', my_cnode, write=True) -*/
            /*- set reply_cap_slot = alloc_cap('reply_cap_slot', None) -*/
            int /*? result ?*/ UNUSED = seL4_CNode_SaveCaller(/*? cnode ?*/, /*? reply_cap_slot ?*/, 32);
            ERR_IF(/*? result ?*/ != 0, /*? error_handler ?*/, ((camkes_error_t){
                    .type = CE_SYSCALL_FAILED,
                    .instance = "/*? instance ?*/",
                    .interface = "/*? interface ?*/",
                    .description = "failed to save reply cap in /*? name ?*/",
                    .syscall = CNodeSaveCaller,
                    .error = /*? result ?*/,
                }), ({
                    /*? info ?*/ = seL4_Wait(/*? ep ?*/, & /*? me.to_interface.name ?*/_badge);
                    continue;
                }));
        /*- endif -*/

        /*- set buffer = c_symbol('buffer') -*/
        void * /*? buffer ?*/ UNUSED = (void*)/*? base ?*/;

        /*- set size = c_symbol('size') -*/
        unsigned int /*? size ?*/ UNUSED = PAGE_SIZE_4K;

        /*- set call = c_symbol('call') -*/
        /*- set call_ptr = c_symbol('call_ptr') -*/
        /*- if methods_len <= 1 -*/
          unsigned int /*? call ?*/ UNUSED;
          unsigned int * /*? call_ptr ?*/ = TLS_PTR(/*? call_tls_var ?*/, /*? call ?*/);
          * /*? call_ptr ?*/ = 0;
        /*- elif methods_len <= 2 ** 8 -*/
          uint8_t /*? call ?*/ UNUSED;
          uint8_t * /*? call_ptr ?*/ = TLS_PTR(/*? call_tls_var ?*/, /*? call ?*/);
        /*- elif methods_len <= 2 ** 16 -*/
          uint16_t /*? call ?*/ UNUSED;
          uint16_t * /*? call_ptr ?*/ = TLS_PTR(/*? call_tls_var ?*/, /*? call ?*/);
        /*- elif methods_len <= 2 ** 32 -*/
          uint32_t /*? call ?*/ UNUSED;
          uint32_t * /*? call_ptr ?*/ = TLS_PTR(/*? call_tls_var ?*/, /*? call ?*/);
        /*- elif methods_len <= 2 ** 64 -*/
          uint64_t /*? call ?*/ UNUSED;
          uint64_t * /*? call_ptr ?*/ = TLS_PTR(/*? call_tls_var ?*/, /*? call ?*/);
        /*- else -*/
          /*? raise(Exception('too many methods in interface %s' % me.to_interface.name)) ?*/
        /*- endif -*/
        /*- if methods_len > 1 -*/
            ERR_IF(sizeof(* /*? call_ptr ?*/) > /*? size ?*/, /*? error_handler ?*/, ((camkes_error_t){
                    .type = CE_MALFORMED_RPC_PAYLOAD,
                    .instance = "/*? instance ?*/",
                    .interface = "/*? interface ?*/",
                    .description = "truncated message encountered while unmarshalling method index in /*? name ?*/",
                    .length = /*? size ?*/,
                    .current_index = sizeof(* /*? call_ptr ?*/),
                }), ({
                    /*? info ?*/ = seL4_Wait(/*? ep ?*/, & /*? me.to_interface.name ?*/_badge);
                    continue;
                }));

            memcpy(/*? call_ptr ?*/, /*? buffer ?*/, sizeof(* /*? call_ptr ?*/));
        /*- endif -*/

        switch (* /*? call_ptr ?*/) {
            /*- for i, m in enumerate(me.to_interface.type.methods) -*/
                case /*? i ?*/: { /*? '/' + '* ' + m.name + ' *' + '/' ?*/
                    /*# Declare parameters. #*/
                    /*- for p in m.parameters -*/

                        /*- if p.array -*/
                            size_t /*? p.name ?*/_sz UNUSED;
                            size_t * /*? p.name ?*/_sz_ptr = TLS_PTR(/*? m.name ?*/_/*? p.name ?*/_sz_to, /*? p.name ?*/_sz);
                            /*- if isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
                                char ** /*? p.name ?*/ UNUSED = NULL;
                                char *** /*? p.name ?*/_ptr = TLS_PTR(/*? m.name ?*/_/*? p.name ?*/_to, /*? p.name ?*/);
                            /*- else -*/
                                /*? show(p.type) ?*/ * /*? p.name ?*/ UNUSED = NULL;
                                /*? show(p.type) ?*/ ** /*? p.name ?*/_ptr = TLS_PTR(/*? m.name ?*/_/*? p.name ?*/_to, /*? p.name ?*/);
                            /*- endif -*/
                        /*- elif isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
                            char * /*? p.name ?*/ UNUSED = NULL;
                            char ** /*? p.name ?*/_ptr = TLS_PTR(/*? m.name ?*/_/*? p.name ?*/_to, /*? p.name ?*/);
                        /*- else -*/
                            /*? show(p.type) ?*/ /*? p.name ?*/ UNUSED;
                            /*? show(p.type) ?*/ * /*? p.name ?*/_ptr = TLS_PTR(/*? m.name ?*/_/*? p.name ?*/_to, /*? p.name ?*/);
                        /*- endif -*/
                    /*- endfor -*/

                    /* Unmarshal parameters */
                    /*- set function = '%s_unmarshal_inputs' % m.name -*/
                    /*- set input_parameters = filter(lambda('x: x.direction in [\'refin\', \'in\', \'inout\']'), m.parameters) -*/
                    /*- set err = c_symbol('error') -*/
                    int /*? err ?*/ = /*- include 'call-unmarshal-inputs.c' -*/;
                    if (unlikely(/*? err ?*/ != 0)) {
                        /* Error in unmarshalling; return to event loop. */
                        /*? info ?*/ = seL4_Wait(/*? ep ?*/, & /*? me.to_interface.name ?*/_badge);
                        continue;
                    }

                    /* Call the implementation */
                    /*- set ret = c_symbol('ret') -*/
                    /*- set ret_sz = c_symbol('ret_sz') -*/
                    /*- set ret_ptr = c_symbol('ret_ptr') -*/
                    /*- set ret_sz_ptr = c_symbol('ret_sz_ptr') -*/
                    /*- if m.return_type -*/
                        /*- if m.return_type.array -*/
                            size_t /*? ret_sz ?*/ UNUSED;
                            size_t * /*? ret_sz_ptr ?*/ = TLS_PTR(/*? m.name ?*/_ret_sz_to, /*? ret_sz ?*/);
                            /*- if isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
                                char ** /*? ret ?*/ UNUSED;
                                char *** /*? ret_ptr ?*/ = TLS_PTR(/*? m.name ?*/_ret_to, /*? ret ?*/);
                            /*- else -*/
                                /*? show(m.return_type) ?*/ * /*? ret ?*/ UNUSED;
                                /*? show(m.return_type) ?*/ ** /*? ret_ptr ?*/ = TLS_PTR(/*? m.name ?*/_ret_to, /*? ret ?*/);
                            /*- endif -*/
                        /*- elif isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
                            char * /*? ret ?*/ UNUSED;
                            char ** /*? ret_ptr ?*/ = TLS_PTR(/*? m.name ?*/_ret_to, /*? ret ?*/);
                        /*- else -*/
                            /*? show(m.return_type) ?*/ /*? ret ?*/ UNUSED;
                            /*? show(m.return_type) ?*/ * /*? ret_ptr ?*/ = TLS_PTR(/*? m.name ?*/_ret_to, /*? ret ?*/);
                        /*- endif -*/
                        * /*? ret_ptr ?*/ =
                    /*- endif -*/
                    /*? me.to_interface.name ?*/_/*? m.name ?*/(
                        /*- if m.return_type and m.return_type.array -*/
                            /*? ret_sz_ptr ?*/
                            /*- if len(m.parameters) > 0 -*/
                                ,
                            /*- endif -*/
                        /*- endif -*/
                        /*- for p in m.parameters -*/
                            /*- if p.array -*/
                                /*- if p.direction == 'in' -*/
                                    *
                                /*- endif -*/
                                /*? p.name ?*/_sz_ptr,
                            /*- endif -*/
                            /*- if p.direction =='in' -*/
                                *
                            /*- endif -*/
                            /*? p.name ?*/_ptr
                            /*- if not loop.last -*/,/*- endif -*/
                        /*- endfor -*/
                    );

                    /* Marshal the response */
                    /*- set function = '%s_marshal_outputs' % m.name -*/
                    /*- set output_parameters = filter(lambda('x: x.direction in [\'out\', \'inout\']'), m.parameters) -*/
                    /*- set return_type = m.return_type -*/
                    /*- set length = c_symbol('length') -*/
                    unsigned int /*? length ?*/ = /*- include 'call-marshal-outputs.c' -*/;

                    /*# We no longer need anything we previously malloced #*/
                    /*- if m.return_type -*/
                      /*- if m.return_type.array -*/
                        /*- if isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
                          /*- set mcount = c_symbol() -*/
                          for (int /*? mcount ?*/ = 0; /*? mcount ?*/ < * /*? ret_sz_ptr ?*/; /*? mcount ?*/ ++) {
                            free((* /*? ret_ptr ?*/)[/*? mcount ?*/]);
                          }
                        /*- endif -*/
                        free(* /*? ret_ptr ?*/);
                      /*- elif isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
                        free(* /*? ret_ptr ?*/);
                      /*- endif -*/
                    /*- endif -*/
                    /*- for p in m.parameters -*/
                      /*- if p.array -*/
                        /*- if isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
                          /*- set mcount = c_symbol() -*/
                          for (int /*? mcount ?*/ = 0; /*? mcount ?*/ < * /*? p.name ?*/_sz; /*? mcount ?*/ ++) {
                            free((* /*? p.name ?*/)[/*? mcount ?*/]);
                          }
                        /*- endif -*/
                        free(* /*? p.name ?*/_ptr);
                      /*- elif isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
                        free(* /*? p.name ?*/_ptr);
                      /*- endif -*/
                    /*- endfor -*/

                    /* Check if there was an error during marshalling. We do
                     * this after freeing internal parameter variables to avoid
                     * leaking memory on errors.
                     */
                    if (unlikely(/*? length ?*/ == UINT_MAX)) {
                        /* Error occurred; return to event loop. */
                        /*? info ?*/ = seL4_Wait(/*? ep ?*/, & /*? me.to_interface.name ?*/_badge);
                        continue;
                    }

                    /*? info ?*/ = seL4_MessageInfo_new(0, 0, 0, 0);

                    /* Send the response */
                    /*- if not options.fcall_leave_reply_cap or len(me.to_instance.type.provides + me.to_instance.type.uses + me.to_instance.type.consumes + me.to_instance.type.mutexes + me.to_instance.type.semaphores) > 1 -*/
                        seL4_Send(/*? reply_cap_slot ?*/, /*? info ?*/);
                        /*? info ?*/ = seL4_Wait(/*? ep ?*/, & /*? me.to_interface.name ?*/_badge);
                    /*- else -*/

                        /*? info ?*/ = seL4_ReplyWait(/*? ep ?*/, /*? info ?*/, & /*? me.to_interface.name ?*/_badge);
                    /*- endif -*/

                    break;
                }
            /*- endfor -*/
            default: {
                ERR(/*? error_handler ?*/, ((camkes_error_t){
                        .type = CE_INVALID_METHOD_INDEX,
                        .instance = "/*? instance ?*/",
                        .interface = "/*? interface ?*/",
                        .description = "invalid method index received in /*? name ?*/",
                        .lower_bound = 0,
                        .upper_bound = /*? methods_len ?*/ - 1,
                        .invalid_index = * /*? call_ptr ?*/,
                    }), ({
                        /*? info ?*/ = seL4_Wait(/*? ep ?*/, & /*? me.to_interface.name ?*/_badge);
                        continue;
                    }));
            }
        }
    }

    UNREACHABLE();
}
