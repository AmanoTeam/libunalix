struct Parameter {
	char* key;
	char* value;
};

struct Query {
	size_t size;
	size_t position;
	size_t slength;
	struct Parameter* parameters;
};

int query_parse(struct Query* obj, const char* query);
void query_free(struct Query* obj);
int add_parameter(struct Query* obj, const char* key, const char* value);
char* query_stringify(const struct Query obj, char* dst);
