#include "../Common.hpp"

typedef int T;

class LinkedList
{
public:
	typedef void(*IterateFunc)(T*);

	template<class UserClass> struct ClassIterator
	{
		typedef void(*_Func)(UserClass*, T*);
		UserClass* userClass;
		_Func func;
		inline ClassIterator(UserClass* _class, _Func _func) : userClass(_class), func(_func) {}
		inline void Invoke(T data) { func(userClass, data); };
	};

	struct Node {
		T* data;
		Node* next;
		Node(T* _data, Node* _next) : data(_data), next(_next) {  }
	};
public:
	LinkedList() : rootNode(nullptr), endNode(nullptr), nodeCount(0) { }

	LinkedList(T* firstData) : rootNode(new Node(firstData, nullptr)), nodeCount(1) { endNode = rootNode; }

	LinkedList(Node* _rootNode) : rootNode(_rootNode), endNode(_rootNode), nodeCount(1) { }

	~LinkedList() { IterateRecDestroy(rootNode); }

	// not recomended for iterating. for iterating Iterate function is more efficient
	T* operator[](int index) const
	{
		int currIndex = 0;
		Node* start = rootNode;
		while (start != nullptr)
		{
			if (currIndex == index) return start->data;
			currIndex++;
			start = start->next;
		}
		return nullptr;
	}

	// #define _Template template<typename Derived, typename std::enable_if < \
		// 	std::is_base_of<T, Derived>{} || std::is_same<T, Derived>{}, bool > ::type = true >
#define _Template template<typename Derived>

	_Template void AddFront(Derived data)
	{
		Node* newNode = nullptr;
		if constexpr (std::is_pointer<T>())
			newNode = new Node(dynamic_cast<T>(data), nullptr);
		else newNode = new Node(static_cast<T>(data), nullptr);

		if (endNode) {
			endNode->next = newNode;
		}
		else {
			endNode = newNode;
			rootNode = newNode;
		}
		endNode = newNode;
		nodeCount++;
	}

	// sets data as first node(root node)
	_Template void AddBack(Derived data)
	{
		Node oldRoot = rootNode;
		if constexpr (std::is_pointer<T>() || std::is_class<T>())
			rootNode = new Node(dynamic_cast<T>(data), oldRoot);
		else rootNode = new Node(static_cast<T>(data), oldRoot);
		nodeCount++;
	}

	_Template void Remove(Derived ptr)
	{
		if (!rootNode) return;
		Node* currentNode = rootNode;

		if (rootNode->data == ptr) {
			rootNode = rootNode->next;
			delete currentNode;
			--nodeCount;
			currentNode = rootNode;
			return;
		}

		do {
			if (currentNode->next->data == ptr)
			{
				Node* removedNode = currentNode->next;
				currentNode->next = removedNode->next;
				--nodeCount;
				delete removedNode;
				break;
			}
			currentNode = currentNode->next;
		} while (currentNode->next != nullptr);
	}

	_Template bool TryGetData(Derived* onent)
	{
		*component = FindNodeByType<Derived>();
		return component[0];
	}

#undef _Template
	// returns removed data
	T* RemoveFront()
	{
		if (nodeCount == 0) return nullptr;
		T* oldEndNodeData = endNode->data;
		Node* oldNode = endNode;
		--nodeCount;
		endNode = FindSecondEndNodeRec(rootNode); // new end node
		endNode->next = nullptr;
		delete oldNode;
		return oldEndNodeData;
	}

	// returns removed data
	// removes first node(root node)
	T* RemoveBack()
	{
		if (!rootNode) return 0;
		T* oldRootNodeData = rootNode->data;
		Node* oldNode = rootNode;
		--nodeCount;
		rootNode = rootNode->next;
		delete oldNode;
		return oldRootNodeData;
	}

	template<class Desired> Desired FindNodeByType() const
	{
		Node* curr = rootNode;

		while (curr != nullptr)
		{
			if (curr->data != nullptr && dynamic_cast<Desired*>(curr->data))
				return dynamic_cast<Desired*>(node->data);
			curr = curr->next;
 		}

		return nullptr;
	}

	T* FindNodeFromPtr(T* ptr) const
	{
		Node* currentNode = rootNode;
		if (currentNode->data == ptr) {
			return currentNode->data;
		}

		while (currentNode->next != nullptr) {
			currentNode = currentNode->next;
			if (currentNode->data == ptr) {
				return currentNode->data;
			}
		}
		return nullptr;
	}

	void Rotate()
	{
		Node* oldRoot = rootNode;
		rootNode = endNode;
		endNode = oldRoot;
		rootNode->next = oldRoot->next;
		endNode->next = nullptr;
	}

	void Reverse()
	{
		int i = nodeCount + 1;
		while (i--) { Rotate(); }
	}

	// this function is more performant than reaching every node by index
	void Iterate(IterateFunc func) const
	{
		Node* currentNode = rootNode;
		func(currentNode->data);

		while (currentNode->next != nullptr) {
			currentNode = currentNode->next;
			func(currentNode->data);
		}
	}

	template<class UserClass>
	void IterateClass(ClassIterator<UserClass> iterator) const
	{
		Node* currentNode = rootNode;
		iterator.Invoke(currentNode->data);

		while (currentNode->next != nullptr) {
			currentNode = currentNode->next;
			iterator.Invoke(currentNode->data);
		}
	}
private:

	// finds this[nodeCount-2] not last one but one before lastNode
	Node* FindSecondEndNodeRec(Node* node)
	{
		if (node->next == nullptr) return nullptr;
		if (node->next->next == nullptr) return node;
		return FindSecondEndNodeRec(node->next);
	}

	void IterateRecDestroy(Node* node)
	{
		if (!node) return;
		if (node->next != nullptr)
			IterateRecDestroy(node->next);
		delete node;
	}
public:
	Node* rootNode, * endNode;
	int nodeCount;
};