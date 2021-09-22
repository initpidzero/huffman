#include <stdio.h>
#include <stdlib.h>

struct table {
	char letter;
	int freq;
	struct table *next;
};

int find_next_unique(char *str, struct table *table)
{
	struct table *temp;
	char *new = str;

	for(; *new; new++) {
		int match = 0;
		for (temp = table; temp; temp = temp->next) {
			if(*new == temp->letter)
				match = 1;
			printf("*str %c temp->letter %c\n", *new,temp->letter);
		}
		if (!match)
			return new - str;
	}
	return -1;
}

int number_of_occurence(char * str, struct table *node)
{
	char *temp;
	for (temp = str; *temp; temp++) {
		if (*temp == node->letter)
			node->freq++;

	}

	return 0;
}

void free_table(struct table *table)
{
	struct table *temp = table;
	struct table *cur;
	for (cur = temp->next; cur; cur = cur->next) {
		free(temp);
		temp = NULL;
		temp = cur;
	}
}

void print_table(struct table *table)
{
	struct table *temp;
		printf("character	frequency\n");
	for (temp = table; temp; temp = temp->next) {
		printf("%c		%d\n", temp->letter, temp->freq);
	}
}

int main (void)
{
	char *str = "ABABABABCDDDEAABBECEADFBADFDFA";
	int i = 0;
	int ret = 0;
	struct table *table = (struct table *)malloc(sizeof(struct table));
	struct table *temp = table;
	if (!table)
		return -1;

	table->letter = str[i];
	table->freq = 0;
	table->next = NULL;
	while(str[i]) {
		int pos_of_next = 0;
		number_of_occurence(str, temp);
		pos_of_next = find_next_unique(str, table);
		if (pos_of_next != -1) {
			i  = pos_of_next;
			printf("pos = %d i = %d\n", pos_of_next, i);
			temp->next = (struct table *)malloc(sizeof(struct table));
			if (!temp) {
				fprintf(stderr, "no mem for reallocation\n");
				ret = -1;
				goto err;
			}
			temp = temp->next;
			temp->letter = str[i];
			temp->freq = 0;
		} else
			break;


	}
	print_table(table);
	printf("%s\n", str);
err:
	if (table) free_table(table);

	return ret;
}
