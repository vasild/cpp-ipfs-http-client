
#ifndef _HIVE_API_H_
#define _HIVE_API_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char* hive_generate_conf(const char* host, int port);
const char* hive_refresh_conf(const char* defaultConf);
const char* hive_random_host(const char* volatileConf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HIVE_API_H_ */
