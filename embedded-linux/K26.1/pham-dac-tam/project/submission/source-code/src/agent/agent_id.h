#ifndef AGENT_ID_H
#define AGENT_ID_H

#include <stddef.h>

int load_or_create_agent_id(const char *path, char *agent_id, size_t size);

#endif // AGENT_ID_H