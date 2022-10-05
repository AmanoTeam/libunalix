#include <assert.h>
#include <string.h>

#include "query.h"
#include "errors.h"

int main() {
	
	struct Query query = {};
	
	const int code = query_parse(&query, "a=b&&g&h=&=i");
	assert (code == UNALIXERR_SUCCESS);
	
	assert (strcmp(query.parameters[0].key, "a") == 0);
	assert (strcmp(query.parameters[0].value, "b") == 0);
	
	assert (query.parameters[1].key == NULL);
	assert (query.parameters[1].value == NULL);
	
	assert (strcmp(query.parameters[2].key, "g") == 0);
	assert (query.parameters[2].value == NULL);
	
	assert (strcmp(query.parameters[3].key, "h") == 0);
	assert (query.parameters[3].value == NULL);
	
	assert (query.parameters[4].key == NULL);
	assert (strcmp(query.parameters[4].value, "i") == 0);
	
	query_free(&query);
	
	return 0;
	
}