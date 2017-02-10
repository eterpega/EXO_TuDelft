/*
 * deviceconfig.c
 *
 * Read device configuration for the SDO transfers from CSV file.
 *
 * Frank Jeschke <fjeschke@synapticon.com>
 *
 * 2017 Synapticon GmbH
 */

#include "deviceconfig.h"

#include <stdio.h>
#include <string.h>

#define MAX_INPUT_LINE    1024
#define MAX_TOKEN_SIZE    255

struct _token_t {
	char **token;
	size_t count;
	struct _token_t *next;
};

/* every token list has the size (token_count) = 2 + number_of_axis */
static size_t get_node_count(struct _token_t *token_list)
{
	struct _token_t *t = token_list;
	size_t token_count = t->count;
	t = t->next;

	while (t->next != NULL) {
		if (token_count != t->count)
			return 0; /* parse error -> token count == 0 */

		t = t->next;
	}

	return (token_count - 2); /* substract index and subindex fields */
}

static size_t get_token_count(char *buf, size_t bufsize)
{
	size_t separator = 0;
	char *c = buf;

	for (size_t i = 0; i < bufsize && *c != '\0'; i++, c++) {
		if (*c == ',')
			separator++;
	}

	return (separator + 1);
}

static void free_token(struct _token_t *t)
{
	if (t->next == NULL) {
		free(t);
	} else {
		free_token(t->next);
	}
}

static void dc_tokenize_inbuf(char *buf, size_t bufsize, struct _token_t *token)
{
	char *sep = ",";
	char *b = malloc(bufsize * sizeof(char));
	char *word = NULL;

	size_t tokenitem = 0;
	token->count = get_token_count(buf, bufsize);
	token->token = malloc(token->count * sizeof(char *));

	memmove(b, buf, bufsize * sizeof(char));

	for (word = strtok(b, sep);  word; word = strtok(NULL, sep)) {
		*(token->token + tokenitem) = malloc(strlen(word) + 1);
		strncpy(*(token->token + tokenitem), word, (strlen(word) + 1));
		tokenitem++;
	}

	free(b);
}

static long parse_token(char *token_str)
{
	long value = strtol(token_str, NULL, 0);

	return value;
}

static void parse_token_for_node(struct _token_t *tokens, SdoParam_t *param, size_t node)
{
	param->index    = (uint16_t) parse_token(*(tokens->token));
	param->subindex = (uint8_t)  parse_token(*(tokens->token + 1));
	param->value    = (uint32_t) parse_token(*(tokens->token + 2 + node));
}

static void dc_parse_tokens(struct _token_t *token, SdoParam_t **params)
{
	uint16_t index    = (uint16_t)parse_token(*(token->token + 0));
	uint8_t  subindex = (uint8_t)parse_token(*(token->token + 1));

	for (size_t k = 0; k < (token->count - 2); k++) {
		SdoParam_t *p = /*malloc(sizeof(SdoParam_t)); */ *(params + k); /* FIXME allocate the params memory! */
		p->index = index;
		p->subindex = subindex;
		p->value = (uint32_t)parse_token(*(token->token + k + 2));

		printf("I: 0x%04x:%d - bitsize: %lu\n", p->index, p->subindex, p->bytecount);
	}

	printf("[DEBUG tokens] ");
	for (size_t i = 0; i < token->count; i++) {
		printf("'%s', ", *(token->token + i));
	}
	printf("\n");
}

int dc_read_file(const char *path, SdoConfigParameter_t *parameter)
{
	if (parameter == NULL)
		return -1;

	FILE *f = fopen(path, "r");
	if (f == NULL) {
		return -1;
	}

	struct _token_t *token = malloc(sizeof(struct _token_t));
	token->token = NULL;
	token->count = 0;
	token->next  = NULL;

	struct _token_t *t = token;
	size_t param_count = 0;

	char inbuf[MAX_INPUT_LINE];
	size_t inbuf_length = 0;
	int c;

	/* read file and tokenize */
	while ((c = fgetc(f)) != EOF) {
		if (c == '#') {
			while (c != '\n') {
				c = fgetc(f);
			}
		}

		if (c == '\n') {
			if (inbuf_length > 1) {
				inbuf[inbuf_length++] = '\0';
				dc_tokenize_inbuf(inbuf, inbuf_length, t);
				param_count++;
				t->next = calloc(1, sizeof(struct _token_t));
				if (t->next != NULL)
					t = t->next;

				//dc_parse_tokens(token, parameter->parameter);

			}

			inbuf_length = 0;
			continue;
		}

		if (c == ' ' || c == '\t') /* filter whitespaces - FIXME attention if strings are supported! */
			continue;

		inbuf[inbuf_length] = (char)c;
		inbuf_length++;
	}

	int retval = -1;
	if (feof(f)) {
		retval = 0;
	}

	fclose(f);

	parameter->param_count = param_count;
	parameter->node_count  = get_node_count(token);
	if (parameter->node_count == 0 || parameter->param_count == 0) {
		fprintf(stderr, "Parse error number of nodes are different!\n");
		free_token(token);
		return -1;
	}

	parameter->parameter = (SdoParam_t **)malloc(parameter->node_count * sizeof(SdoParam_t *));
	SdoParam_t **sdoparam = parameter->parameter;

	for (size_t node = 0; node < parameter->node_count; node++) {
		SdoParam_t *p = *(sdoparam + node);
		p = malloc(parameter->param_count * sizeof(SdoParam_t));

		t = token;
		for (size_t param = 0; param < parameter->param_count; param++) {
			if (t->next == NULL) {
				fprintf(stderr, "Warning, token ran out before parameter count was reached.\n");
				break;
			}

			parse_token_for_node(t, (p + param), node);
			t = t->next;
		}
	}

	free_token(token);

	return retval;
}
