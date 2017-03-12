#pragma once
#include "Pool.h"

namespace tblib
{
	// поебавшись с шаблонами, € мог бы, возможно, вху€рить ссылки на X:XClass<T> и Y:YClass<T>, где X и Y - параметры шаблонного класса T

	template <int POOL_SIZE> 
	class List2D
	{		
	public :

		struct XClass;
		struct YClass;	

		struct XYNode
		{
			int index;
			XClass *x;
			YClass *y;
			XYNode *prevx, *nextx, *prevy, *nexty;
			XYNode() {}
		};

		struct XClass
		{
			XYNode *firsty, *lasty;
			List2D *l;

			XClass () : firsty(NULL), lasty(NULL), l(NULL) {}
			~XClass () { if(l) l->DelXFromList(this); }
		private :
			XClass (const XClass&);
			XClass& operator = (const XClass&);
		};
		
		struct YClass
		{
			XYNode *firstx, *lastx;
			List2D *l;

			YClass () : firstx(NULL), lastx(NULL), l(NULL) {}
			~YClass () { if(l) l->DelYFromList(this); }
		private :
			YClass (const YClass&);
			YClass& operator = (const YClass&);
		};

	private :
		Pool<XYNode, POOL_SIZE> nodes;	

	public :
		~List2D()
		{
			for (int i=0; i<nodes.Size(); ++i) if (nodes.Valid(i))
			{
				new (nodes[i].x) XClass();
				new (nodes[i].y) YClass();
			}
		}

		void PutXToY (XClass* x, YClass* y)
		{
			assert (x->l == NULL || x->l == this);
			x->l = this;
			assert (y->l == NULL || y->l == this);
			y->l = this;

			int index = nodes.Push();
			XYNode* node = &nodes[index];
			node->index = index;
			node->x = x;
			node->y = y;

			node->nexty = NULL;
			node->prevy = x->lasty;
			(x->firsty ? x->lasty->nexty : x->firsty) = node;
			x->lasty = node;
			
			node->nextx = NULL;
			node->prevx = y->lastx;
			(y->firstx ? y->lastx->nextx : y->firstx) = node;
			y->lastx = node;		
		}

		void DelNodeFromList (XYNode* node)
		{
			XYNode* &fromprevx = node->prevx ? node->prevx->nextx : node->y->firstx;
			XYNode* &fromnextx = node->nextx ? node->nextx->prevx : node->y->lastx;			
			XYNode* &fromprevy = node->prevy ? node->prevy->nexty : node->x->firsty;
			XYNode* &fromnexty = node->nexty ? node->nexty->prevy : node->x->lasty;
			assert(fromprevx == node);
			assert(fromnextx == node);
			assert(fromprevy == node);
			assert(fromnexty == node);
			fromprevx = node->nextx;
			fromnextx = node->prevx;
			fromprevy = node->nexty;
			fromnexty = node->prevy;
			nodes.FreeIndex(node->index);
		}

		void DelXFromFirstY (XClass* x)
		{
			XYNode* node = x->firsty;
			assert(node);
			DelNodeFromList(node);
		}

		void DelXFromList (XClass* x)
		{
			while (x->firsty)
				DelXFromFirstY(x);
			x->l = NULL;
		}
		
		void DelYFromFirstX (YClass* y)
		{
			XYNode* node = y->firstx;
			assert(node);
			DelNodeFromList(node);
		}

		void DelYFromList (YClass* y)
		{
			while (y->firstx)
				DelYFromFirstX(y);
			y->l = NULL;
		}
    
	};
};