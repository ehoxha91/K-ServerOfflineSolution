/*
   Student:    Ejup Hoxha 
   Semester:   Fall 2018 - 
   Class:      Advanced Algorithms - Dr. Peter Brass
   University: City University of New York - Groove School of Engineering
*/

struct Servers
{
	int id;
	int position;
};

struct Seg
{
	int _id;
	int _x1, _x2, _y1, _y2;
	struct Seg *next;
};

int count_seg =0;
struct Seg *current = NULL;	/* Top of the stack. */

void AddSegment(int __id, int __x1, int __y1, int __x2, int __y2)
{
	struct Seg* temp = (struct Seg*)malloc(sizeof(struct Seg));
	temp->_id = __id;
	temp->_x1 = __x1;
	temp->_y1 = __y1;
	temp->_x2 = __x2;
	temp->_y2 = __y2;
	temp->next = current;
	current = temp;
	count_seg ++;
}

struct Seg* GetSegById(int __id)
{
	struct Seg* temp =current;
	if(temp == NULL) 
	{ printf("List is empty\n"); }
	else 
	{	
		while(temp != NULL)
		{ if(__id == temp->_id)
		  { return temp; }
		 temp = temp->next;}
	}
	return temp;
}
void DeleteSegments()
{
    int iter= count_seg;
    for(int i=0; i<count_seg;i++)
    {
	struct Seg* temp = current;
	if(current!=NULL) { current = current->next;
	   free(current); count_seg--; }
    }
}

struct Pxy
{
	int x;
	int y;
};

struct Robot
{
	int r_id;
	int r_px, r_py;
	int r_height;
	int r_width;
	double distance_trv;
	//struct Robot* next_robot; we will not link this list, not needed.
};

#pragma region REQUEST POINTS
struct RequestPoint
{
	int rp_id;
	int _req_x, _req_y;
	struct RequestPoint* next_rp;
};

int request_count =0;
struct RequestPoint *requestP = NULL;

void AddRequestPoint(struct Pxy _pxy)
{
	struct RequestPoint* temp_rp = (struct RequestPoint*)malloc(sizeof(struct RequestPoint));
	temp_rp->rp_id = request_count;
	temp_rp->_req_x = _pxy.x;
	temp_rp->_req_y = _pxy.y;	
	temp_rp->next_rp = requestP;
	requestP = temp_rp;
	request_count++;
}

struct RequestPoint* GetPoint(int _pid)
{
	struct RequestPoint* temp = requestP;
	while(temp != NULL)
	{ if(_pid == temp->rp_id)
	  { return temp;} temp = temp->next_rp; 
	}
	return temp;
}

void DeleteRequestPoints()
{
	int iter = 0;
	while(requestP != NULL)
	{
		free(requestP);
		request_count--;
		requestP = requestP->next_rp;
	}
}
#pragma endregion

struct GraphNode
{
	int reqp_id;
	int rpx, rpy;
	double neigbours_weight[30];
};

struct GraphNode graphlist[30]; 

double calcostnodes(struct GraphNode _from, struct GraphNode _to)
{
	double ax = powl(_from.rpx-_to.rpx, 2);
 	double ay = powl(_from.rpy-_to.rpy, 2);
 	return sqrtl(ax+ay);
}

int nodecount = 0;
void CreateGraph()
{
	struct RequestPoint *reqpoint = NULL;
	struct RequestPoint *reqpoint2 = NULL;
	for(int i = 0; i < request_count; i++)
	{
			reqpoint = GetPoint(i);
			struct GraphNode _node;
			_node.reqp_id = reqpoint->rp_id;
			_node.rpx = reqpoint->_req_x;
			_node.rpy = reqpoint->_req_y;
			/* now fill cost matrix of this node*/
			for(int j = 0; j < request_count; j++)
			{
				/* If we found ourselves. */
				if(i==j) _node.neigbours_weight[j] = 0;
				else
				{
					reqpoint2 = GetPoint(j);
					struct GraphNode tmpnode;
					tmpnode.rpx = reqpoint2->_req_x;
					tmpnode.rpy = reqpoint2->_req_y;
					_node.neigbours_weight[j] = calcostnodes(_node, tmpnode);
				}
			}
			graphlist[i] = _node;	
			nodecount++;	
	}
}

void PrintGraph()
{
	printf("{");
	for(int i = 0; i < nodecount; i++)
	{
		struct GraphNode tempnode = graphlist[i];
		printf(",{");
		for(int j = 0; j < nodecount; j++)
		{
			printf("Node[%d]COST[%d] = %f\n",i,j,tempnode.neigbours_weight[j]);
			//printf(",%Lf",tempnode.neigbours_weight[j]);
		}
		printf("}\n");
	}
	printf("}");
	
}

