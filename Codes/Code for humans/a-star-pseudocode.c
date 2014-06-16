#include <stdio.h>
#include <math.h>
#define maze_length 7
#define maze_width 7

// Declaration of variables
int is_at_end;
int parent_of[maze_length][maze_width],G[maze_length][maze_width],H[maze_length][maze_width],F[maze_length][maze_width];
int on_optimal_path[maze_length][maze_width];
int path[maze_length][maze_width][maze_length][maze_width];
int parent_of[maze_length][maze_width];
int is_open[maze_length][maze_width];

// Initialization of variables
void init(){
	int i,j,k,l;
	
	for(i=0;i<maze_length;i++){
	
		for(j=0;j<maze_width;j++){
		
			for(k=0;k<maze_length;k++){
			
				for(l=0;l<maze_width;l++){
					path[i][j][k][l] = 0;
				}
			}
		}
	}
	
	for(i=0;i<maze_length;i++){
	
		for(j=0;j<maze_width;j++){
			parent_of[i][j] = -1;
			on_optimal_path[i][j] = 0;
			is_open[i][j] = 0;
			G[i][j] = maze_length*maze_length*maze_width*maze_width;
		}
		
	}
	
	parent_of[0][0] = 0;
}

// Moves the bot from specified start to specified destination
void moveto(int arg1,int arg2,int arg3,int arg4){ // Should edit moveto function
	
	if(arg1 == arg3){
	
		if(arg4>arg2){
			//move forward;
		}
		
		else{
			//turn back;
			//move forward;
		}
	}
	
	else if(arg2 == arg4){
	
		if(arg3>arg1){
			//turn left;
			//move forward;
		}
		
		else{
			//turn right;
			//move forward;
		}
	}
	
}

// Moves the bot from specified start to specified destination
void moveto_far(int arg1,int arg2,int arg3,int arg4){
	int parent_x_1,parent_x_2,parent_y_1,parent_y_2;
	
	if(!((arg1==arg3&&fabs(arg2-arg4)==1)||(arg2==arg4&&fabs(arg1-arg3)==1))){
		parent_x_1 = parent_of[arg1][arg2]/10;
		parent_y_1 = parent_of[arg1][arg2]%10;
		parent_x_2 = parent_of[arg3][arg4]/10;
		parent_y_2 = parent_of[arg3][arg4]%10;
		moveto(arg1,arg2,parent_x_1,parent_y_1);
		moveto_far(parent_x_1,parent_y_1,parent_x_2,parent_y_2);
		moveto(parent_x_2,parent_y_2,arg3,arg4);
	}
	
	else moveto(arg1,arg2,arg3,arg4);
}

// Returns minimum of given variables
int min(int arg1,int arg2,int arg3,int arg4){
	int minimum;
	minimum = arg1;
	
	if(minimum<arg2) minimum = arg2;
	if(minimum<arg3) minimum = arg3;
	if(minimum<arg4) minimum = arg4;
	
	return minimum;
}

// Checks whether the node has a child with outward path or not
int no_child_with_outpath(int arg1,int arg2){
	int F1,F2,F3,F4;
	int flag = 0;
	
	F1 = maze_length*maze_length*maze_width*maze_width;
	F2 = maze_length*maze_length*maze_width*maze_width;
	F3 = maze_length*maze_length*maze_width*maze_width;
	F4 = maze_length*maze_length*maze_width*maze_width;
	
	if(arg1+1<=maze_length-1 && path[arg1][arg2][arg1+1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1+1) + arg2 && G[arg1][arg2]+1<G[arg1+1][arg2]){
        F1 = F[arg1 + 1][arg2];
    }
	
    if(arg1-1>=0 && path[arg1][arg2][arg1-1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1-1) + arg2 && G[arg1][arg2]+1<G[arg1-1][arg2]){
        F2 = F[arg1 - 1][arg2];
    }
	
    if(arg2+1<=maze_width-1 && path[arg1][arg2][arg1][arg2+1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2+1 && G[arg1][arg2]+1<G[arg1][arg2+1]){
        F3 = F[arg1][arg2 + 1];
    }
	
    if(arg2-1>=0 && path[arg1][arg2][arg1][arg2-1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2-1 && G[arg1][arg2]+1<G[arg1][arg2-1]){
        F4 = F[arg1][arg2 - 1];
    }
	
	if(F1 == maze_length*maze_length*maze_width*maze_width && F2 == maze_length*maze_length*maze_width*maze_width && 
		F3 == maze_length*maze_length*maze_width*maze_width && F4 == maze_length*maze_length*maze_width*maze_width) flag++;// Checking F values
		
	return flag;
}

// Checking whether the node has outward paths or not
int no_out_path_from(int arg1,int arg2){
	int child_x,child_y;
	int flag = 0;
	int turned_back = 0;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length){
		// turn left
		// move forward
		if(!turned_back) flag++;
	}
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x > 0){
		// turn right
		// move forward
		if(!turned_back) flag++;
	}
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_y < maze_width){
		// move forward
		if(!turned_back) flag++;
	}
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_y > 0){
		// turn back
		// move forward
		if(!turned_back) flag++;
	}
}

// Whether a given child is reachable or not
void check_reachability_of_children_of(int arg1,int arg2){
	int child_x,child_y;
	int turned_back = 0,node_reached = 0;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length){
		// turn left
		// move forward
		if(node_reached) path[arg1][arg2][child_x][child_y] = 1;
		if(turned_back) path[arg1][arg2][child_x][child_y] = 0;
	}
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x > 0){
		// turn right
		// move forward
		if(node_reached) path[arg1][arg2][child_x][child_y] = 1;
		if(turned_back) path[arg1][arg2][child_x][child_y] = 0;
	}
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_y < maze_width){
		// move forward
		if(node_reached) path[arg1][arg2][child_x][child_y] = 1;
		if(turned_back) path[arg1][arg2][child_x][child_y] = 0;
	}
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_y > 0){
		// turn back
		// move forward
		if(node_reached) path[arg1][arg2][child_x][child_y] = 1;
		if(turned_back) path[arg1][arg2][child_x][child_y] = 0;
	}
}

// Getting coordinates for next node on the optimal path
int child_value(int arg1,int arg2){
	int F1,F2,F3,F4;
	int flag = 0;
	int retval;
	
	F1 = maze_length*maze_length*maze_width*maze_width;
	F2 = maze_length*maze_length*maze_width*maze_width;
	F3 = maze_length*maze_length*maze_width*maze_width;
	F4 = maze_length*maze_length*maze_width*maze_width;
	
	if(arg1+1<=maze_length-1 && path[arg1][arg2][arg1+1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1+1) + arg2 && G[arg1][arg2]+1<G[arg1+1][arg2]){
        F1 = F[arg1 + 1][arg2];
    }
	
    if(arg1-1>=0 && path[arg1][arg2][arg1-1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1-1) + arg2 && G[arg1][arg2]+1<G[arg1-1][arg2]){
        F2 = F[arg1 - 1][arg2];
    }
	
    if(arg2+1<=maze_width-1 && path[arg1][arg2][arg1][arg2+1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2+1 && G[arg1][arg2]+1<G[arg1][arg2+1]){
        F3 = F[arg1][arg2 + 1];
    }
	
    if(arg2-1>=0 && path[arg1][arg2][arg1][arg2-1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2-1 && G[arg1][arg2]+1<G[arg1][arg2-1]){
        F4 = F[arg1][arg2 - 1];
    }
	
	if(min(F1,F2,F3,F4)==F1) retval = 10*(arg1+1) + arg2;
	else if(min(F1,F2,F3,F4)==F2) retval = 10*(arg1-1) + arg2;
	else if(min(F1,F2,F3,F4)==F3) retval = 10*arg1 + arg2 + 1;
	else if(min(F1,F2,F3,F4)==F4) retval = 10*arg1 + arg2 - 1;
	
	return retval;
}

// Add all the children to the open list
void expand_children(int arg1,int arg2){
	int F1,F2,F3,F4;
	
	F1 = maze_length*maze_length*maze_width*maze_width;
	F2 = maze_length*maze_length*maze_width*maze_width;
	F3 = maze_length*maze_length*maze_width*maze_width;
	F4 = maze_length*maze_length*maze_width*maze_width;
	
	check_reachability_of_children_of(arg1,arg2);
	
	if(arg1+1<=maze_length-1 && path[arg1][arg2][arg1+1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1+1) + arg2 && G[arg1][arg2]+1<G[arg1+1][arg2]){
        G[arg1+1][arg2] = G[arg1][arg2]+1;
        F[arg1+1][arg2] = G[arg1+1][arg2] + H[arg1+1][arg2];
		F1 = F[arg1+1][arg2];
		if(H[arg1+1][arg2]==0) is_at_end++;
        parent_of[arg1+1][arg2] = 10*arg1 + arg2;
		is_open[arg1+1][arg2] = 1;
    }
	
    if(arg1-1>=0 && path[arg1][arg2][arg1-1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1-1) + arg2 && G[arg1][arg2]+1<G[arg1-1][arg2]){
        G[arg1-1][arg2] = G[arg1][arg2]+1;
        F[arg1-1][arg2] = G[arg1-1][arg2] + H[arg1-1][arg2];
		F2 = F[arg1-1][arg2];
		if(H[arg1 - 1][arg2]==0) is_at_end++;
        parent_of[arg1-1][arg2] = 10*arg1 + arg2;
		is_open[arg1-1][arg2] = 1;
    }
	
    if(arg2+1<=maze_width-1 && path[arg1][arg2][arg1][arg2+1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2+1 && G[arg1][arg2]+1<G[arg1][arg2+1]){
        G[arg1][arg2+1] = G[arg1][arg2]+1;
        F[arg1][arg2+1] = G[arg1][arg2+1] + H[arg1][arg2+1];
		F3 = F[arg1][arg2+1];
		if(H[arg1][arg2+1]==0) is_at_end++; 
        parent_of[arg1][arg2+1] = 10*arg1 + arg2;
		is_open[arg1][arg2+1] = 1;
    }
	
    if(arg2-1>=0 && path[arg1][arg2][arg1][arg2-1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2-1 && G[arg1][arg2]+1<G[arg1][arg2-1]){
        G[arg1][arg2-1] = G[arg1][arg2]+1;
        F[arg1][arg2-1] = G[arg1][arg2-1] + H[arg1][arg2-1];
		F4 = F[arg1][arg2-1];
		if(H[arg1][arg2-1]==0) is_at_end++;
        parent_of[arg1][arg2-1] = 10*arg1 + arg2;
		is_open[arg1][arg2-1] = 1;
    }
	
	// if(F1 == min(F1,F2,F3,F4)){
		// moveto(arg1,arg2,arg1+1,arg2);
		// on_optimal_path[arg1+1][arg2] = 1;
	// }//simply the bot must move left by one cell
	
	// else if(min(F1,F2,F3,F4) == F2){
		// moveto(arg1,arg2,arg1-1,arg2);
		// on_optimal_path[arg1-1][arg2] = 1;
	// }//simply the bot must move right by one cell
	
	// else if(F3 == min(F1,F2,F3,F4)){
		// moveto(arg1,arg2,arg1,arg2+1);
		// on_optimal_path[arg1][arg2+1] = 1;
	// }//simply the bot must move forward by one cell
	
	// else if(F4 == min(F1,F2,F3,F4)){
		// moveto(arg1,arg2,arg1,arg2-1);
		// on_optimal_path[arg1][arg2-1] = 1;
	// }//simply the bot must move backward by one cell
}

// Give the coordinates of the successor
int successor_value(int arg1,int arg2){
	int min = 0;
	int destination_x,destination_y;
	int retval,i,j;
	
	for(i=0;i<maze_length;i++){
	
		for(j=0;j<maze_width;j++){
		
			if(is_open[i][j]==1 && min<F[i][j]){
				destination_x = i;
				destination_y = j;
			}
		}
	}
	
	retval = 10*destination_x + destination_y;
	
	return retval;
}

// Go to the correct node of the open list
void goto_correct_successor(int arg1,int arg2){
	int min = 0;
	int destination_x,destination_y,i,j;
	
	for(i=0;i<maze_length;i++){
	
		for(j=0;j<maze_width;j++){
		
			if(is_open[i][j]==1 && min<F[i][j]){
				destination_x = i;
				destination_y = j;
			}
		}
	}
	
	moveto_far(arg1,arg2,destination_x,destination_y);
}

// Go to the parent of the specified node
void goto_parent_of(int arg1,int arg2){
	int current_x,current_y;
	int parent_x,parent_y,parent;
	
	current_x = arg1;
	current_y = arg2;
	parent = parent_of[current_x][current_y];
	parent_x = parent/10;
	parent_y = parent%10;
	on_optimal_path[current_x][current_y] = 0;
	
	moveto(current_x,current_y,parent_x,parent_y);
}

// Go to the next node in the optimal path
void goto_next_node(int arg1,int arg2){
	int child_x,child_y,mod = 0;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1 && on_optimal_path[child_x][child_y]==1 && mod==0){
		moveto(arg1,arg2,child_x,child_y);
		if(H[child_x][child_y]==0) is_at_end++;
		mod++;
	}
		
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x >= 0 && path[arg1][arg2][child_x][child_y]==1 && on_optimal_path[child_x][child_y]==1 && mod==0){
		moveto(arg1,arg2,child_x,child_y);
		if(H[child_x][child_y]==0) is_at_end++;
		mod++;
	}
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_y < maze_length && path[arg1][arg2][child_x][child_y]==1 && on_optimal_path[child_x][child_y]==1 && mod==0){
		moveto(arg1,arg2,child_x,child_y);
		if(H[child_x][child_y]==0) is_at_end++;
		mod++;
	}
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_y >= 0 && path[arg1][arg2][child_x][child_y]==1 && on_optimal_path[child_x][child_y]==1 && mod ==0){
		moveto(arg1,arg2,child_x,child_y);
		if(H[child_x][child_y]==0) is_at_end++;
		mod++;
	}
}

// Exploring the maze using A-star
void a_star_exploring(int arg1,int arg2){
	int current_x=arg1,current_y=arg2;
	int successor_x,successor_y,parent_x,parent_y;
	is_at_end = 0;
	init();

	while(!is_at_end){
	
			if(no_out_path_from(current_x,current_y) || no_child_with_outpath(current_x,current_y)){
				goto_parent_of(current_x,current_y);
				F[current_x][current_y] = maze_length*maze_length*maze_width*maze_width;
				parent_x = parent_of[current_x][current_y]/10;
				parent_y = parent_of[current_x][current_y]%10;
				a_star_exploring(parent_x,parent_y);
			}
			
			expand_children(current_x,current_y);
			goto_correct_successor(current_x,current_y);
			successor_x = successor_value(current_x,current_y)/10;
			successor_y = successor_value(current_x,current_y)%10;
			is_open[successor_x][successor_y] = 0;
			on_optimal_path[successor_x][successor_y] = 1;
			a_star_exploring(successor_x,successor_y);
	}
	
	if(is_at_end) printf("Done Exploring! Lets solve it!");
	
}

// Solving the maze using A-star
void a_star_solving(int arg1,int arg2){
	int current_x = arg1,current_y = arg2;
	int child_x,child_y;
	is_at_end = 0;
	
	while(!is_at_end){
		goto_next_node(arg1,arg2);
		child_x = child_value(current_x,current_y)/10;
		child_y = child_value(current_x,current_y)%10;
		a_star_solving(child_x,child_y);
	}
	
	if(is_at_end) printf("Solved it! At end!");
}

// Main program for exploring and solving the maze
int main(){
	int i = 0,j = 0;
	
	a_star_exploring(i,j);
	//_delay_ms(20000); // Introducing delay for setting the position of the bot for second run
	a_star_solving(i,j);
	
	return 0;
}