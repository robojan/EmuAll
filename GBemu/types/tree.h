#ifndef _TREE_H
#define _TREE_H

struct node
{
	void	*ptr;		
	int		key;		
	node	*left;		// key less than
	node	*middle;	// key equal than
	node	*right;		// key greater than
};

class tree
{
public:
	tree();
	~tree();

	void insert(int key, void *ptr);
	node *search(int key);
	void destroy_tree();
private:
	void destroy_tree(node *leaf);
	void insert(int key, void *ptr, node *leaf);
	node *search(int key, node *leaf);

	node *root;
};

#endif _TREE_H