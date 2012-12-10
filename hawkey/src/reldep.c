#include <assert.h>

// libsolv
#include <solv/pool.h>
#include <solv/util.h>

// hawkey
#include "iutil.h"
#include "reldep_internal.h"
#include "sack_internal.h"

struct _HyReldep {
    Pool *pool;
    Id r_id;
};

struct _HyReldepList {
    Pool *pool;
    Queue queue;
};

HyReldep
reldep_create(Pool *pool, Id r_id)
{
    HyReldep reldep = solv_calloc(1, sizeof(*reldep));
    reldep->pool = pool;
    reldep->r_id = r_id;
    return reldep;
}

Id
reldep_id(HyReldep reldep)
{
    return reldep->r_id;
}

HyReldepList
reldeplist_from_queue(Pool *pool, Queue h)
{
    HyReldepList reldeplist = solv_calloc(1, sizeof(*reldeplist));
    reldeplist->pool = pool;
    queue_init_clone(&reldeplist->queue, &h);
    return reldeplist;
}

HyReldep
hy_reldep_create(HySack sack, const char *name, int cmp_type, const char *evr)
{
    Pool *pool = sack_pool(sack);
    Id id = pool_str2id(pool, name, 0);

    if (id == STRID_NULL || id == STRID_EMPTY)
	// stop right there, this will never match anything.
	return NULL;

    if (evr) {
	assert(cmp_type);
        Id ievr = pool_str2id(pool, evr, 1);
        int flags = cmptype2relflags(cmp_type);
        id = pool_rel2id(pool, id, ievr, flags, 1);
    }
    return reldep_create(pool, id);
}

void
hy_reldep_free(HyReldep reldep)
{
    solv_free(reldep);
}

HyReldep
hy_reldep_clone(HyReldep reldep)
{
    return reldep_create(reldep->pool, reldep->r_id);
}

char
*hy_reldep_str(HyReldep reldep)
{
    const char *str = pool_dep2str(reldep->pool, reldep->r_id);
    return solv_strdup(str);
}

HyReldepList
hy_reldeplist_create(HySack sack)
{
    HyReldepList reldeplist = solv_calloc(1, sizeof(*reldeplist));
    reldeplist->pool = sack_pool(sack);
    queue_init(&reldeplist->queue);
    return reldeplist;
}

void
hy_reldeplist_free(HyReldepList reldeplist)
{
    queue_free(&reldeplist->queue);
    solv_free(reldeplist);
}

void
hy_reldeplist_add(HyReldepList reldeplist, HyReldep reldep)
{
    queue_push(&reldeplist->queue, reldep->r_id);
}

int
hy_reldeplist_count(HyReldepList reldeplist)
{
    return reldeplist->queue.count;
}

HyReldep
hy_reldeplist_get_clone(HyReldepList reldeplist, int index)
{
    Id r_id = reldeplist->queue.elements[index];
    return reldep_create(reldeplist->pool, r_id);
}
