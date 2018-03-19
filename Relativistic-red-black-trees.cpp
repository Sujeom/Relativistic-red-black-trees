#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#define TOTALTHREADS 5

#define BLACK 'b'
#define RED 'r'

using namespace std;
template <class T>
class RealRBT
{
	public:
		//Stack();
	class Node{
		public:
			T val;
			int key;
			char color;
			Node *left=NULL, *right=NULL;
			Node(int _key, T _val)
			{
				val = _val;
				key = _key;
				color = RED;
			}
	};

	std::atomic<Node*> root;


	atomic_int numOps{0};
	atomic_int size{0};


	RealRBT()
	{
		root = NULL;
	}

	void insert(int key, T x)
	{
		Node *newNode = new Node(key, x);

		if(root == NULL)
		{
			head = newNode;
			head->color = BLACK;
			return;
		}

		char prevColor = BLACK;

		Node *tempNodePointer = head;

		while()
		
	}

	//this function will return false if there should be a rebalancing of the tree
	bool placeNode(Node* root, Node* newNode)
	{
		if(root->key > newNode->key)
		{
			if(root->left == NULL)
			{
				root->left = newNode;
				return root == BLACK? true:false;
			}

			return placeNode(root->left, newNode);			
		}
		
		if(root->right == NULL)
		{
			root->right = newNode;
			return root == BLACK? true:false;
		}
			
		return placeNode(root->right, newNode);
	}

		
	T lookup(int key)
	{
		Node* newNode;
		Node* prevNode;

		do
		{
			
			prevNode = head;
			
			if(prevNode == NULL)
			{
				return NULL;
			}

			newNode = prevNode->next;
			
			numOps+=1;
		
		}while(!head.compare_exchange_weak(prevNode, newNode, std::memory_order_release, std::memory_order_relaxed));

		size-=1;

		return prevNode->val;
	}


	void deleteNode(int key)
	{
		return numOps;
	}

	T first()
	{

	}

	T last()
	{
		
	}

	T next()
	{
		
	}

	T prev()
	{
		
	}
};

//global variables
bool canGetSize = false, canGetNumOp = false, canPop = false, canPush = false;
bool *canGetSizeP = &canGetSize, *canGetNumOpP = &canGetNumOp, *canPopP = &canPop, *canPushP = &canPush;
int value;
int *valuePush = &value;
ConcurrentStack<int*> stack;



//Function prototypes
void menu(void);
void pushToStack();
void popFromStack(void);
void numerOpFromStack(void);
void sizeOfStack(void);
void testStack();

int main()
{
	testStack();

}

//helper function to test stack functionality
void testStack()
{
	int i;

    //initialize the myThread array
  	std::thread myThreads[TOTALTHREADS];

    //init each index of myThread
	myThreads[0] = thread(menu);
	myThreads[1] = thread(pushToStack);
	myThreads[2] = thread(popFromStack);
	myThreads[3] = thread(numerOpFromStack);
	myThreads[4] = thread(sizeOfStack);

    //detaching so the threads that control the stack operations can run idependantly
	for(i=1; i<TOTALTHREADS; i++) myThreads[i].detach();
	
    //join this thread so we wait for the menu thread to finish
    myThreads[0].join();
	
}

//menu function that will be ran by an independant thread
void menu(void)
{
	int option=1;

	cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
	
	while(option != 0)
	{
		
		cin >> option;

		switch(option)
		{
			case 0:
				canPushP = NULL;
				canPopP = NULL;
				canGetNumOpP = NULL;
				canGetSizeP = NULL;
				break;
			case 1:
				cout<<"what value would you like to push?"<<endl<<endl;
				cin >> value;
				// cout<<"pushing "<<valuePush;
				canPush = true;
				break;
			case 2:
				// cout<<"popping a value ";
				canPop = true;
				break;
			case 3:
				// cout<<"the size of the stack is ";
				canGetNumOp = true;
				break;
			case 4:
				// cout<<"the size of the stack is ";
				canGetSize = true;
				break;
			default:
				cout<<"give a valid input"<<endl<<endl;
				
		}
	}

}

//push function that will be ran by an independant thread
void pushToStack()
{
	while(canPushP != NULL)
	{
		if(*canPushP)
		{
			cout<<"pushing "<<*valuePush<<endl<<endl;
			stack.push(valuePush);
			*canPushP = false;
			cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
		}	
	}
}

//pop function that will be ran by an independant thread
void popFromStack(void)
{
	while(canPopP != NULL)
	{
		if(*canPopP)
		{
			cout<<"popped the value "<<*stack.pop()<<endl<<endl;
			*canPopP = false;
			cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
		}	
	}
}

//number of operations function that will be ran by an independant thread
void numerOpFromStack(void)
{
	while(canGetNumOpP != NULL)
	{
		if(*canGetNumOpP)
		{
			cout<<"the number of operations is "<<stack.getNumOps()<<endl<<endl;
			*canGetNumOpP = false;
			cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
		}	
	}
}

//size function that will be ran by an independant thread
void sizeOfStack(void)
{
	while(canGetSizeP != NULL)
	{
		if(*canGetSizeP)
		{
			cout<<"the size of the stack is "<<stack.getSize()<<endl<<endl;
			*canGetSizeP = false;
			cout << "options:" << endl<<"1. push a value\n2. pop a value\n3. number of operations\n4. size of the stack\n0. exit\n"<<endl;
		}	
	}
}