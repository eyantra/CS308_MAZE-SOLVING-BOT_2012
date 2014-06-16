#include "astar.cpp"
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <utility>
#include <libplayerc++/playerc++.h>
#include "bfs.cpp"
float X=-4.5,Y=-4.5;
int A=0;

#define create(x,y)   puzzle_mn *p =new puzzle_mn(x,y);
#define set_next neighbours.push_back(pair<node<int,string>*,int>(p,1))
#define rot90    p2dProxy.SetSpeed(0, dtor(90)); usleep(1000000);p2dProxy.SetSpeed(0, dtor(0));usleep(10000);botDirection=(botDirection+1)%4;A=(A+90)%360;\
sim.SetPose2d(name,X,Y,A*1.0*3.1415/180);

#define movef    p2dProxy.SetSpeed(1, dtor(0)); usleep(1000000);p2dProxy.SetSpeed(0, dtor(0));usleep(10000);\
if(botDirection==0)X++;\
if(botDirection==1)Y++;\
if(botDirection==2)X--;\
if(botDirection==3)Y--;\
sim.SetPose2d(name,X,Y,A*1.0*3.1415/180);




int botDirection=0;
char name[]="bob1";
PlayerClient    robot("localhost", 6665);

Position2dProxy p2dProxy(&robot,0);
SonarProxy      sonarProxy(&robot,0);
SimulationProxy sim(&robot,0) ;  
//sim.SetPose2d("bob1",X,Y,A);  
double forwardSpeed, turnSpeed;

      
    
typedef pair<int,int> _pair;
class puzzle_mn : public node<int, string>
{
public:

  static _pair goal;
  _pair config;

  void setkey(){
    key="";
    char c[10];
    sprintf(c,"%d:%d",config.first,config.second); key+=string(c);
  }
  
  puzzle_mn(_pair _start,_pair _end)
  {
    goal=_end;
    config=_start; 
    setkey();
    h = heuristic();
    g = INT_MAX;
  }

  puzzle_mn(int x,int y)
  { 
    config.first=x;config.second=y;
    setkey();
    h = heuristic();
    g = INT_MAX;
  }
  


  int heuristic(){
#if ZERO
    return 0;
#endif
    return manhattan();
  }
  int manhattan(){
    return abs(config.first-goal.first)+abs(config.second-goal.second);
  }
    
      
  bool
  isgoal()
  {
    if(config.first==goal.first && config.second==goal.second){
      return true;
    }
    return false;
  }
  

  typedef pair<node<int, string>*,int> edge;
  void
  expand()
  {


    //request geometries
    p2dProxy.RequestGeom();
    sonarProxy.RequestGeom();
    //blobfinder doesn't have geometry
	
    /*here so that laserProxy[90] doesn't segfault on first loop*/
    
 //   printf("position %f %f \n",p2dProxy.GetXPos(),p2dProxy.GetYPos());
 printf("aim to go %d %d\n",config.first,config.second);
 printf("path is called\n");
    path(p2dProxy.GetXPos()+.5,p2dProxy.GetYPos()+.5,config.first,config.second);
 printf("path is returned\n");
    //path(0,0,2,0);
    
    for(int i=Path.size()-2;i>0;i--){
      printf("path %d %d \n",Path[i]->first,Path[i]->second);
      robot.Read();
      int x=p2dProxy.GetXPos()+.5;
      int y=p2dProxy.GetYPos()+.5;
      if(Path[i]->first-x==1){
      cout<<"came1"<<endl;
      while(botDirection!=0){rot90;}
      movef;
      }
      else if(Path[i]->first-x==-1){
      
      cout<<"came2"<<endl;
      while(botDirection!=2){rot90;}
      movef;
      }
      else if(Path[i]->second-y==1){
      cout<<"came3"<<endl;
      while(botDirection!=1){rot90;}
      movef;
      }
      else if(Path[i]->second-y==-1){
      cout<<"came4"<<endl;
      while(botDirection!=3){rot90;}
      movef;
      }
    }
    robot.Read();
    
     int  x=p2dProxy.GetXPos()+.5;
     int  y=p2dProxy.GetYPos()+.5;
      if(config.first-x==1){
      cout<<"came1"<<endl;
      while(botDirection!=0){rot90;}
      movef;
      }
      else if(config.first-x==-1){
      
      cout<<"came2"<<endl;
      while(botDirection!=2){rot90;}
      movef;
      }
      else if(config.second-y==1){
      cout<<"came3"<<endl;
      while(botDirection!=1){rot90;}
      movef;
      }
      else if(config.second-y==-1){
      cout<<"came4"<<endl;
      while(botDirection!=3){rot90;}
      movef;
      }
    
     robot.Read();
       x=p2dProxy.GetXPos()+.5;
       y=p2dProxy.GetYPos()+.5;   
    if(config.first!=x || config.second!=y){printf("error position\n");exit(0);}
    robot.Read();
 /*   while(1){
    printf("sonar %f %f %f %f %f %f\n",p2dProxy.GetXPos(),p2dProxy.GetYPos(),sonarProxy[0],sonarProxy[1],sonarProxy[2],sonarProxy[3]);
    rot90;
    robot.Read();
    printf("%d\n",botDirection);
    }*/
	if(Path.size()==0){printf("path %d %d null\n",config.first,config.second);}
	printf("bot direction is %d\n",botDirection);
    printf("sonar %d %d %f %f %f %f %f %f\n",config.first,config.second,p2dProxy.GetXPos(),p2dProxy.GetYPos(),sonarProxy[0],sonarProxy[1],sonarProxy[2],sonarProxy[3]);
    if(sonarProxy[(1+botDirection)%4]>.5){
    create(config.first+1,config.second+0);
    set_next;edges[config.first][config.second][1]=1;
    edges[config.first+1][config.second][3]=1;
    cout<<config.first<<" "<<config.second<<" "<<config.first+1<<" "<<config.second<<endl;
    }
    if(sonarProxy[(2+botDirection)%4]>.5){
    create(config.first,config.second-1);
    set_next;edges[config.first][config.second][0]=1;
    edges[config.first][config.second-1][2]=1;
    cout<<config.first<<" "<<config.second<<" "<<config.first<<" "<<config.second-1<<endl;
    }
    if(sonarProxy[(3+botDirection)%4]>.5){
    create(config.first-1,config.second+0);
    set_next;edges[config.first][config.second][3]=1;
    edges[config.first-1][config.second][1]=1;
    cout<<config.first<<" "<<config.second<<" "<<config.first-1<<" "<<config.second<<endl;
    }
    if(sonarProxy[(0+botDirection)%4]>.5){
    create(config.first,config.second+1);
    set_next;edges[config.first][config.second][2]=1;
    edges[config.first][config.second+1][0]=1;
    cout<<config.first<<" "<<config.second<<" "<<config.first<<" "<<config.second+1<<endl;
    }
    
  }
  
//#undef set_next
//#undef create
//#undef rot90
//#undef movef
};





_pair puzzle_mn::goal;
template<>
int node<int, string>::count = 0;
template<>
int node<int, string>::expandcount = 0;
//////////////////////////////
int g=0;
//////////////////////////////
int main()
{ /*
    int start[12]={1,2,0,3,5,6,7,4,9,10,11,8};
    int end[12]={1,2,3,4,5,6,7,8,9,10,11,0};
  */
  p2dProxy.SetMotorEnable(1);
  using namespace std;
  init();
  _pair start(0,0);
  _pair end(8,8);
  puzzle_mn p = puzzle_mn(start,end);
  if(true){
    puzzle_mn* l = dynamic_cast<puzzle_mn*>(astar<int, string>(&p));
    printf("yes\n");
    while (l != NULL)
      { 
	cout<<l->config.first<<" "<<l->config.second<<endl;
/////////////////////////////////	
	
	robot.Read();
     int  x=p2dProxy.GetXPos()+.5;
     int  y=p2dProxy.GetYPos()+.5;
      if(l->config.first-x==1){
      cout<<"came1"<<endl;
      while(botDirection!=0){rot90;}
      movef;
      }
      else if(l->config.first-x==-1){
      
      cout<<"came2"<<endl;
      while(botDirection!=2){rot90;}
      movef;
      }
      else if(l->config.second-y==1){
      cout<<"came3"<<endl;
      while(botDirection!=1){rot90;}
      movef;
      }
      else if(l->config.second-y==-1){
      cout<<"came4"<<endl;
      while(botDirection!=3){rot90;}
      movef;
      }
      
      
////////////////////////////////////////      
      
      
	l = dynamic_cast<puzzle_mn*>(l->parentptr);
      }
    printf("nodes created:%d\n",p.count);
    printf("nodes expanded:%d\n",p.expandcount);
  }
  else{
    cout<<"goal nt reachable\n";
  }
  
  while(1);
}

