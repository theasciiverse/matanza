#ifndef __MATANZA_DLIST_H__
#define __MATANZA_DLIST_H__

INLINE MatScm            *mat_dlist_add_head               ( MatScm **head,
                                                             MatScm **tail,
                                                             MatScm *obj );
INLINE MatScm            *mat_dlist_add_tail               ( MatScm **head,
                                                             MatScm **tail,
                                                             MatScm *obj );
INLINE MatScm            *mat_dlist_add_before             ( MatScm **head,
                                                             MatScm **tail,
                                                             MatScm *next,
                                                             MatScm *obj );
INLINE MatScm            *mat_dlist_add_after              ( MatScm **head,
                                                             MatScm **tail,
                                                             MatScm *prev,
                                                             MatScm *obj );
INLINE void               mat_dlist_rm                     ( MatScm **head,
                                                             MatScm **tail,
                                                             MatScm *ent );
INLINE void               mat_dlist_forall                 ( MatScm *head,
                                                             void (*func) ( void *, MatScm * ),
                                                             void *data );
INLINE MatScm            *mat_dlist_prev_get               ( MatScm *ent );
INLINE MatScm            *mat_dlist_next_get               ( MatScm *ent );
INLINE MatScm            *mat_dlist_obj_get                ( MatScm *ent );

#endif
