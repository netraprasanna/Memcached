
#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include <libmemcached/memcached.h>
#define MEMC_UDF_MAX_SIZE 256*256
#define ERRMSG_SIZE 1000

my_bool memc_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_set(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_set_deinit(UDF_INIT *initid);

my_bool memc_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  unsigned int count;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_SET, 2, 3);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);
  count= memcached_server_count(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_set(UDF_INIT *initid, UDF_ARGS *args,
                   char *is_null,
                   char *error)
{
  memcached_return rc;
  *is_null= false;

  memc_function_st *container= (memc_function_st *)initid->ptr;

  if (args->args[1] == NULL)
    args->lengths[1]= 0;

  rc= memcached_set(&container->memc,
                    args->args[0], (size_t)args->lengths[0],
                    args->args[1], (size_t)args->lengths[1],
                    container->expiration, (uint16_t)0);

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;
}

void memc_set_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}

