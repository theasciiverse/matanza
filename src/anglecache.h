#ifndef __MATANZA_ANGLECACHE_H__
#define __MATANZA_ANGLECACHE_H__

typedef struct MatAngleCache MatAngleCache;

MatAngleCache            *mat_anglecache_new               ( unsigned int        angs );

void                      mat_anglecache_free              ( MatAngleCache      *m );

INLINE double             mat_anglecache_sin               ( MatAngleCache      *m,
                                                             unsigned int        ang );

INLINE double             mat_anglecache_cos               ( MatAngleCache      *m,
                                                             unsigned int        ang );

unsigned int              mat_anglecache_best              ( MatAngleCache      *m,
                                                             double              dx,
                                                             double              dy,
                                                             double              dist );

#endif
