#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <assert.h>

#define TOTAL_THREADS 5
#define MAX_NUM_NODES 1000000

#define BLACK 0
#define RED 1

using namespace std;

// Node definition
template <class T>
class Node {
	public:
		// Member variables
		T *val;
		size_t key;
		bool color;
		Node *left, *right, *parent;

		Node(int _key, T *_val) {
			val = _val;
			key = _key;
			color = BLACK;
			left = NULL, right = NULL, parent = NULL;
		}
};

// RRBT definition
template <class T>
class RealRBT {
	private:
		vector<Node<T>*> nodeBank;
		atomic_int nodeBankIndex{0};

	public:
		atomic<Node<T>*> root;
		atomic_int size{0};
		mutex *lock;

		RealRBT() {
			root = NULL;
			lock = new mutex();

			for(int i = 0; i < MAX_NUM_NODES; i++)
				nodeBank.push_back(new Node<T>(0, new T()));
		}

		Node<T> *getNewNode() {
			return nodeBank[(nodeBankIndex++) % MAX_NUM_NODES];
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

		void readLock(mutex *lock) {
			lock->lock();
		}

		void readUnlock(mutex *lock) {
			lock->unlock();
		}

		T *first() {
			Node<T> *node = NULL;
			T *val = NULL;

			readLock(this->lock);

			node = leftmost(this->root);

			if(node == NULL)
				return NULL;

			val = node->val;

			readUnlock(this->lock);

			return val;
		}

		T *last() {
			Node<T> *node = NULL;
			T *val = NULL;

			readLock(this->lock);

			node = rightmost(this->root);

			if(node == NULL)
				return NULL;

			val = node->val;

			readUnlock(this->lock);

			return val;
		}

		Node<T> *next(Node<T> *currentNode)
		{
			Node<T> *currentNodex,*currentNodey;

			if ((currentNodex = *currentNode->right) != NULL) return leftmost(currentNodex);

			currentNodey = currentNode->parent;
			while (currentNodey != NULL && currentNodey->right != NULL && currentNode->key== *(currentNodey->right)->key)
			{
				currentNode = currentNodey;
				currentNodey = *currentNodey->parent;
			}

			return currentNodey;
		}

		void *previous(T *tree, long nextKey, long *key)
		{
			assert(0);
			return NULL;
		}
	}

	// This a function that will delete the node that has two children
	void interiorDelete(Node<T> *nodeToDelete)
	{
		Node<T> *cNode = next(nodeToDelete);

		Node<T> *cPrimeNode = cNode.getCopy();

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

		Node<T> *eNode = c->parent;
		//TODO crate this fuction
		/////////rpPublish(eNode->left, cNode->right);
		cNode->right->parent = eNode;

		//TODO crate this fuction
		rpFree(cNode);
	}

	// This is for the case of the special delete of the repalcement node being the child of
	// the node to be deleted
	void specialInterior(Node<T> *bNode)
	{
		Node<T> *cNode = next(bNode);

		cNode->color = bNode->color;
		cNode->left = bNode->left;
		cNode->left->parent = cNode;

		Node<T> eNode = bNode->parent;
		if(eNode->left == bNode)
			;//////rpPublish(eNode->left, cNode);
		else
			;//////rpPublish(eNode->right, cNode);

		rpFree(bNode);
	}


	// This function will handle the restucturing of the tree
	// with left diagnal
	void diagLeftRestruct(Node<T> *bNode)
	{
		Node<T> cNodePrime = cNode.getCopy();
		cNodePrime->left = bNode->right;
		cNodePrime->left->parent = cNodePrime;

		///rpPublish(bNode->right, cNodePrime);
		cNodePrime->prime = bNodePrime;

		Node<T> *dNode = cNode->parent;

		if(dNOde->left == cNode)
			;////rpPublish(dNode->left, bNode);
		else
			;//////rpPublish(dNode->right, bNode);

		bNode->parent = dNode;

		rpFree(cNode);

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
