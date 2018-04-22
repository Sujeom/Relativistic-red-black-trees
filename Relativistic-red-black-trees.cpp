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
		Node *left, *right, *parent;

		Node(int _key, T *_val) {
			val = _val;
			key = _key;
			doubleB = false;
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
			

				return placeNode(root->left, newNode);
			}
			else if(root->key < key)
			{
				if(root->right == NULL) {
					return false;
				}
				else{
					root = root->right;
				}
				return placeNode(root->right, newNode);
			}
			else{//we found the node to be deleted
				
				//the case that the node to be deleted is a 
				//leaf node so it is just a simple delete
				if(root->left == NULL && root->right == NULL)
				{
					return deleteNoChild(root);
				}
				else if(deleteOneChild(root))
				{//when there is at least one child connected to the node that is going to be deleted
					return true;
				}
				else
				{//when the node to be deleted has two children
					deleteTwoChildren(root);
					return true;
				}
			}

		}

		bool deleteNoChild(Node<T> *root)
		{
			if(root->parent->left == root)
			{
						root->parent->left = null;
			}
			else 
			{
						root->parent->right = null;
			}

			return true;
		}

		bool deleteOneChild(Node<T> *root)
		{
			if(root->left == NULL && root->right != NULL)
				{
					if(root->parent->left == root)
					{
						redBlackDelete(root, root->right);
						root->parent->left = root->right;
					}
					else 
					{
						root->parent->right = root->right;
					}

					return true;
				}
				else if( root->left != NULL && root->right == NULL)
				{
					if(root->parent->left == root)
					{
						root->parent->left = root->left;
					}
					else 
					{
						root->parent->left = root->right;
					}
					return true;
				}

			return false;
		}

		bool deleteTwoChildren(Node<T> *root)
		{
			Node<T> *temp = root->right;

			while(temp->left != NULL)
				temp = temp->left;
			
			if(root->parent->left == root)
			{
						root->parent->left = temp;
						
			}
			else 
			{
						root->parent->right = temp;
			}

			temp->left = root->left;
		
			return true;
		}

		void redBlackDelete(Node<T> *root, Node<T> *replace)
		{
			
			//if the replacement is NULL it does not matter whatever we do
			//because the replacement of root will be the color black
			if(replace == NULL && root->color == RED)
			{
				//we will return because nothing needs to be done.
				return;
			}
			//if the two nodes are both different colors
			else if(root->color != replace->color)
			{			
				replace->color = BLACK;
			}
			//both of the nodes are black 
			else
			{
				twoBlackNodes(root, replace);
			}
		}

		void twoBlackNodes(Node<T> *root , Node<T> *replace)
		{
			
			//variables needed to follow the rules for 
			//a RB tree
			Node<T> *doubleBlackNode, *sibling, *p, *s, *r, *u, *childL, *childR;
			
			bool isLeft = false;

			//this is to get the palcement of the root node
			//with respect to the parent
			if(root->parent->left == root)
			{
				sibling = root->parent->right;
				isLeft = true;
			}
			else 
				sibling = root->parent->left;
				
			//if the the replace is NULL then it will be marked as a 
			//double black node
			u = replace;
			p = root->parent;
			doubleBlackNode = NULL;

			childL = sibling->left;
			childR = sibling->right;

			if(childL == NULL && childR == NULL)
			{

			}

			int rotationCase = getR(sibling, r, isLeft);



			if(rotationCase > -1)
			{

			}
			else if(rotationCase == -1)
			{//the case where both children are BLACK and the S is also black
				s->color = RED;
			}
			else if(sibling->color == BLACK && sibling->left == BLACK && sibling->right==BLACK)
			{
				s->color = RED;
			}
			else if(sibling->color == RED)
			{
				if(isLeft)
				{//this will be a left rotation

				}
				else
				{//this will be a right rotaion.

				}
			}

			
		}

		//this will get the R value and also return the 
		//type of rotation that shoud be applied
		//RR: 1 
		//RL: 2
		//LL: 3
		//LR: 4
		int getR(Node<T> *s, Node<T> *r bool isLeft)
		{
			if(s->left != NULL && s->right != NULL)
			{
				if(isLeft && s->right->color == RED)
				{//this is the RR case
					r = s->right;
					return 1;
				}	
				else if(isLeft && s->left->color == RED)
				{//thsi is the RL case
					r = s->left;
					return 2;
				}	
			}
			else if(s->left != NULL)
			{
				if(isLeft)
				{
					if(s->left->color == RED)
					{
						//this will be the LL case
						r = s->left;
						return 3;
						
					}
				}
				else
				{
					if(isLeft)
					{
						if(s->left->color == RED)
						{
							//this will be the RL case
							r = s->left;
							return 2;
							
						}

					}

				}
			}
			else if(s->right != NULL)
			{
				if(isLeft)
				{
					if(s->left->color == RED)
					{//this will be the RL case
						r = s->left;
						return 2;
					}
				}
				else
				{
					if(s->right->color == RED)
					{//this will be the RR case
						r = s->right;
						return 4;
					}

				}
			}
			else if(s->color == BLACK)//this is the case where the two children are both NULL
				return -1;

			return -2;
		}

		void lLCase(Node<T> *u, Node<T> *p, Node<T> *s, Node<T> *r)
		{

		}

		void lRCase(Node<T> u, Node<T> p, Node<T> s, Node<T> r)
		{

		}

		void rLCase(Node<T> u, Node<T> p, Node<T> s, Node<T> r)
		{

		}

		void rRCase(Node<T> u, Node<T> p, Node<T> s, Node<T> r)
		{

		}


		/*///////////////////////////////////////////////////
			PLACEHOLDER CODE FOR LATER

			//variables needed to follow the rules for 
				//a RB tree
				Node<T> doubleBlackNode, sibling, p, s, r, u;
				
				bool isLeft = false;

				//this is to get the palcement of the root node
				//with respect to the parent
				if(root->parent->left == root)
				{
					sibling = root->parent->right;
					isLeft = true;
				}
				else 
					sibling = root->parent->left;
					

				//case 1 where the replace node is NULL
				if(replace == NULL)
				{
					//if the the replace is NULL then it will be marked as a 
					//double black node
					u = replace;
					p = root->parent;
					doubleBlackNode = NULL;

					//if the root node is a left node we will then check its sibling and determin if the next step
					//will the be process of a right right delete or a right left delete
					if(isLeft)
					{
						if(sibling == NULL)
						{
							
						}
						else if(sibling == BLACK)
						{
							//this is the case for a Right right delete
							if(sibling->left != NULL && sibling->right != NULL)
							{

								//now we can determin r
								//r will be the right child because that will be the easiest to delete
								r = sibling->right;

								if(sibling->parent->right->val == sibling->val || sibling->left-> == RED && sibling->right->color == RED)
								{

								}
							}
							
							else if(sibling->left != NULL)
							{
								if(sibling->left->color == RED)
								{
								}
							}
							else
							{
								if(sibling->right->color == RED)
								{

								}
							}
						}
					}
					else 
					{
						root->parent->right = root->right;
					}

				}
				//case 2
				else if()
				{

				}










		*///////////////////////////////////
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
