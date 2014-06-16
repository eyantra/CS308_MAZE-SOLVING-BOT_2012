#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <utility>
#include <string>
#include <stack>
#include <libplayerc++/playerc++.h>
#include "ophead.cpp"

float X=-4.5,Y=-4.5;
int A=0;

#define rot90    p2dProxy.SetSpeed(0, dtor(90)); usleep(1000000);p2dProxy.SetSpeed(0, dtor(0));usleep(10000);botDirection=(botDirection+1)%4;A=(A+90)%360;\
sim.SetPose2d(name,X,Y,A*1.0*3.1415/180);

#define movef    p2dProxy.SetSpeed(1, dtor(0)); usleep(1000000);p2dProxy.SetSpeed(0, dtor(0));usleep(10000);\
if(botDirection==0)X++;\
if(botDirection==1)Y++;\
if(botDirection==2)X--;\
if(botDirection==3)Y--;\
sim.SetPose2d(name,X,Y,A*1.0*3.1415/180);

int botDirection=0;
char name[]="robot";

PlayerClient    robot("localhost", 6665);

Position2dProxy p2dProxy(&robot,0);
SonarProxy      sonarProxy(&robot,0);
SimulationProxy sim(&robot,0);  
/*
int wall[10][10][4];
int parent_x[10][10];
int parent_y[10][10]; 
int visited[10][10];

void clear_visited(){
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            visited[i][j];
}
void init(){
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++){
            parent_x[i][j]=-1;
            parent_y[i][j]=-1;
            for(int k=0;k<4;k++)
                wall[i][j][k]=0;
        }
}

void route(int x1,int y1,int x2,int y2){
stack <int> rx;
stack <int> ry;
    int m,n,temp1,temp2;
    m=x2;n=y2;
        rx.push(m);
        ry.push(n);
    while(parent_x[m][n]==x1&&parent_y[m][n]==y1){
        //move
        temp1=parent_x[m][n];temp2=parent_y[m][n];
        m=temp1;n=temp2;
        rx.push(m);
        ry.push(n);
    }
    while(!rx.empty()&&!ry.empty()){
    	m=rx.top();n=ry.top();
    	rx.pop();ry.pop();
    	cout<<m<<" "<<n<<endl;
    	if(x1==m+1&&y1==n){
    		while(botDirection!=2){rot90;}
    		movef;
    		x1=m;y1=n;
    	}
    	if(x1==m-1&&y1==n){
    	while(botDirection!=0){rot90;}
    		movef;
    		x1=m;y1=n;
    	}
    	if(x1==m&&y1==n-1){
    		while(botDirection!=1){rot90;}
    		movef;
    		x1=m;y1=n;
    	}
    	if(x1==m&&y1==n+1){
    		while(botDirection!=3){rot90;}
    		movef;
    		x1=m;y1=n;
    	}
    
    }
    return;
}

void move(int x1,int y1,int x2,int y2){
    clear_visited();
    int a,b,temp1,temp2;
    stack <int> s_x;
    stack <int> s_y;
    s_x.push(x1);
    s_y.push(y1);
    cout<<"vachindi\n";
    cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
    while(!s_x.empty()&&!s_y.empty()){
        a=s_x.top();b=s_y.top();
        s_x.pop();s_y.pop();
        if(wall[a][b][0]==1&&visited[a+1][b]==0){
            parent_x[a+1][b]=a;parent_y[a+1][b]=b;
            s_x.push(a+1);s_y.push(b);
            if(x2==a+1&&y2==b) {route(x1,y1,x2,y2);return;}
        }
        if(wall[a][b][1]==1&&visited[a][b+1]==0){
            parent_x[a][b+1]=a;parent_y[a][b+1]=b;
            s_x.push(a);s_y.push(b+1);
            if(x2==a&&y2==b+1) {route(x1,y1,x2,y2);return;}
        }
        if(wall[a][b][2]==1&&visited[a-1][b]==0){
            parent_x[a-1][b]=a;parent_y[a-1][b]=b;
            s_x.push(a-1);s_y.push(b);
            if(x2==a-1&&y2==b) {route(x1,y1,x2,y2);return;}
        }
        if(wall[a][b][3]==1&&visited[a][b-1]==0){
            parent_x[a][b-1]=a;parent_y[a][b-1]=b;
            s_x.push(a);s_y.push(b-1);
            if(x2==a&&y2==b-1) {route(x1,y1,x2,y2);return;}
        }
        
    }
    
    
}
    
class robot1:public AstarGenericNode {
public:
int x,y;

    robot1(string a, int b){
        x=a[0]-'0';y=a[1]-'0';

    }
    robot1(){}
string convert(int a,int b){
	char p[2];
    		sprintf(p, "%d%d",a,b);
    		printf("string is: %s\n",p);
	return string(p);
}
    bool goalCheck (void) {
    return false;
    	if(x==0&&y==3) return true;
    	return false;
     
    }
    

    void listChildren(vector<string>& v){
        p2dProxy.RequestGeom();
        sonarProxy.RequestGeom();
        robot1 p=*this;sonarProxy[0]<1
        robot.Read();
        string s;
        printf("tomove:%f %f %d %d\n",p2dProxy.GetXPos(),p2dProxy.GetYPos(),p.x,p.y);
        move(p2dProxy.GetXPos()+.5,p2dProxy.GetYPos()+.5,p.x,p.y);
        robot.Read();
        printf("sonar:%f %f %f %f\n",sonarProxy[0],sonarProxy[1],sonarProxy[2],sonarProxy[3]);
        if(sonarProxy[(0+botDirection)%4]>0.5) {printf("1\n");wall[p.x][p.y][0]=1;wall[p.x+1][p.y][2]=1;v.push_back(convert(p.x+1,p.y));}
        if(sonarProxy[(1+botDirection)%4]>0.5) {printf("2\n");wall[p.x][p.y][1]=1;wall[p.x][p.y+1][3]=1;v.push_back(convert(p.x,p.y+1));}
        if(sonarProxy[(2+botDirection)%4]>0.5) {printf("3\n");wall[p.x][p.y][2]=1;wall[p.x-1][p.y][0]=1;v.push_back(convert(p.x-1,p.y));}
        if(sonarProxy[(3+botDirection)%4]>0.5) {printf("4\n");wall[p.x][p.y][3]=1;wall[p.x][p.y-1][1]=1;v.push_back(convert(p.x,p.y-1));}
        
        
    }


};
*/


int main(){
using namespace std;
p2dProxy.RequestGeom();
sonarProxy.RequestGeom();
        robot.Read();
int x=p2dProxy.GetXPos()+.5;
int y=p2dProxy.GetYPos()+.5;
cout<<x<<"   "<<y<<endl;
rot90;
while(x!=0||y!=6){

        robot.Read();
x=p2dProxy.GetXPos()+.5;
y=p2dProxy.GetYPos()+.5;
cout<<x<<"   "<<y<<"   "<<p2dProxy.GetXPos()<<"   "<<p2dProxy.GetYPos()<<endl;
        printf("sonar:%f %f %f %f\n",sonarProxy[0],sonarProxy[1],sonarProxy[2],sonarProxy[3]);
if(sonarProxy[0]>1&&sonarProxy[1]<1) {
    //if(sonarProxy[3]>1) s.append("S");
    movef;
}/*
else if(sonarProxy[0]<1&&sonarProxy[1]<1&&sonarProxy[3]<1){
rot90;rot90;
s.append("B");
}*/
else if(sonarProxy[1]>1){
rot90;movef;
//s.append("L");
//cout<<"left sensor\n";
}
else if(sonarProxy[0]<1){
rot90;rot90;rot90;
//s.append("R");
//cout<<"front sensor\n";
}


}

while(1);
return 0;
}
