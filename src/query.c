#include <stdlib.h>

#include "query.h"
#include "uri.h"
#include "errors.h"

static int put_parameter(struct Query* obj, const struct Parameter parameter) {
	
	const size_t size = obj->size + sizeof(struct Parameter) * 1;
	struct Parameter* parameters = realloc(obj->parameters, size);
	
	if (parameters == NULL) {
		return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
	}
	
	obj->size = size;
	obj->parameters = parameters;
	
	obj->parameters[obj->position] = parameter;
	obj->position++;
	
	if (obj->slength > 0) {
		obj->slength += strlen(AND);
	}
	
	if (parameter.key != NULL) {
		obj->slength += strlen(parameter.key);
	}
	
	obj->slength += strlen(EQUAL);
	
	if (parameter.value != NULL) {
		obj->slength += strlen(parameter.value);
	}
	
	return UNALIXERR_SUCCESS;
	
}

int add_parameter(struct Query* obj, const char* key, const char* value) {
	
	struct Parameter parameter = {0};
	
	const size_t key_size = strlen(key);
	
	if (key_size > 0) {
		parameter.key = malloc(key_size + 1);
		
		if (parameter.key == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(parameter.key, key);
	}
	
	const size_t value_size = strlen(value);
	
	if (value_size > 0) {
		parameter.value = malloc(value_size + 1);
		
		if (parameter.value == NULL) {
			return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
		}
		
		strcpy(parameter.value, value);
	}
	
	return put_parameter(obj, parameter);
	
}

char* query_stringify(const struct Query obj, char* dst) {
	
	memset(dst, '\0', 1);
	
	for (size_t index = 0; index < obj.position; index++) {
		const struct Parameter parameter = obj.parameters[index];
		
		if (*dst != '\0') {
			strcat(dst, AND);
		}
		
		if (parameter.key != NULL) {
			strcat(dst, parameter.key);
		}
		
		strcat(dst, EQUAL);
		
		if (parameter.value != NULL) {
			strcat(dst, parameter.value);
		}
	}
	
	return dst;
	
}

int query_parse(struct Query* obj, const char* query) {
	
	const char* query_end = strchr(query, '\0');
	
	const char* param_start = query;
	const char* param_end = strstr(query, AND);
	
	while (1) {
		if (param_end == NULL) {
			param_end = query_end;
		}
		
		const char* separator = strstr(param_start, EQUAL);
		
		if (separator == NULL || separator > param_end) {
			separator = param_end;
		}
		
		struct Parameter parameter = {0};
		
		const size_t key_size = (size_t) (separator - param_start);
		
		if (key_size > 0) {
			parameter.key = malloc(key_size + 1);
			
			if (parameter.key == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			memcpy(parameter.key, param_start, key_size);
			parameter.key[key_size] = '\0';
		}
		
		if (separator != param_end) {
			separator++;
		}
		
		const size_t value_size = (size_t) (separator == param_end ? 0 : param_end - separator);
		
		if (value_size > 0) {
			parameter.value = malloc(value_size + 1);
			
			if (parameter.value == NULL) {
				return UNALIXERR_MEMORY_ALLOCATE_FAILURE;
			}
			
			memcpy(parameter.value, separator, value_size);
			parameter.value[value_size] = '\0';
		}
		
		const int code = put_parameter(obj, parameter);
		
		if (code != UNALIXERR_SUCCESS) {
			return code;
		}
		
		if (param_end == query_end) {
			break;
		}
		
		param_start = param_end;
		param_start++;
		
		param_end = strstr(param_start, AND);
	}
	
	return UNALIXERR_SUCCESS;
	
}

void query_free(struct Query* obj) {
	
	for (size_t index = 0; index < obj->position; index++) {
		struct Parameter* parameter = &obj->parameters[index];
		
		if (parameter->key != NULL) {
			free(parameter->key);
			parameter->key = NULL;
		}
		
		if (parameter->value != NULL) {
			free(parameter->value);
		}
	}
	
	obj->size = 0;
	obj->position = 0;
	
	free(obj->parameters);
	obj->parameters = NULL;
	
}
