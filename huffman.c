#include <stdio.h>
#include <stdlib.h>

struct table {
	char letter;
	int freq;
	struct table *next;
};

struct tree {
	char letter;
	int freq;
	struct tree *left;
	struct tree *right;

};

void swap_node(struct table *node1, struct table *node2)
{
	char letter = node1->letter;
	int freq = node1->freq;
	node1->letter =node2->letter;
	node1->freq = node2->freq;
	node2->letter = letter;
	node2->freq = freq;
}

void print_table(struct table *table)
{
	struct table *temp;
	printf("character	frequency\n");
	for (temp = table; temp; temp = temp->next) {
		printf("%c		%d\n", temp->letter, temp->freq);
	}
}


int present_in_table(char str, struct table *table)
{
	struct table *temp;
	for (temp = table; temp; temp = temp->next) {
		if(str == temp->letter)
			return 1;
	}
	return 0;

}

int sort_table(struct table *table)
{
	struct table *cur = table;
	struct table *next = table->next;
	for (cur = table; cur->next != NULL; cur = cur->next) {
		for(next = cur->next; next != NULL; next = next->next) {
			if(next->freq < cur->freq)
				swap_node(cur, next);
		}
	}
	print_table(table);
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

struct tree *add_tree_node(char letter, int freq)
{
	struct tree *tree = (struct tree *)malloc(sizeof(struct tree));
	tree->letter = letter;
	tree->freq = freq;
	tree->left = NULL;
	tree->right = NULL;

	return tree;
}

struct tree *create_tree(struct table *lnode, struct table *rnode)
{
	struct tree *root;
	struct tree *temp;

	root = add_tree_node(-1, lnode->freq + rnode->freq);

	temp = add_tree_node(lnode->letter, lnode->freq);
	root->left = temp; /* attach left node */

	temp = add_tree_node(rnode->letter, rnode->freq);
	root->right = temp; /* attach right node */

	return root;
}

struct tree *create_mix_tree(struct tree *tree, struct table *node)
{

	struct tree *root;
	struct tree *temp;

	root = add_tree_node(-1, tree->freq + node->freq);

	root->left = tree; /* attach left node */

	temp = add_tree_node(node->letter, node->freq);
	root->right = temp; /* attach right node */

	return root;

}

struct tree *combine_tree(struct tree *tree, struct tree *new_tree)
{
	struct tree *root;
	root = add_tree_node(-1, tree->freq + new_tree->freq);
	root->left = tree; /* attach left node */
	root->right = new_tree; /* attach right node */

	return root;
}

int create_huffman_tree(struct table *table)
{
	struct table *cur = table;
	struct table *next = cur->next;
	struct tree *c_root = create_tree(cur, next);

	for (cur = next->next; cur->next != NULL; cur = cur->next) {
		struct tree *temp;
		if (cur->next) {
			next = cur->next;
			if (c_root->freq <= next->freq) {
				temp = create_mix_tree(c_root, cur);
				c_root = temp;
			} else {
				temp = create_tree(cur, next);
				c_root = combine_tree(c_root, temp);
			}

		} else {
			temp = create_mix_tree(c_root, cur);
			c_root = temp;
		}

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
	sort_table(table);
	printf("%s\n", str);
err:
	if (table) free_table(table);

	return ret;
}
