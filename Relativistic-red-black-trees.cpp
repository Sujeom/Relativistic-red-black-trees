#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
// #include <queue>

#define TOTAL_THREADS 5
#define MAX_NUM_NODES 10000

#define BLACK 0
#define RED 1
#define DOUBLEBLACK 2

using namespace std;

// Node definition
template <class T>
class Node {
	public:
		// Member variables
		T *val;
		int key;
		bool doubleB;
		int color;
		long index;
		Node *left, *right, *parent;

		Node(int _key, T *_val) {
			val = _val;
			key = _key;
			doubleB = false;
			color = BLACK;
			index = _index;
			left = NULL, right = NULL, parent = NULL;
		}
w
		//the node copy function needed for the 
		//opereations needed withing the paper
		//DO I GET IT FROM THE NODE BANK???????
		//BLUWWWAAAAAA
		Node<T>* getCopy()
		{
			Node<T> *n = new Node<T>(key, val);
			
			memcpy(n, *this, sizeof(Node<T>));

			return n;
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
		atomic_int numOps{0};
		atomic_int size{0};

		RealRBT() {
			root = NULL;

			for(int i = 0; i < MAX_NUM_NODES; i++)
				nodeBank.push_back(new Node<T>(0,  new T()));
		}

		Node<T> *getNewNode() {
			return nodeBank[(nodeBankIndex++) % MAX_NUM_NODES];
		}

		void insert(T x) {
			size_t key = hash<T>{}(x);
			Node<T> *newNode = getNewNode();

			newNode->key = key;
			*(newNode->val) = x;

			//
			// if(root == NULL) {
			// 	head = newNode;
			// 	head->color = BLACK;
			// 	return;
			// }
			//
			// if(placeNode(root, newNode)) {
			//
			// }
			// else {
			//
			// }
			//
			// while();

		}

		// this function will return false if the parent is red and true if the parent is black
		bool placeNode(Node<T> *root, Node<T> *newNode) {
			if(root->key > newNode->key) {
				if(root->left == NULL) {
					root->left = newNode;
					newNode->parent = root;
					return checkUncle(root);
				}

				return placeNode(root->left, newNode);
			}

			if(root->right == NULL) {
				root->right = newNode;
				newNode->parent = root;
				return root == BLACK? true:false;
			}

			return placeNode(root->right, newNode);
		}

		bool checkUncle(Node<T> *parent) {
			if(parent->parent == NULL) {
				return false;
			}

			if(parent->parent->left != NULL) {
				if(parent->parent->left->color == BLACK)
					return false;

				return true;
			}

			if(parent->parent->right != NULL) {
				if(parent->parent->right->color == BLACK)
					return false;

				return true;
			}

			return false;
		}


		T lookup(int key) {
			Node<T> *newNode;
			Node<T> *prevNode;

			do {
				prevNode = root;

				if(prevNode == NULL) {
					return NULL;
				}

				newNode = prevNode->next;

				numOps += 1;

			} while(!root.compare_exchange_weak(prevNode, newNode, std::memory_order_release, std::memory_order_relaxed));

			size -= 1;

			return prevNode->val;
		}

		bool deleteN(Node<T> *root, int key) {
			if(root->key > key) {
				if(root->left == NULL) {
					return false;
				}
				else{
					root = root->left;
					return deleteN(root, key);
				}
			}
			else if(root->key < key)
			{
				if(root->right == NULL) {
					return false;
				}
				else{
					root = root->right;
				}
			}
			else{//we found the node to be deleted
					return oneChild(root);
				}
		}

		

		
	Node<T>* getParent(Node<T>* root) {
		
		return root->parent;
	
	}

	Node<T>* getGrandparent(Node<T>* root) {
		
		Node<T>* p = parent(root);
		
		if (p == NULL)
			return NULL; // No parent means no grandparent
		
		return parent(p);
	}

	Node<T>* getSibling(Node<T>* root) {
		
		Node<T>* p = parent(root);
		
		if (p == NULL)
			return NULL; // No parent means no sibling
		
		if (root == p->left)
			return p->right;
		else
			return p->left;
	}

	Node<T>* getUncle(Node<T>* root) {
		
		Node<T>* p = parent(root);
		Node<T>* g = grandparent(root);
		
		if (g == NULL)
			return NULL; // No grandparent means no uncle
		
		return sibling(p);
	}

	void rotate_left(Node<T>* root) 
	{
		Node<T>* temp = root->right;
		
		assert(temp->left != NULL && temp->right != NULL); 

		root->right = temp->left;
		temp->left = root;
		temp->parent = root->parent;
		root->parent = temp;
		// (the other related parent and child links would also have to be updated)
	
	}

	void rotate_right(Node<T>* root)
	{
		Node<T>* temp = root->left;
		
		assert(temp->left != NULL && temp->right != NULL); // since the leaves of a red-black tree are empty, they cannot become internal nodes
		
		root->left = temp->right;
		temp->right = root;
		temp->parent = root->parent;
		root->parent = temp;
		// (the other related parent and child links would also have to be updated)
	}

	void replace(Node<T> *root, Node<T> *child)
	{
		child->parent = n->parent;

		if(n == n->parent->left)
			child->parent->left = child;
		else
			child->parent->right = child;
	}

	bool onChild(Node<T> *root)
	{
		Node<T>* temp = root->right == NULL && root->left == NULL? n->left:n->right;

		replace(root, temp);

		if(root->color == BLACK)
		{
			if(temp->color == RED)
				temp->color = BLACK;
			else
				dCase1(temp);
		}
		//free

		return true;
	}

	//when the root node is the head of hte tree
	//
	void dCase1(Node<T> *root)
	{
		if(root->pareant != NULL)
			dCase2(root);
	}

	void dCase2(Node<T> *root)
	{
		Node<T> *sibling = getSibling(root);

		if(sibling->color == RED)
		{
			root->parent->color = RED;
			sibling->color = BLACK;

			if(root==root->parent->left)
			{
				rotate_left(root->parent);

			}
			else
				rotate_right(root->parent;)
		}

		dCase3(root);
	}


	void dCase3(Node<T> *root)
	{
		Node<T> *sibling = getSibling(root);

		if(n->parente->color == BLACK && sibling->color == BLACK && sibling->left->color == BLACK && sibling->right->color == BLACK)
		{
			sibling->color == RED;
			dCase1(root->parent)
		}
		else
			dCase4(root);
	}

	void dCase4(Node<T> *root)
	{
		Node<T> *sibling = getSibling(root);

		if(root->parent->color == RED && sibling->color == BLACK && sibling->left->color == BLACK && sibling->right->color == BLACK)
		{
			sibling->color = RED;
			root->parent->color = BLACK;
		}
		else 
			dCase5(root);
	}

	void dCase6(Node<T> *root)
	{
		Node<T> *sibling = getSibling(root);

		sibling->color = root->parent->color;
		root->parent->color = BLACK;

		if(root == root->paent->left)
		{
			sibling->right->color = BLACK;
			rotate_left(root->parent);
		}
		else
		{
			sibling->left->color = BLACK;
			rotate_right(root->parent);
		}
	}

	//this a functtion that will delete the nod ethat has two children
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

	//this is for the case of the special delete of the repalcement node being the child of 
	//the node to be deleted
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
	

	//thsi function will handle the restucturing of the tree
	//with left diagnal
	void diagLeftRestruct(Node<T> *bNode)
	{
		Node<T> cNodePrime = cNode.getCopy();
		cNodePrime->left = bNode->right;
		cNodePrime->left->parent = cNodePrime;

		///rpPublish(bNode->right, cNodePrime);
		cNodePrime->parent = bNode;

		dNode = cNode->parent;

		if(dNOde->left == cNode)
			;////rpPublish(dNode->left, bNode);
		else
			;//////rpPublish(dNode->right, bNode);

		bNode->parent = dNode;

		rpFree(cNode);

	}

	void diagRightRestruct(Node<T> *bNode)
	{
		Node<T> cNodePrime = cNode.getCopy();
		cNodePrime->right = bNode->left;
		cNodePrime->right->parent = cNodePrime;

		///rpPublish(bNode->right, cNodePrime);
		cNodePrime->parent = bNode;

		dNode = cNode->parent;

		if(dNOde->right == cNode)
			;////rpPublish(dNode->right, bNode);
		else
			;//////rpPublish(dNode->left, bNode);

		bNode->parent = dNode;

		rpFree(cNode);

	}

	void zigLeftRestruct(Node<T> *bNode)
	{
		Node<T> *aPrime = aNode.getCopy();
		aPrime->right = B->left;
		aPrime->right->parent = aPrime;

		//rpPublish(B->left, aPrime);
		aPrime->parent = B;

		cPrime = C.getCopy();
		cPrime->left = B->right;
		cPrime->left->parent = cPrime;

		rpPublish(B->right, cPrime);
		cPrime->parent = B;

		D = C->parent;

		if(D->left == C)
			;//rpPubllish(D->left, B);
		else
			;//rpPublish(D->right, B);

		rpFree(A);
		rpFree(C);
		
	}

	void zigRightRestruct(Node<T> *bNode)
	{
		Node<T> *aPrime = aNode.getCopy();
		aPrime->left = B->right;
		aPrime->left->parent = aPrime;

		//rpPublish(B->left, aPrime);
		aPrime->parent = B;

		cPrime = C.getCopy();
		cPrime->right = B->left;
		cPrime->right->parent = cPrime;

		rpPublish(B->left, cPrime);
		cPrime->parent = B;

		D = C->parent;

		if(D->right == C)
			;//rpPubllish(D->right, B);
		else
			;//rpPublish(D->left, B);

		rpFree(A);
		rpFree(C);
		
	}

	void rpFree(/*void *lock, */void (*func)(void *ptr), void *ptr) 
	{
		rp_lock_t *rp_lock = (rp_lock_t *)lock;
    	int head;


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

// global variables
// bool canGetSize = false, canGetNumOp = false, canPop = false, canPush = false;
// bool *canGetSizeP = &canGetSize, *canGetNumOpP = &canGetNumOp, *canPopP = &canPop, *canPushP = &canPush;
// int value;
// int *valuePush = &value;
// stack<int*> stack;

//menu function that will be ran by an independant thread
// void menu(void) {
// 	int option = 1;
//
// 	cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
//
// 	while(option != 0) {
//
// 		cin >> option;
//
// 		switch(option) {
// 			case 0:
// 				canPushP = NULL;
// 				canPopP = NULL;
// 				canGetNumOpP = NULL;
// 				canGetSizeP = NULL;
// 				break;
// 			case 1:
// 				cout<<"what value would you like to push?"<<endl<<endl;
// 				cin >> value;
// 				// cout<<"pushing "<<valuePush;
// 				canPush = true;
// 				break;
// 			case 2:
// 				// cout<<"popping a value ";
// 				canPop = true;
// 				break;
// 			case 3:
// 				// cout<<"the size of the stack is ";
// 				canGetNumOp = true;
// 				break;
// 			case 4:
// 				// cout<<"the size of the stack is ";
// 				canGetSize = true;
// 				break;
// 			default:
// 				cout<<"give a valid input"<<endl<<endl;
//
// 		}
// 	}
//
// }
//
// // push function that will be ran by an independant thread
// void pushToStack(void) {
// 	while(canPushP != NULL) {
// 		if(*canPushP) {
// 			cout<<"pushing "<<*valuePush<<endl<<endl;
// 			stack.push(valuePush);
// 			*canPushP = false;
// 			cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
// 		}
// 	}
// }
//
// //pop function that will be ran by an independant thread
// void popFromStack(void) {
// 	while(canPopP != NULL) {
// 		if(*canPopP) {
// 			cout<<"popped the value "<<*stack.pop()<<endl<<endl;
// 			*canPopP = false;
// 			cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
// 		}
// 	}
// }
//
// //number of operations function that will be ran by an independant thread
// void numerOpFromStack(void) {
// 	while(canGetNumOpP != NULL) {
// 		if(*canGetNumOpP) {
// 			cout<<"the number of operations is "<<stack.getNumOps()<<endl<<endl;
// 			*canGetNumOpP = false;
// 			cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
// 		}
// 	}
// }
//
// //size function that will be ran by an independant thread
// void sizeOfStack(void) {
// 	while(canGetSizeP != NULL) {
// 		if(*canGetSizeP) {
// 			cout<<"the size of the stack is "<<stack.getSize()<<endl<<endl;
// 			*canGetSizeP = false;
// 			cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
// 		}
// 	}
// }
//
// //helper function to test stack functionality
// void testStack() {
//   // initialize the myThread array
// 	thread myThreads[TOTAL_THREADS];
//
//   // init each index of myThread
// 	myThreads[0] = thread(menu);
// 	myThreads[1] = thread(pushToStack);
// 	myThreads[2] = thread(popFromStack);
// 	myThreads[3] = thread(numerOpFromStack);
// 	myThreads[4] = thread(sizeOfStack);
//
//     //detaching so the threads that control the stack operations can run idependantly
// 	for(int i = 1; i < TOTALTHREADS; i++)
// 		myThreads[i].detach();
//
//   //join this thread so we wait for the menu thread to finish
//   myThreads[0].join();
//
// }

void runThread(RealRBT<int> *rbt, int id) {
	// Make a random integer in the interval [1, 100)
	int val = rand() % 100;

	// Insert it and jot down the time
	rbt->insert(val);
	long time = clock();

	// Strings print out weird concurrently. Build them first then print them out.
	string p_out = "Thread " + to_string(id) + " inserted node with value " + to_string(val) + " at " + to_string(time) + "ms.\n";
	cout << p_out;
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
