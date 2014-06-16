#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <map>
#include <utility>
#include <string>
#include "heap.cpp"

#include <libplayerc++/playerc++.h>
using namespace PlayerCc;
using namespace std;

template<class T, class K>
class node : public element<T>
{
public:
  static int count;
  static int expandcount;
  int id;
  T g;
  T h;
  K key;
  node* parentptr;
  bool markgoal;
  bool closed;

  T f() {
    return g + h;
  }

  typedef pair<node*, T> edge;
  vector<edge> neighbours;
  node()
  {
    parentptr = NULL;
    closed = false;
    count++;
    id = count;
    markgoal = false;
  }
  node(T _key, T _g, T _h)
  {
    key = _key;
    this->g = _g;
    this->h = _h;
    parentptr = NULL;
    closed = false;
    count++;
    id = count;
    markgoal = false;
  }
  void  setgoal(){
    markgoal = true;
    this->h = 0;
  }
  virtual void expand() {
    return;
  }
  virtual bool isless(node* n)  {
    return key < n->key;
  }
  virtual bool isgoal() {
    return markgoal;
  }
  virtual  ~node() {}
};



template<class K>
struct keycomp
{
  bool
  operator()(const K& k1, const K& k2) const
  {
    return k1 < k2;
  }
};

template<class T, class K>
node<T, K>*
astar(node<T, K>* startnode)
{
  startnode->g = 0;
  typedef node<T, K>* nodeptr;
  typedef pair<nodeptr, T> edge;
  map<K, nodeptr, keycomp<K> > keyptrmap;
  priorityqueue<T> openlist;
  openlist.insert(startnode);
  keyptrmap[startnode->key] = startnode;
  while (!openlist.empty())
    {
      element<T>* top = openlist.pop();
      nodeptr n = dynamic_cast<nodeptr>(top);
#if DEBUG
      cout<<"node "<<n->key<<" popped from open list - f="<<n->f()<<"\n";
#endif
      n->closed = true;
      if (n->isgoal())
	{
	  cout << "search successful\n" << endl;
	  return n;
	}
      if(n->neighbours.size()==0){
	n->expand();
	n->expandcount++;
      }
#if DEBUG
      cout<<"node "<<n->key<<" expanded\n";
#endif
      for (unsigned int i = 0; i < n->neighbours.size(); i++)
	{
	  K key1 = n->neighbours[i].first->key;
	  T edgecost = n->neighbours[i].second;
#if DEBUG
	  cout<<"neighbour "<<key1<<" found with edgecost "<< edgecost<<"\n";
#endif
	  if (keyptrmap.count(key1) == 0)
	    {
	      nodeptr r = n->neighbours[i].first;
	      keyptrmap[key1] = r;
	      r->g = n->g + edgecost;
	      r->parentptr = n;
	      openlist.insert(r);
#if DEBUG
	      cout<<"neighbour "<<r->key<<" added to openlist g->"<<r->g<<" parent->"<<r->parentptr->key<<"\n";
#endif
	      continue;
	    }
	  //if found check g value
	  nodeptr m = keyptrmap[key1];
	  delete(n->neighbours[i].first);
	  n->neighbours[i].first=m;            
	  if (m->g > n->g + edgecost)
	    {
	      m->parentptr = n;
	      m->g = n->g + edgecost;
#if DEBUG
	      cout<<"neighbour "<<m->key<<" - g->"<<m->g<<" parent-> "<<m->parentptr<<"\n";
#endif
	      if (m->closed)
		{
		  m->closed = false;
		  openlist.insert(m);
#if DEBUG
		  cout<<"neighbour "<<m->key<<" added to openlist from closedlist\n";
#endif
		}
	      else
		{
		  openlist.update(m);
#if DEBUG
		  cout<<"neighbour "<<m->key<<" updated in openlist\n";
#endif
		}
	    }
	}
    }
  cout << "search failed\n" << endl;
  return NULL;
}

