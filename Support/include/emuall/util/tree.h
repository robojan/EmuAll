#pragma once

#include <emuall/common.h>
#include <cassert>
#include <iterator>

template<typename KeyType, typename ValueType>
class AATree 
{
	struct Node {
		Node(KeyType key, ValueType value, int level, struct Node *left, struct Node *middle, struct Node *right) :
			key(key), value(value), level(level), left(left), middle(middle), right(right)
		{ }

		KeyType key;
		ValueType value;
		int level;
		struct Node *left;
		struct Node *middle;
		struct Node *right;
	};
public:
	template<typename KeyType, typename ValueType>
	class ForwardIterator : public std::iterator<std::forward_iterator_tag,
		ValueType, std::ptrdiff_t, ValueType*, ValueType &>
	{
	public:
		ForwardIterator() : _itr(nullptr) {}
		explicit ForwardIterator(Node *node) : _itr(node) {}
		
		void swap(ForwardIterator &other) noexcept
		{
			std::swap(_itr, other._itr);
		}

		ForwardIterator &operator++()
		{
			assert(_itr != nullptr);
			_itr = _itr->middle;
			return *this;
		}

		ForwardIterator operator++(int)
		{
			assert(_itr != nullptr);
			ForwardIterator tmp(*this);
			_itr = _itr->middle;
			return tmp;
		}

		template<typename OtherKeyType, typename OtherValueType>
		bool operator == (const ForwardIterator<OtherKeyType, OtherValueType> &rhs) const
		{
			return _itr == rhs._itr;
		}

		template<typename OtherKeyType, typename OtherValueType>
		bool operator != (const ForwardIterator<OtherKeyType, OtherValueType> &rhs) const
		{
			return _itr != rhs._itr;
		}

		ValueType &operator*() const
		{
			assert(_itr != nullptr);
			return _itr->value;
		}

		ValueType &operator->() const
		{
			assert(_itr != nullptr);
			return _itr->value;
		}
		operator ForwardIterator<KeyType, const ValueType>() const
		{
			return ForwardIterator<KeyType, const ValueType>(_itr);
		}

	private:
		Node *_itr;
	};

	typedef ForwardIterator<KeyType, ValueType> iterator;
	typedef ForwardIterator<KeyType, const ValueType> const_iterator;

	AATree();
	~AATree();

	template<typename KeyType, typename ValueType>
	void Insert(KeyType key, ValueType value) {
		_root = Insert(_root, key, value);
	}

	template<typename KeyType, typename ValueType>
	void Delete(KeyType key) {
		_root = Delete(_root, key);
	}

	void Clear() {
		_root = Clear(_root);
	}

	const_iterator CEnd() { return const_iterator(); }
	iterator End() { return iterator(); }

	iterator Find(KeyType key) {
		Node *startNode = Find(_root, key);
		return iterator(startNode);
	}

private: 
	Node *_root;

	Node *Find(Node *node, KeyType key) {
		if (node == nullptr) {
			return nullptr;
		}
		if (key < node->key) {
			return Find(node->left, key);
		}
		else if (key > node->key) {
			return Find(node->right, key);
		}
		else {
			return node;
		}
	}

	bool IsValid(Node *node) {
		if (node == nullptr) return true;
		if (node->left != nullptr) {
			if (node->left->key >= node->key || 
				!IsValid(node->left)) return false;
		}
		if (node->right != nullptr) {
			if (node->right->key <= node->key ||
				!IsValid(node->right)) return false;
		}
		return true;
	}

	Node *Clear(Node *node) {
		if (node == nullptr)
			return nullptr;
		node->middle = Clear(node->middle);
		node->left = Clear(node->left);
		node->right = Clear(node->right);
		delete node;
		return nullptr;
	}

	template<typename KeyType, typename ValueType>
	Node *Insert(Node *node, KeyType key, ValueType value)
	{
		if (node == nullptr) {
			node = new Node(key, value, 1, nullptr, nullptr, nullptr);
		}
		else if (key < node->key) {
			node->left = Insert(node->left, key, value);
		}
		else if (key > node->key) {
			node->right = Insert(node->right, key, value);
		}
		else if (key == node->key) {
			node->middle = Insert(node->middle, key, value);
		}
		node = Skew(node);
		node = Split(node);
		assert(IsValid(node));
		return node;
	}

	Node *Delete(Node *node, KeyType key) {
		if (node == nullptr) {
			return node;
		}
		else if (key > node->key) {
			node->right = Delete(node->right, key);
		}
		else if (key < node->key) {
			node->left = Delete(node->left, key);
		}
		else {
			if (node->left == nullptr && node->right == nullptr) {
				node->middle = Delete(node->middle, key);
				delete node;
				return nullptr;
			}
			else if (node->left == nullptr) {
				Node *l = Successor(node);
				node->key = l->key;
				node->value = l->value;
				node->middle = Delete(node->middle, key);
				node->middle = l->middle;
				l->middle = nullptr;
				node->right = Delete(node->right, l->key);
			}
			else {
				Node *l = Predecessor(node);
				node->key = l->key;
				node->value = l->value;
				node->middle = Delete(node->middle, key);
				node->middle = l->middle;
				l->middle = nullptr;
				node->left = Delete(node->left, l->key);
			}
		}
		node = DecreaseLevel(node);
		node = Skew(node);
		node->right = Skew(node->right);
		if (node->right != nullptr) {
			node->right->right = Skew(node->right->right);
		}
		node = Split(node);
		node->right = Split(node->right);
		return node;
	}

	Node *Predecessor(Node *node) {
		if (node->left == nullptr) {
			return nullptr;
		}
		node = node->left;
		while (node->right != nullptr) {
			node = node->right;
		}
		return node;
	}

	Node *Successor(Node *node) {
		if (node->right == nullptr) {
			return nullptr;
		}
		node = node->right;
		while (node->left != nullptr) {
			node = node->left;
		}
		return node;
	}

	Node *DecreaseLevel(Node *node) {
		if (node->left == nullptr || node->right == nullptr) {
			return node;
		}
		int shouldBe = node->left->level < node->right->level ? node->left->level : node->right->level;
		if (shouldBe < node->level) {
			node->level = shouldBe;
			if (shouldBe < node->right->level) {
				node->right->level = shouldBe;
			}
		}
		return node;
	}

	Node *Skew(Node *node) {
		if (node == nullptr) {
			return nullptr;
		}
		else if (node->left == nullptr) {
			return node;
		}
		else if (node->left->level == node->level) {
			Node *l = node->left;
			node->left = l->right;
			l->right = node;
			return l;
		}
		else {
			return node;
		}
	}

	Node *Split(Node *node) {
		if (node == nullptr) {
			return nullptr;
		}
		else if (node->right == nullptr || node->right->right == nullptr) {
			return node;
		}
		else if (node->level == node->right->right->level) {
			Node *r = node->right;
			node->right = r->left;
			r->left = node;
			r->level += 1;
			return r;
		}
		else {
			return node;
		}
	}

};

template<typename KeyType, typename ValueType>
AATree<KeyType, ValueType>::AATree() :
	_root(nullptr)
{

}

template<typename KeyType, typename ValueType>
AATree<KeyType, ValueType>::~AATree() {
	Clear();
}