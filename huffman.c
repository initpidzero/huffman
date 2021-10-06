/* An implementation of huffman encoding */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* list for finding finding frequency of each character */
struct table {
	char letter;
	int freq;
	struct table *next;
};

/* huffman tree */
struct tree {
	char letter;
	int freq;
	struct tree *left;
	struct tree *right;

};

/* lookup table for character to codes */
struct code_table {
	char letter;
	char *code_prefix;
	int len;
};

/* for tree traversal, we want to know if it was left child or right child */
enum direction {
	RIGHT = 0,
	LEFT = 1,
	ROOT = 2,
};

/* For getting codes, we need to get path from root to leaves */
struct path {
	int letter;
	int len;
	int cur;
	unsigned char *dir;
	void **pointer;
	struct code_table *codes;
};

/* the last nodes on huffman tree */
static int total_leaves = 0;

/* swap function for sorting our list */
void swap_node(struct table *node1, struct table *node2)
{
	char letter = node1->letter;
	int freq = node1->freq;
	node1->letter =node2->letter;
	node1->freq = node2->freq;
	node2->letter = letter;
	node2->freq = freq;
}

/* print list for characters and thier frequency in string */
void print_table(struct table *table)
{
	struct table *temp;
	if (!table)
		return;
	printf("character	frequency\n");
	for (temp = table; temp; temp = temp->next) {
		printf("%c		%d\n", temp->letter, temp->freq);
	}
}


/* check if we have already counted this character in our list
 * */
int present_in_table(char str, struct table *table)
{
	struct table *temp;
	if (!table)
		return -1;
	for (temp = table; temp; temp = temp->next) {
		if(str == temp->letter)
			return 1;
	}
	return 0;

}

/* a basic sort for our char-freq list */
/* it returns total nodes in the list */
int sort_table(struct table **table)
{
	int total_nodes  = 1; /* since we are skipping last node, we start at 1 */
	struct table *cur = *table;
	struct table *next = (*table)->next;
	for (cur = *table; cur->next != NULL; cur = cur->next) {
		for(next = cur->next; next != NULL; next = next->next) {
			if(next->freq < cur->freq)
				swap_node(cur, next);
		}
		total_nodes++;
	}
	printf("total_nodes = %d\n", total_nodes);

	return total_nodes;
}

/* Function to check how many time does a character appear in the string*/
int number_of_occurence(char *str, struct table *node, struct table *table)
{
	char *temp;
	int unique = 0;
	for (temp = str; *temp; temp++) {
		if (*temp == node->letter)
			node->freq++;
		else
			if (!unique) {
				/* Find the next character which has already not
				 * been counted (next unique) */
				if (!present_in_table(*temp, table))
					unique = temp - str;
			} else {
			//	printf("unique = %d\n", unique);
			}

	}

	return unique;
}

void free_code_table(struct code_table *codes, int total)
{
	int i;
	for(i =0; i < total; i++)
		if (codes[i].code_prefix) free(codes[i].code_prefix);
	free(codes);
}

void free_tree(struct tree **tree)
{
	if ((*tree)->left == NULL && (*tree)->right == NULL) {
		free(*tree);
		*tree = NULL;
	}
	else if ((*tree)->left)
		free_tree(&(*tree)->left);
	else if ((*tree)->right)
		free_tree(&(*tree)->right);

}

void free_table(struct table **table)
{
	struct table *cur = *table;
	while (cur) {
		struct table *temp = cur;
		cur = cur->next;
		free(temp);
		temp = NULL;
	}
}

void print_path(struct path *path)
{
	printf("print path for %c cur = %d:\n",path->letter, path->cur);

	for (int i = 0; i < path->len; i++) {
		if(!path->pointer[i])
			break;
		printf("%p\n",path->pointer[i]);
		printf("%d\n",path->dir[i]);
	}
}

void print_preorder(struct tree *tree, enum direction dir)
{
	if (!tree)
		return;

	if (dir == ROOT) {
		printf("Tree Starts here-->\n root node\n");
		printf("Node = %p total freq = %d\n",tree, tree->freq);
	} else if (dir == LEFT) {
		printf("1\n");
		printf("Node = %p total freq = %d\n",tree, tree->freq);
		if (tree->letter != -1)
			printf("letter = %c\n",tree->letter);
	} else {
		printf("0\n");
		printf("Node = %p total freq = %d\n",tree, tree->freq);
		if (tree->letter != -1)
			printf("letter = %c\n",tree->letter);

	}
	print_preorder(tree->left, LEFT);
	print_preorder(tree->right, RIGHT);

}

struct tree *add_tree_node(char letter, int freq)
{
	struct tree *tree = (struct tree *)malloc(sizeof(struct tree));
	if (!tree) return NULL;

	tree->letter = letter;
	tree->freq = freq;
	tree->left = NULL;
	tree->right = NULL;

//	printf("char = %c freq =%d\n", (tree->letter == -1)?'?':tree->letter, tree->freq);
	return tree;
}

/* make a tree with two table nodes */
struct tree *create_tree(struct table *lnode, struct table *rnode)
{
	struct tree *root;
	struct tree *temp;

	if (!lnode)
		return NULL;
	/* this might only happen if there is one one node */
	if (!rnode)
		return add_tree_node(lnode->letter, lnode->freq);

	root = add_tree_node(-1, lnode->freq + rnode->freq);
	if (!root) return NULL;

	temp = add_tree_node(lnode->letter, lnode->freq);
	if (!temp) return NULL;
	root->left = temp; /* attach left node */

	temp = add_tree_node(rnode->letter, rnode->freq);
	if (!temp) return NULL;
	root->right = temp; /* attach right node */

	return root;
}

/* make a tree with two tree nodes*/
struct tree *combine_tree(struct tree *tree, struct tree *new_tree)
{
	struct tree *root;
	root = add_tree_node(-1, tree->freq + new_tree->freq);
	if (!root) return NULL;
	root->left = tree; /* attach left node */
	root->right = new_tree; /* attach right node */

	return root;
}

/* make a tree by attaching tree node to left and table node to right*/
struct tree *create_mix_tree(struct tree *tree, struct table *node)
{
	struct tree *root;
	struct tree *temp;

	root = add_tree_node(-1, tree->freq + node->freq);
	if (!root) return NULL;

	root->left = tree; /* attach left node */

	temp = add_tree_node(node->letter, node->freq);
	if (!temp) return NULL;
	root->right = temp; /* attach right node */

	return root;
}

struct tree *create_huffman_tree(struct table *table)
{
	struct tree *c_root;
	struct table *cur = table;
	struct table *next = cur->next;

	c_root = create_tree(cur, next);
	if (!c_root) return NULL;

	if (!next)
		return c_root;
	for (cur = next->next; cur != NULL; cur = cur->next) {
		struct tree *temp = NULL;
		next = cur->next;
		if (next) {
			if (c_root->freq <= next->freq) {
				temp = create_mix_tree(c_root, cur);
				if (!temp) return NULL;
				c_root = temp;
			} else {
				temp = create_tree(cur, next);
				if (!temp) return NULL;
				c_root = combine_tree(c_root, temp);
				if (!c_root) return NULL;
				cur = next; /* we have consumed both */
			}

		} else {
			c_root = create_mix_tree(c_root, cur);
			if (!c_root) return NULL;
		}

	}

	return c_root;
}

/* we know left path is the longest path
*/
int longest_path(struct tree *tree)
{
	struct tree *node = NULL;
	int count = 0;
	for (node = tree; node; node = node->left)
		count++;

	return count;
}

/* fill path pointer with NULL values */
void reset_path(struct path *path, int reset_index)
{
	int i;
	path->cur = reset_index; /* current node */
	for (i = reset_index; i < path->len; i++) {
			path->pointer[i] = NULL;
	}
}

/* add a new tree node address to the path table */
void add_to_path(struct path *path, struct tree *node, enum direction dir)
{
	int i;

	if (!path)
		return;
	if (!node)
		return reset_path(path, 0);

	if (path->cur >= path->len) {
		printf("ERR: FULL PATH\n");
		return;
	}

	//printf("add pointer = %p cur = %d\n", node, path->cur);
	path->pointer[path->cur] = node;
	path->dir[path->cur] = dir;
	path->cur++;
	return;

}

/* path table is acting like a stack, so we remove the tree nodes when we are
 * coming back from our recurse function */
void remove_last_node(struct path *path)
{
	if (path->cur <= 0) {
		printf("ERR: EMPTY ATH\n");
		return;
	}
	printf("remove %d\n", path->cur);
	path->pointer[path->cur] = NULL;
	path->cur--;

}

/* At this point, path variable contains exact path between root node and relevant character node */
void extract_path(struct path *path)
{
	int j;

	path->codes->len = path->cur - 1;
	/* if there is only one node in the tree */
	if (path->codes->len == 0)
		path->codes->len = 1;
	path->codes->code_prefix = (char *)malloc(path->codes->len);
	if (!path->codes->code_prefix)
		return;
	printf("code for letter %c:\n", path->letter);

        if (path->codes->len == 1) {
                path->codes->code_prefix[0] = 0;
                return;
        }

	for (j = 0; j < path->codes->len; j++) {
		path->codes->code_prefix[j] = path->dir[j + 1];
		printf("%d", path->codes->code_prefix[j]);
	}
	printf("\n");
}

/* Traverse the tree until you find right character node/leaf */
void path_in_tree(struct tree *tree, struct path *path, enum direction dir)
{
	if (!tree)
		return;
	add_to_path(path, tree, dir);
	if (tree->letter == path->letter) {
		//		printf("%c %p\n",path->letter, tree);
		printf("*************\n");
		print_path(path);
		printf("*************\n");
		extract_path(path);
		return;
	}
	path_in_tree(tree->left, path, LEFT);
	path_in_tree(tree->right, path, RIGHT);
	/* pop */
	remove_last_node(path);
}

/* all the nodes present in our tree */
int total_nodes(struct tree *tree)
{
	static int count = 0;
	if (!tree)
		return count;
	else
		count++;
	total_nodes(tree->left);
	total_nodes(tree->right);

	return count;
}

/* we have the tree, now we want to extract codes using path between root to
 * leaf nodes */
int convert_to_codes(struct tree *tree, struct code_table *codes)
{
	int i;
	int ret = 0;
	int num_nodes = total_nodes(tree); /* number of nodes in tree */

	printf("num_nodes = %d\n", num_nodes);
	for (i = 0; i < total_leaves; i++) {

		struct path path;

		memset(&path, 0 , sizeof(struct path));
		path.pointer = NULL;
		path.dir = NULL;

		path.pointer = malloc(num_nodes * sizeof(void *));
		if (!path.pointer) {
			ret = -1;
			goto freepath;
		}
		path.dir = (unsigned char *)malloc(num_nodes * sizeof(unsigned char));
		if (!path.dir) {
			ret = -1;
			goto freepath;
		}
		//printf("dir = %p\n",path->dir);
		path.len = num_nodes;
		reset_path(&path, 0);

		path.letter = codes[i].letter;
		path.codes = &codes[i];
		path_in_tree(tree, &path, ROOT);

		print_path(&path);

freepath:
		//printf("dir = %p\n",path.dir);
		if (path.dir) free(path.dir);
		if (path.pointer) free(path.pointer);
		if (ret == -1) break;
	}

	return ret;
}

int freq_of_chars(struct table **table, char *str)
{
	int i = 0;
	struct table *temp;
	if (!table)
		return -1;

	(*table)->letter = str[i];
	(*table)->freq = 0;
	(*table)->next = NULL;
        temp = *table;
	while (str[i]) {
		int pos_of_next = 0;
		pos_of_next = number_of_occurence(str, temp, *table);
		if (pos_of_next) {
			i  = pos_of_next;
	//		printf("pos = %d i = %d\n", pos_of_next, i);
			temp->next = (struct table *)malloc(sizeof(struct table));
			if (!temp->next) {
				fprintf(stderr, "no mem\n");
                                return -1;
			}
			temp->next->letter = str[i];
			temp->next->freq = 0;
                        temp->next->next = NULL;
			temp = temp->next;
		} else
			break;

	}
        return 0;
}
/* how many bits are required to encode this string */
int total_bits_encoded(struct code_table *codes, struct table *table)
{
	struct table *temp;
	int i = 0;
	int sum = 0;

        if (!table)
                return -1;

	for (temp = table ;temp; temp=temp->next, i++) {
		if (temp->letter != codes[i].letter) {
			fprintf(stderr, "Catastropic err\n");
			return -1;
		}
		sum = sum + (temp->freq * codes[i].len);
	}

	return sum;
}

/* encode str into encoded_str using codes from code table codes */
void encode(char *str, struct code_table *codes, char *encoded_str)
{
	char *tmp = NULL;
	int j = 0;
	for (tmp = str; *tmp; tmp++) {
		int i;
		for (i = 0; i < total_leaves; i++)
			if (codes[i].letter == *tmp) {
				memcpy(encoded_str + j, codes[i].code_prefix, codes[i].len);
				j += codes[i].len;
				break;
			}
	}
}

/* decode encoded_str to decoded_str using code table codes */
void decode(char *encoded_str, int encoded_len, char *decoded_str, struct code_table *codes)
{
	int j = 0;
	int i;
	int k;
	int match = 0;
	int m = 0;
	while(j < encoded_len) {
		for (i = total_leaves - 1; i >=0; i--) {
			for(k = 0; k < codes[i].len; k++) {
				if (codes[i].code_prefix[k] == encoded_str[k + j])
					match = 1;
				else {
					match = 0;
					break;
				}
			}
			if (match == 1) {
				decoded_str[m++] = codes[i].letter;
				//printf("matched at %d %c %c\n", j, codes[i].letter , decoded_str[j]);
				j = j + codes[i].len;
				break;
			}
		}

	}
	decoded_str[m] = '\0';

}

int decode_len(struct table *table)
{
	int sum = 1;
	struct table *temp;

        if (!table)
                return -1;

	for (temp = table; temp; temp=temp->next) {
		sum = sum + (temp->freq);
	}

	return sum;
}

int main (int argc, char *argv[])
{
	struct code_table *codes;
	struct tree *huffman_tree;
	char *str = "ABABABABCDDDEAABBECEADFBADFDFA";
	int ret = 0;
	struct table *temp;
	char *encoded_str;
	char *decoded_str;
	int encoding_len;
	int decoding_len;
	int i;
	struct table *table = (struct table *)malloc(sizeof(struct table));
	if (!table)
		goto err;

	if (argc >1)
		str = argv[1];

	ret = freq_of_chars(&table, str);
	if (ret == -1)
		goto err;

	print_table(table);
	total_leaves = sort_table(&table);
	huffman_tree = create_huffman_tree(table);
	if (!huffman_tree)
		goto err;
	print_preorder(huffman_tree, ROOT);

	codes = malloc(sizeof(struct code_table) * total_leaves);
	if (!codes)
		goto err;

	for (temp = table, i = 0; temp; temp=temp->next, i++) {
		codes[i].letter = temp->letter;
		codes[i].code_prefix = NULL;
		printf("codes letter = %c\n", codes[i].letter);
	}

	if (convert_to_codes(huffman_tree, codes) == -1)
		goto err;

	printf("%s\n", str);
	print_table(table);
	for (i = 0; i < total_leaves; i++) {
		int j;
		printf("letter = %c code =", codes[i].letter);
		for (j = 0; j < codes[i].len; j++)
			printf("%d", codes[i].code_prefix[j]);
		printf("\n");
	}

	encoding_len = total_bits_encoded(codes, table);
	encoded_str = malloc(encoding_len);
	if (!encoded_str)
		goto err;

	printf("total bytes needed = %d\n", encoding_len);
	encode(str, codes, encoded_str);
	printf("%s encoded to:\n", str);
	for (i = 0; i < encoding_len; i++)
		printf("%d", encoded_str[i]);
	printf("\n");

	decoding_len = decode_len(table);
	decoded_str = malloc(decoding_len);
	if (!decoded_str)
		goto err;
	decode(encoded_str,encoding_len, decoded_str ,codes);
	printf("decoded string = %s\n", decoded_str);
err:
	if (table) free_table(&table);
	if (huffman_tree) free_tree(&huffman_tree);
	if (codes) free_code_table(codes, total_leaves);
	if (encoded_str) free(encoded_str);
	if (decoded_str) free(decoded_str);

	return ret;
}
