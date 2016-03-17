
#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include <libmemcached/memcached.h>
#define MEMC_UDF_MAX_SIZE 256*256
#define ERRMSG_SIZE 1000

/*init UDF*/
my_bool memc_delete_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_delete(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_delete_deinit(UDF_INIT *initid);

my_bool memc_delete_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_DELETE, 1, 2);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

/*
  memc_delete
  delete cached object, takes hash-key arg
*/
long long memc_delete(UDF_INIT *initid,
                      UDF_ARGS *args,
                      __attribute__ ((unused)) char *is_null,
                      char *error)
{
  memcached_return rc;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  rc= memcached_delete(&container->memc,
                       args->args[0], (size_t)args->lengths[0],
                       container->expiration);

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;
}

/* de-init UDF */
void memc_delete_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}

