
#include <iostream>
#include <stdio.h>
using namespace std;
template<class T>
class heap;

template<class T>
class element
{
public:
  heap<T>* root;
  element(){}
  virtual T f()=0;
  virtual ~element(){}
};

template<class T>
class heap
{
public:
  heap* l;
  heap* r;
  heap* p;
  int s;
  element<T>* el;
  heap(element<T>* e)
  {
    el = e;
    this->el->root = this;
    s = 1;
    l = NULL;
    r = NULL;
  }
  void
  add(element<T>* e)
  {
    s++;
    if (this->l == NULL)
      {
	if (el->f() > e->f())
	  {
	    l = new heap(el);
	    l->p = this;
	    l->el->root = l;
	    el = e;
	    el->root = this;
	  }
	else
	  {
	    l = new heap(e);
	    l->p = this;
	    l->el->root = l;
	  }
      }
    else if (r == NULL)
      {
	if (el->f() > e->f())
	  {

	    r = new heap(el);
	    r->p = this;
	    r->el->root = r;
	    el = e;
	    this->el->root = this;
	  }
	else
	  {
	    r = new heap(e);
	    r->p = this;
	    r->el->root = r;
	  }
      }
    else if (l->s > r->s)
      {
	if (el->f() > e->f())
	  {
	    r->add(el);
	    el = e;
	    this->el->root = this;
	  }
	else
	  {
	    r->add(e);
	  }
      }
    else
      {
	if (el->f() > e->f())
	  {
	    l->add(el);
	    el = e;
	    this->el->root = this;
	  }
	else
	  {
	    l->add(e);
	  }
      }
  }

  heap*
  removeMin()
  {
    heap* newRoot = getRomove();
    if (s > 2)
      {
	newRoot->l = this->l;
	newRoot->r = this->r;
	newRoot->s = this->s;
	newRoot->p = NULL;
	if (newRoot->l->p != NULL)
	  newRoot->l->p = newRoot;
	if (newRoot->r->p != NULL)
	  newRoot->r->p = newRoot;
	newRoot->heapifydown();
      }
    else if (s == 2)
      {
	newRoot->l = this->l;
	newRoot->s = this->s;
	newRoot->r = NULL;
	newRoot->p = NULL;
	if (newRoot->l->p != NULL)
	  newRoot->l->p = newRoot;
	newRoot->heapifydown();

      }
    else
      {
	newRoot->p = NULL;
	newRoot->s = 1;
	newRoot->l = NULL;
	newRoot->r = NULL;
      }
    //delete (this);
    return newRoot;
  }
  heap*
  update(element<T>* e)
  {
    return e->root->heapifyup();
  }

  void
  print()
  {
    if (l != NULL)
      l->print();
    cout << el->f() << " " << s << "\n";
    if (r != NULL)
      r->print();
  }
private:
  heap*
  heapifyup()
  {
    if (this->p == NULL)
      return this;
    else if (this->p->el->f() < this->el->f())
      {
	return this->p->heapifyup();
      }
    else
      {
	element<T>* e = this->p->el;
	this->p->el = el;
	this->p->el->root = this->p;
	this->el = e;
	el->root = this;
	return this->p->heapifyup();
      }
  }
  void
  heapifydown()
  {
    if (this->s == 1)
      return;
    if (this->s == 2)
      {
	if (this->el->f() <= l->el->f())
	  return;
	else
	  {
	    element<T>* e = this->el;
	    this->el = l->el;
	    this->el->root = this;
	    l->el = e;
	    l->el->root = l;
	    return;
	  }
      }
    if (this->el->f() <= l->el->f() && this->el->f() <= r->el->f())
      {
	return;
      }
    else if (l->el->f() > r->el->f())
      {
	element<T>* e = this->el;
	this->el = r->el;
	this->el->root = this;
	r->el = e;
	r->el->root = r;
	r->heapifydown();
      }
    else
      {
	element<T>* e = this->el;
	this->el = l->el;
	this->el->root = this;
	l->el = e;
	l->el->root = l;
	l->heapifydown();
      }
  }
  heap*
  getRomove()
  {
    this->s--;
    if (s <= 2)
      {
	if (s == 2)
	  {
	    heap* h = this->r;
	    this->r = NULL;
	    return h;
	  }
	else if (s == 1)
	  {
	    heap* h = this->l;
	    this->l = NULL;
	    return h;
	  }
	cout << "error";
      }
    else if (l->s == r->s)
      {
	return r->getRomove();
      }
    else
      {
	return l->getRomove();
      }
    return this;
  }

};
template<class T>
class priorityqueue
{
public:
  heap<T>* h;
  priorityqueue()
  {
    h = NULL;
  }
  element<T>*
  pop()
  {
    if (h == NULL)
      {
	cout << "heap null at pop" << endl;
	return NULL;
      }
    element<T>* root = h->el;
    if (h->s == 1)
      {
	h = NULL;
	return root;
      }
    h = h->removeMin();
    return root;
  }
  void
  insert(element<T>* e)
  {
    if (h == NULL)
      {
	h = new heap<T>(e);
      }
    else
      {
	h->add(e);
      }
  }
  bool
  empty()
  {
    return h == NULL;
  }
  void
  update(element<T>* e)
  {
    if (h == NULL)
      {
	cout << "heap null at update" << endl;
	return;
      }
    h = h->update(e);
  }
};
