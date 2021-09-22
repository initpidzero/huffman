#include <stdio.h>
#include <stdlib.h>

struct table {
	char letter;
	int freq;
	struct table *next;
};

int present_in_table(char str, struct table *table)
{
		struct table *temp;
		for (temp = table; temp; temp = temp->next) {
			if(str == temp->letter)
				return 1;
		}
		return 0;

}

/* TODO get rid of this function since we are not using this anymore */
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

int number_of_occurence(char * str, struct table *node, struct table *table)
{
	char *temp;
	int unique = 0;
	for (temp = str; *temp; temp++) {
		if (*temp == node->letter)
			node->freq++;
		else
			if (!unique) {
				if (!present_in_table(*temp, table))
					unique = temp - str;
			} else
				printf("unique = %d\n", unique);

	}

	return unique;
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

int main (int argc, char *argv[])
{
	char *str = "ABABABABCDDDEAABBECEADFBADFDFA";
	int i = 0;
	int ret = 0;
	struct table *table = (struct table *)malloc(sizeof(struct table));
	struct table *temp = table;
	if (!table)
		return -1;

	if (argc >1)
		str = argv[1];

	table->letter = str[i];
	table->freq = 0;
	table->next = NULL;
	while(str[i]) {
		int pos_of_next = 0;
		pos_of_next = number_of_occurence(str, temp, table);
		if (pos_of_next) {
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
