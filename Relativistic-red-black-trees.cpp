#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <assert.h>

#define TOTAL_THREADS 	5
#define MAX_NUM_NODES 	10000
#define NUM_READERS 		10
#define EPOCH 					200

// RESTRUCTURE CASES
#define DIAG_LEFT 			1
#define ZIG_LEFT 				2
#define DIAG_RIGHT 			3
#define ZIG_RIGHT				4

#define BLACK 					0
#define RED 						1

using namespace std;

// CONVENTION:
// aNode = currNode
// bNode = parent
// cNode = grandparent

// Node definition
template <class T>
class Node {
	public:
		// Member variables
		T val;
		int key;
		bool color;
		Node<T> *left, *right, *parent;
		Node<T> *backup;

		Node(int _key) {
			key = _key;
			color = BLACK;
			left = NULL, right = NULL, parent = NULL, backup = NULL;
		}

		Node<T> *getCopy() {
			return backup;
		}
};

// RRBT definition
template <class T>
class RealRBT {
	private:
		vector<Node<T>*> nodeBank;
		atomic_int nodeBankIndex{0};
		int *readers;
		int reader;
		atomic_int epoch{EPOCH};

	public:
		atomic<Node<T>*> root;
		atomic_int size{0};
		mutex *lock;

		RealRBT() {
			root = NULL;
			lock = new mutex();
			readers = new int[NUM_READERS];

			for(int i = 0; i < MAX_NUM_NODES; i++) {
				Node<T> *n = new Node<T>(0);
				n->backup = new Node<T>(0);

				nodeBank.push_back(n);
			}
		}

		Node<T> *getNewNode() {
			return nodeBank[(nodeBankIndex++) % MAX_NUM_NODES];
		}

		// RP read primitives
		void startRead() {
			readers[reader] = EPOCH;
		}

		void endRead() {
			readers[reader] = 0;
		}

		// RP write primitives
		void waitForReaders() {
			int currEpoch;

			this->epoch++;
			currEpoch = this->epoch;

			for(int i = 0; i < NUM_READERS; i++)
				while(readers[i] != 0 && readers[i] < currEpoch);
		}

		Node<T> *insert(T x) {
			size_t key = hash<T>{}(x);
			Node<T> *newNode = getNewNode();

			newNode->key = key;
			*(newNode->val) = x;

			treeInsert(root, newNode);

			repairRBT(newNode);

			root = newNode;

			while(root->parent != NULL)
				root = root->parent;

			return root;

		}

		void treeInsert(Node<T> *root, Node<T> *newNode)
		{

			if(root == NULL)
			{
				newNode->parent = root;
				newNode->left = NULL;
				newNode->right = NULL;
				newNode->color = RED;

				return;
			}

			if(newNode->val < root->val)
			{//traverse to the left side of the tree
				if(root->left != NULL)
				{
					treeInsert(root->left, newNode);
					return;
				}

				root->left = newNode;
			}
			else if(root->right != NULL)
			{//try to traverse the right side of the tree
				treeInsert(root->right, newNode);
				return;
			}

			root->right = newNode;
		}

		void repairRBT(Node<T> *newNode)
		{
			if(newNode->parent == NULL)
			{
				newNode->color = BLACK;
				return;
			}
			else if(newNode->parent == BLACK)
			{
				return;
			}
			else if(newNode->parent == RED)
			{
				newNode->parent->color = BLACK;
				getUncle(newNode)->color = BLACK;
				getGrandparent(newNode)->color = BLACK;
				treeInsert(getGrandparent(newNode));
				return;
			}

			Node<T> p = newNode->parent;
			Node<T> g = getGrandparent(newNode);

			if(newNode == g->left->right)
			{
				Node<T> temp = p->right;
				assert(temp != NULL); // since the leaves of a red-black tree are empty, they cannot become internal nodes
				p->right = temp->left;
				temp->left = p;
				p->parent = p->parent;
				p->parent = temp;

				newNode = newNode->left;
			}
			else if(newNode == g->right->left)
			{
				Node<T> temp = p->left;
				assert(temp != NULL); // since the leaves of a red-black tree are empty, they cannot become internal nodes
				p->left = temp->right;
				temp->right = p;
				p->parent = p->parent;
				p->parent = temp;

				newNode = newNode->right;
			}

			p = newNode->parent;
			g = getGrandparent(newNode);

			if(newNode == p->left)
			{
				Node<T> temp = g->left;
				assert(temp != NULL); // since the leaves of a red-black tree are empty, they cannot become internal nodes
				g->left = temp->right;
				temp->right = g;
				g->parent = g->parent;
				g->parent = temp;
			}
			else
			{
				Node<T> temp = g->right;
				assert(temp != NULL); // since the leaves of a red-black tree are empty, they cannot become internal nodes
				g->right = temp->left;
				temp->left = g;
				g->parent = g->parent;
				g->parent = temp;
			}

			p->color = BLACK;
			g->color = RED;

		}

  // this function will return false if the parent is red and true if the parent is black
  bool placeNode(Node<T> *root, Node<T> *newNode) {
    if(root->key > newNode->key) {
      if(root->left == NULL) {
        root->left = newNode;
        newNode->parent = root;
        return checkUncle(root);
      }
		}
  }

// Get the leftmost node from the given node
		Node<T> *leftmost(Node<T> *node) {
			if(node == NULL)
				return NULL;

			Node<T> *temp = node;
			while(temp->left != NULL)
				temp = temp->left;

			return temp;
		}

		// Get the rightmost node from the given node
		Node<T> *rightmost(Node<T> *node) {
			if(node == NULL)
				return NULL;

			Node<T> *temp = node;
			while(temp->right != NULL)
				temp = temp->right;

			return temp;
		}

		T *first(int reader) {
			Node<T> *node = NULL;
			T *val = NULL;

			startRead();

			node = leftmost(this->root);

			if(node == NULL)
				return NULL;

			val = node->val;

			endRead();

			return val;
		}

		T *last() {
			Node<T> *node = NULL;
			T *val = NULL;

			startRead(reader);

			node = rightmost(this->root);

			if(node == NULL)
				return NULL;

			val = node->val;

			endRead(reader);

			return val;
		}

		// Performs a regular BST style insertion sequentially
		bool bstInsert(Node<T> *newNode) {
			if(newNode == NULL)
				return this->root;

			Node<T> *temp = this->root;

			while(temp != NULL) {

				if(newNode->key == temp->key)
					return false;
				else if(newNode->key > temp->key)
					temp = temp->right;
				else
					temp = temp->left;
			}

			if(newNode->key >= temp->parent->key)
				temp->right = newNode;
			else
				temp->left = newNode;

			return true;
		}

		Node<T> *sibling(Node<T> *node) {
			if (node->parent->left == node)
        return node->parent->right;
    	else
        return node->parent->left;
		}

		int restructure(Node<T> *node, Node<T> *parent, Node<T> *grandpa,
										Node<T> *aNode, Node<T> *bNode, Node<T> *cNode) {

			if(grandpa->left == parent && parent->left == node) {
				diagLeftRestruct(node, parent, grandpa);
				return DIAG_LEFT;
			}
			else if(grandpa->left == parent && parent->right == node) {
				zigLeftRestruct(node, parent, grandpa);
				return ZIG_LEFT;
			}
			else if(parent->right == node && grandpa->right == parent) {
				diagRightRestruct(node, parent, grandpa);
				return DIAG_RIGHT;
			}
			else {
				zigRightRestruct(node, parent, grandpa);
				return ZIG_RIGHT;
			}

			return 0;
		}

		void recolor(Node<T> *node) {
			Node<T> *parent = node->parent;
			Node<T> *grandpa = parent->parent;
			Node<T> *uncle = sibling(parent);

			if(uncle == NULL || uncle->color == BLACK) {
				restructure(node, parent, grandpa);
				node->color = RED;
				parent->color = BLACK;
				grandpa->color = RED;
			}
			else if(uncle != NULL && uncle->color == RED) {
				uncle->color = BLACK;
				parent->color = BLACK;

				if(grandpa->parent != NULL)
					grandpa->color = RED;

				if(grandpa->parent != NULL && grandpa->parent->color == RED)
					recolor(grandpa);
			}
		}

		bool insert(int key, T *val) {
			Node<T> *newNode = getNewNode();
			newNode->val = val;
			newNode->key = key;

			// If empty tree
			if(this->root == NULL) {
				newNode->color = BLACK;
				this->root = newNode;
			}
			else {
				bool success = bstInsert(newNode);

				if(!success)
					return false;

				if(newNode->parent->color == RED)
					recolor(newNode);
			}

			return true;
		}

		Node<T> *lookupHelper(int key) {
			Node<T> *temp = this->root;

			while(temp != NULL && key != temp->key) {
				if(key > temp->key)
					temp = temp->right;
				else
					temp = temp->left;
			}

			return temp;
		}

		T *lookup(int key) {
			Node<T> *found = lookupHelper(key);

			if(found == NULL)
				return NULL;

			return found->val;
		}

		// Leftmost node in right subtree
		Node<T> *next(Node<T> *currentNode) {
			return leftmost(currentNode->right);
		}

		// Rightmost node in left subtree
		void *previous(Node<T> *currentNode) {
			return rightmost(currentNode->left);
		}

		void swap(Node<T> *bNode) {
			Node<T> *cNode = next(bNode);
			Node<T> *cPrimeNode = cNode->getCopy();

			cPrimeNode->color = bNode->color;

			cPrimeNode->left = bNode->left;
			cPrimeNode->left->parent = cPrimeNode;

			cPrimeNode->right = bNode->left;
			cPrimeNode->right->parent = cPrimeNode;

			Node<T> *fNode = bNode->parent;
			cPrimeNode->parent = fNode;

			if(fNode->left == bNode)
				;// rpPublish(fNode->left, cPrimeNode);
			else
				;// rpPublish(fNode->right, cPrimeNode);

			rpFree(bNode);

			waitForReaders();

			Node<T> *eNode = cNode->parent;
			rpPublish(eNode->left, cNode->right);
			cNode->right->parent = eNode;

			rpFree(cNode);
		}

		void specialSwap(Node<T> *bNode) {
			Node<T> *cNode = next(bNode);

			cNode->color = bNode->color;
			cNode->left = bNode->left;
			cNode->left->parent = cNode;

			Node<T> *eNode = bNode->parent;

			if(eNode->left == bNode)
				;// rpPublish(eNode->left, cNode);
			else
				;// rpPublish(eNode->right, cNode);

			rpFree(bNode);
		}

		// This a function that will delete the node that has two children
		void interiorDelete(Node<T> *nodeToDelete) {
			Node<T> *cNode = next(nodeToDelete);

			Node<T> *cPrimeNode = cNode->getCopy();

			cPrimeNode->color = nodeToDelete->color;

			cPrimeNode->left = nodeToDelete->left;
			cPrimeNode->left->parent = cPrimeNode;

			cPrimeNode->right = nodeToDelete->right;
			cPrimeNode->right->parent = cPrimeNode;

			Node<T> *fNode = nodeToDelete->parent;
			cPrimeNode->parent = fNode;

			if(fNode->left == nodeToDelete)
				//TODO create this fuction
				;////rpPublish(fNode->left, cPrimeNode);
			else
				//TODO crate this fuction
				;///rpPublish(fNode->right, cPrimeNode);
			//TODO crate this fuction
			////rpFree(nodeToDelete);

			////waitForReaders();

			Node<T> *eNode = cNode->parent;
			//TODO crate this fuction
			/////////rpPublish(eNode->left, cNode->right);
			cNode->right->parent = eNode;

			//TODO crate this fuction
			rpFree(cNode);
		}

		// This is for the case of the special delete of the repalcement node being the child of
		// the node to be deleted
		void specialInteriorDelete(Node<T> *bNode) {
			Node<T> *cNode = next(bNode);

			cNode->color = bNode->color;
			cNode->left = bNode->left;
			cNode->left->parent = cNode;

			Node<T> *eNode = bNode->parent;
			if(eNode->left == bNode)
				;//////rpPublish(eNode->left, cNode);
			else
				;//////rpPublish(eNode->right, cNode);

			rpFree(bNode);
		}


		// This function will handle the restucturing of the tree
		// with left diagnal
		void diagLeftRestruct(Node<T> *aNode, Node<T> *bNode, Node<T> *cNode) {
			Node<T> *cNodePrime = cNode->getCopy();

			cNodePrime->left = bNode->right;
			cNodePrime->left->parent = cNodePrime;

			///rpPublish(bNode->right, cNodePrime);
			cNodePrime->parent = bNode;

			Node<T> *dNode = cNode->parent;

			if(dNode->left == cNode)
				;////rpPublish(dNode->left, bNode);
			else
				;//////rpPublish(dNode->right, bNode);

			bNode->parent = dNode;

			rpFree(cNode);
		}

		void diagRightRestruct(Node<T> *aNode, Node<T> *bNode, Node<T> *cNode) {
			Node<T> *cNodePrime = cNode->getCopy();

			cNodePrime->right = bNode->left;
			cNodePrime->right->parent = cNodePrime;

			///rpPublish(bNode->left, cNodePrime);
			cNodePrime->parent = bNode;

			Node<T> *dNode = cNode->parent;

			if(dNode->right == cNode)
				;////rpPublish(dNode->right, bNode);
			else
				;//////rpPublish(dNode->left, bNode);

			bNode->parent = dNode;

			rpFree(cNode);
		}

		void zigLeftRestruct(Node<T> *aNode, Node<T> *bNode, Node<T> *cNode) {
			Node<T> *aPrime = aNode->getCopy();
			aPrime->right = bNode->left;
			aPrime->right->parent = aPrime;

			//rpPublish(B->left, aPrime);
			aPrime->parent = bNode;

			Node<T> *cPrime = cNode->getCopy();
			cPrime->left = bNode->right;
			cPrime->left->parent = cPrime;

			// rpPublish(B->right, cPrime);
			cPrime->parent = bNode;

			Node<T> *dNode = cNode->parent;

			if(dNode->left == cNode)
				;//rpPubllish(D->left, B);
			else
				;//rpPublish(D->right, B);

			rpFree(aNode);
			rpFree(cNode);
		}

		void zigRightRestruct(Node<T> *aNode, Node<T> *bNode, Node<T> *cNode) {
			Node<T> *aPrime = aNode->getCopy();
			aPrime->left = bNode->right;
			aPrime->left->parent = aPrime;

			//rpPublish(B->right, aPrime);
			aPrime->parent = bNode;

			Node<T> *cPrime = cNode->getCopy();
			cPrime->right = bNode->left;
			cPrime->right->parent = cPrime;

			// rpPublish(B->left, cPrime);
			cPrime->parent = bNode;

			Node<T> *dNode = cNode->parent;

			if(dNode->right == cNode)
				;//rpPubllish(D->right, B);
			else
				;//rpPublish(D->left, B);

			rpFree(aNode);
			rpFree(cNode);
		}

		void rpFree(Node<T> *node) {
			free(node);
		}

		// void deleteNode(int key) {
		// 	return numOps;
		// }

		//returns the lowest keyed value within the tree
		 Node<T> *first(Node<T> root)
		 {
			if (root == NULL)
				return NULL;
			Node<T> temp = root;
			while (temp->left != NULL)
			{
				temp = temp->left;
			}
			return temp;
		 }

		//returns the highest keyed value within the tree
		Node<T> *last(Node<T> root) {
			if (root == NULL)
				return NULL;

			Node<T> temp = root;
			while (temp->right != NULL)
			{
				temp = temp->right;
			}
			return temp;
		}
		//

		// T next(Node<T> *root) {
		// 	Node<T> *temp;
		//
		// 	//if we are the root then we
		// 	//will return the left most child of the right node
		// 	if(root->parent==NULL)
		// 		return first(root->right);
		// 	//if the current node is the right greater than the parent then
		// 	//the next keyed node will be the leftmost node from the right child of tha node
		// 	else if(root->parent->val < root->val)
		// 		if(root->right==NULL)
		// 			return NULL;
		// 		else
		// 			return first(root->right);
		// 	//if the current nodes value is less than the value
		// 	//of its parent then the next keyed value will be the left most
		// 	//child of its sibling right child
		// 	else
		// 		return first(root->parent->right);
		//
		// }

		Node<T> *prev(Node<T> *root) {

			Node<T> *temp;

			//if we are the root then we
			//will return the left most child of the right node
			if(root->parent==NULL)
				return last(root->left);
			//if the current node is the right greater than the parent then
			//the next keyed node will be the leftmost node from the right child of tha node
			else if(root->parent->val < root->val)
				if(root->left==NULL)
					return NULL;
				else
					return last(root->left);
			//if the current nodes value is less than the value
			//of its parent then the next keyed value will be the left most
			//child of its sibling right child
			else
				return last(root->left);
		}
};

void runThread(RealRBT<int> *rbt, int id) {
	// Make a random integer in the interval [1, 100)
	int val = rand() % 100;

	// Insert it and jot down the time
	// size_t key = insertOp(rbt, val, id);
	// int *found = lookupOp(rbt, key, id);
}

int main(int argc, char **argv) {
	vector<thread*> threads;

	// Make a shared RBT of integers
	RealRBT<int> *rbt = new RealRBT<int>();

	// Random number seed
	srand(time(NULL));

	// Create threads with shared RBT and an ID
	for(int i = 0; i < TOTAL_THREADS; i++)
		threads.push_back(new thread(runThread, rbt, i));

	// Join 'em!
	for(int i = 0; i < TOTAL_THREADS; i++)
		threads[i]->join();

	// testStack();
}
