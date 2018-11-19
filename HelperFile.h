/*
   Student:    Ejup Hoxha 
   Semester:   Fall 2018 - 
   Class:      Advanced Algorithms - Dr. Peter Brass
   University: City University of New York - Groove School of Engineering
*/


/* *************************** */
/* Segment's Stack LinkedList  */
/* *************************** */
#pragma region SEGMENT
struct Seg
{
	int _id;
	int _x1, _x2, _y1, _y2;
	struct Seg *next;
};

int count_seg =0;
struct Seg *current = NULL;	/* Top of the stack. */

/* We add each segment into the stack by calling this function. */
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

/* When we need to search for a specific segment by id. */
struct Seg* GetSegById(int __id)
{
	struct Seg* temp =current;
	if(temp == NULL) 
	{ printf("List is empty\n"); }
	else 
	{	
		while(temp != NULL) //lype segmentin me ID
		{ if(__id == temp->_id)
		  {  //printf("Segment found[%d] =(%d,%d)~(%d,%d)\n", temp->_id,temp->_x1, temp->_y1, temp->_x2, temp->_y2);
			  return temp; }
		 temp = temp->next; 
	}}
	return temp;
}

/* Delete all segments on stack. */
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
#pragma endregion

#pragma region ROBOT
/* Robot's struct. */
struct Robot
{
	int r_id;
	int r_px, r_py;
	int r_height;
	int r_width;
	double distance_trv;
	struct Robot* n_rob;
};
#pragma endregion

#pragma region ROBOT ROAD
/* 
   Robot Road. 
   This struct keeps logs of robot's
   travel route. 
*/
struct RobotRoad
{
	int line_id;
	int rob_id;
	int rx1, ry1;
	int rx2, ry2;
	struct RobotRoad* next_road;
};

int c_r_road =0;		 		 /* Number of elements. */
struct RobotRoad *r_line = NULL; /* Top of the stack.   */

/* Whenever we move we call this function to add movement to history. */
void AddRoad(int r_id, int __x1, int __y1, int __x2, int __y2)
{
	c_r_road ++;
	struct RobotRoad* temp = (struct RobotRoad*)malloc(sizeof(struct RobotRoad));
	temp->line_id =c_r_road;
	temp->rob_id = r_id;
	temp->rx1 = __x1;
	temp->ry1 = __y1;
	temp->rx2 = __x2;
	temp->ry2 = __y2;
	temp->next_road = r_line;
	r_line = temp;	
}

/* Get a specific segment of the robot's road by id. */
struct RobotRoad* GetRoad(int l_id)
{
	struct RobotRoad* temp =r_line;
	if(temp == NULL) 
	{ printf("List is empty\n");}
	else 
	{	while(temp != NULL)
		{  if(l_id == temp->line_id)
		   { return temp;}
		   temp = temp->next_road;
		}
	}
	return temp;
}

/* Delete robot's path. */
void DeleteRoad()
{
	struct RobotRoad* tmp = r_line;
	int iter= 0;
	for(int i=0; i<c_r_road;i++)
	{ if(r_line!=NULL)
	  {
	    r_line = r_line->next_road;
	    free(r_line); r_line--;
          }
	}
}


struct req_point
{
	int rp_id;
	int _req_x, _req_y;
	struct req_point* next_rp;
};


int req_id =0;
struct req_point *last_in = NULL;

/* Every time we request a new position it's added to this linked list, so we have complete history of the movements. */
void AddRequestPoint(int _xpos, int _ypos)
{
	struct req_point* temp_rp = (struct req_point*)malloc(sizeof(struct req_point));
	temp_rp->rp_id = req_id;
	temp_rp->_req_x = _xpos;
	temp_rp->_req_y = _ypos;	
	temp_rp->next_rp = last_in;
	last_in = temp_rp;
	req_id ++;
}

struct req_point* GetPoint(int _pid)
{
	struct req_point* temp = last_in;
	while(temp != NULL)
	{ if(_pid == temp->rp_id)
	  { return temp;} temp = temp->next_rp; 
	}
	return temp;
}






