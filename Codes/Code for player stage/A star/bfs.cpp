#include<iostream>
#include<utility>
#include <vector>
using namespace std;


#define size 10
bool edges[size][size][4];
bool visit [size][size];
bool visited [size][size];
pair<int,int> parent[size][size];
pair<int,int> queue[size*size];
vector<pair<int,int>* > Path;

void init()
{
	for(int i=0;i<size;i++)
	{
		for(int j=0;j<size;j++)
		{
			visited[i][j]=false;
			visit[i][j]=0;
			for(int k=0;k<4;k++)
			{
				edges[i][j][k]=0;
			}
		}
	}
}

bool insert(int x,int y,int z,int &rear)
{
	//z=0,1,2,3
	int p1=x;int p2=y;
	switch(z)
	{
	case 0: y--;break;
	case 1: x++;break;
	case 2: y++;break;
	case 3: x--;break;
	}
	if(x>=0 && x<size && y>=0 && y<size && !visit[x][y] && !visited[x][y] && edges[p1][p2][z])
	{
		queue[rear].first=x;
		queue[rear].second=y;
		rear++;
		visit[x][y]=true;
		parent[x][y].first=p1;
		parent[x][y].second=p2;
	//	cout<<"inserted "<<x<<" "<<y<<endl;
	}
}



void path(int s1,int s2,int g1,int g2){
for(int i=0;i<size;i++)
	{
		for(int j=0;j<size;j++)
		{
			visited[i][j]=false;
			visit[i][j]=0;
		}
	}
visited[s1][s2]=true;
int x,y;
Path.clear();
	int front=0;
	int rear=0;
	queue[rear].first=s1;
	queue[rear].second=s2;
	pair<int,int> curr;
	rear++;
	bool found=false;
	while(rear!=front)
	{
		curr=queue[front++];
		x=curr.first;
		y=curr.second;
		visit[x][y]=0;
		if(x==g1 and y==g2)
		{
			found=true;
			break;
		}
		insert(x,y,0,rear);
		insert(x,y,1,rear);
		insert(x,y,2,rear);
		insert(x,y,3,rear);
		visited[x][y]=true;
	}
		if(found)
		{
		curr=parent[g1][g2];
		x=curr.first;
		y=curr.second;
		pair<int,int>* p=new pair<int,int>(g1,g2);
		Path.push_back(p);
		while(!(x==s1 && y==s2))
		{
			x=curr.first;
			y=curr.second;
			//cout<<x<<" "<<y<<endl;
			pair<int,int>* p=new pair<int,int>(x,y);
			Path.push_back(p);
			curr=parent[x][y];
		}
		
		}
}
#undef size
