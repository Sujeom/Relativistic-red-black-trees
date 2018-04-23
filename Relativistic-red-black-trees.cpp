#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <assert.h>

#define TOTAL_THREADS 5
#define MAX_NUM_NODES 1000000
#define NUM_READERS 10
#define EPOCH 200

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
		Node *backup;

		Node(int _key, T *_val) {
			val = _val;
			key = _key;
			color = BLACK;
			left = NULL, right = NULL, parent = NULL;
			backup = new Node<T>(0, new T());
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

			for(int i = 0; i < MAX_NUM_NODES; i++)
				nodeBank.push_back(new Node<T>(0, new T()));
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
		Node<T> *bstInsert(Node<T> *newNode) {
			if(newNode == NULL)
				return this->root;

			Node<T> *temp = this->root;

			while(temp != NULL) {

				// If newNode's key is too large, it belongs on the right
				if(newNode->key >= temp->key)
					temp = temp->right;
				else
					temp = temp->left;
			}

			if(newNode->key >= temp->parent->key)
				temp->right = newNode;
			else
				temp->left = newNode;

			return newNode;
		}

		void insert(int key, T *val) {
			Node<T> *newNode = getNewNode();
			newNode->val = val;
			newNode->key = key;

			newNode = bstInsert(newNode);
			newNode->color = RED;

			if(this->root == newNode) {
				newNode->color = BLACK;
				return;
			}

			// TODO: Complete insert -> how to check if restructures/recolors are
			// needed?
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
		void diagLeftRestruct(Node<T> *cNode) {
			Node<T> *cNodePrime = cNode->getCopy();
			Node<T> *bNode = cNode->left;

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

		void diagRightRestruct(Node<T> *cNode) {
			Node<T> *cNodePrime = cNode->getCopy();
			Node<T> *bNode = cNode->right;

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

		void zigLeftRestruct(Node<T> *cNode) {
			Node<T> *aNode = cNode->left;
			Node<T> *bNode = aNode->right;
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

		void zigRightRestruct(Node<T> *cNode) {
			Node<T> *aNode = cNode->right;
			Node<T> *bNode = aNode->left;
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
		//
		// T first() {
		//
		// }
		//
		// T last() {
		//
		// }
		//
		// T next() {
		//
		// }
		//
		// T prev() {
		//
		// }

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
