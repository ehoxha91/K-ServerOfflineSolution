struct Vertex
{
	int id;
	int s1pos, s2pos, s3pos, npos;
	int p_id;
	int p_s1, p_s2, p_s3, p_npos;	/* Keep track of the parent of this node. */
	double cost;
};

struct Node
{
	struct Vertex v;
	struct Node* next;
};

struct Node* front = NULL;
struct Node* rear = NULL;

struct Vertex mVertex;
int QueueIsEmpty = 0;
int vcounter;

void Push(struct Vertex _node)
{
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
	temp->v = _node;
	temp->next = NULL;
	if (front == NULL && rear == NULL)
	{
		front = rear = temp;
	}
	rear->next = temp;
	rear = temp;
	vcounter++;
}

void Pop()
{
	struct Node* temp = NULL;
	temp = front;
	if (front == NULL)
		{
			QueueIsEmpty = 1;
			printf("Queue is Empty\n");
			return;
		}
	if (front == rear)
		{
			front = rear = NULL;
			vcounter--;
		}
	else
		{
			front = front->next;
			mVertex = temp->v;
			vcounter--;
		}
	free(temp);
}

void Front()
{
	if (front == NULL)
	{
		printf("Queue is empty\n");
		return;
	}
	mVertex = front->v;
}

struct Node* GetVertexById(int _id)
{
	struct Node* temp = front;
	if (front == NULL) printf("Queue is empty!\n");
	else
	{
		while (temp!=NULL)
		{
			if (_id == temp->v.id)
				return temp;
			temp = temp->next;
		}
	}
	return temp;
}