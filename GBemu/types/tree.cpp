#include "../util/memDbg.h"


#include "tree.h"
tree::tree()
{
	root = NULL;
}

tree::~tree()
{
	destroy_tree();
}

void tree::destroy_tree(node *leaf)
{
	if(leaf != NULL)
	{
		destroy_tree(leaf->left);
		destroy_tree(leaf->middle);
		destroy_tree(leaf->right);
		delete leaf;
	}
}

void tree::insert(int key, void *ptr, node *leaf)
{
	if(key < leaf->key)
	{
		if(leaf->left!=NULL)
		{
			insert(key, ptr, leaf->left);
		} else {
			leaf->left = new node;
			leaf->left->key = key;
			leaf->left->ptr = ptr;
			leaf->left->left = NULL;
			leaf->left->middle = NULL;
			leaf->left->right = NULL;
		}
	} else if( key == leaf->key) {
		if(leaf->middle != NULL)
		{
			insert(key, ptr, leaf->middle);
		} else {
			leaf->middle = new node;
			leaf->middle->key = key;
			leaf->middle->ptr = ptr;
			leaf->middle->left = NULL;
			leaf->middle->middle = NULL;
			leaf->middle->right = NULL;
		}
	} else if( key > leaf->key) {
		if(leaf->right != NULL)
		{
			insert(key, ptr, leaf->right);
		} else {
			leaf->right = new node;
			leaf->right->key = key;
			leaf->right->ptr = ptr;
			leaf->right->left = NULL;
			leaf->right->middle = NULL;
			leaf->right->right = NULL;
		}
	}
}

node *tree::search(int key, node *leaf)
{
	if(leaf != NULL)
	{
		if(key == leaf->key)
		{
			return leaf;
		} 
		if(key < leaf->key)
		{
			return search(key, leaf->left);
		} else {
			return search(key, leaf->right);
		}
	}
	return NULL;
}

void tree::insert(int key, void *ptr)
{
	if(root != NULL)
	{
		insert(key, ptr, root);
	} else {
		root = new node;
		root->key = key;
		root->ptr = ptr;
		root->left = NULL;
		root->middle = NULL;
		root->right = NULL;
	}
}

node *tree::search(int key)
{
	return search(key, root);
}

void tree::destroy_tree()
{
	destroy_tree(root);
	root = NULL;
}