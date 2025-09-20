/// ----
/// ---- file   : tks-list.h
/// ---- author : Bastian Spiegel <bs@tkscript.de> <fli@tkscript.de>
/// ---- date   : ??-??-1999 / 16-Jun-2003, 27-Dec-2007(64bit fix), 18-Jul-2023 (fix register warning)
/// ---- diz    : a set of functions to handle double linked lists
//                originally written for the "tks" project
/// ---- license: GNU Lesser ("Library") General Public License. ("LGPL")
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/// ---- 

#ifndef __TKS_STATiCLIST_H__
#define __TKS_STATiCLIST_H__

#define NEW_NODE (StaticList::Node*)(::malloc( sizeof(StaticList::Node) ))
#define FREE_NODE(p) ::free((void*)p)

class StaticList {
public:
   
   // ---- a single list node ----
   struct Node {
      /// ---- pointer to previous node ----
      struct Node *previous;
      
      /// ---- pointer to previous node ----
      struct Node *next;
      
      /// ---- pointer to data ----
      void *data;
   };
   
   public:
      StaticList(void);
      ~StaticList(void);
      
      /// Seek head of list starting with node <b>a</b>
      /// @param Node
      /// @return head node (or a)
      static StaticList::Node *Head(StaticList::Node *a) {
         while(a && a->previous ) a=a->previous;
         return a;
      }
      
      /// Seek tail of list starting with node <b>a</b> 
      /// @param Node
      /// @return head node (or a)
      /// @see Head Tail Remove RemoveData Append AppendTail Insert InsertHead Find Seek
      static StaticList::Node *Tail(StaticList::Node *a) {
         while(a && a->next ) a=a->next;
         return a;
      }
      
      /// Free list starting at <b>a</b>
      /// @param a Node 
      /// @see Head Tail Remove RemoveData Append AppendTail Insert InsertHead Find Seek
      static void Free( StaticList::Node **a ) {
         StaticList::Node *b;
         while((b=*a)) {
            *a=b->next;
            b->data=0;
            b->previous=0;
            b->next=0;
            FREE_NODE(b);
         }
      }
      
      /// Remove node referenced by <b>a</b> from list.
      /// Please avoid removing the first node of the list (use Free())
      /// @param a Node
      /// @see Head Tail Free RemoveData CreateEmpty Append AppendTail Insert InsertHead
      /// Find Seek
      static void Remove( StaticList::Node *a ) {
         if(a) {
            if( a->next ) a->next->previous=a->previous;
            if(a->previous) a->previous->next=a->next;
            FREE_NODE(a);
         }
      }
      
      /// Remove node with UserData <b>b</b> in list that start with node 
      /// referenced by <b>a</b>.
      /// A is updated if the UserData field was found in the first node.
      ///
      /// @param a Node
      /// @param b UserData
      /// @see Head Tail Free Remove CreateEmpty Append AppendTail Insert InsertHead  
      /// Find Seek
      static void RemoveData( StaticList::Node **a , void *b ) {
         StaticList::Node *p=StaticList::Find(*a,b);
         if(p) {
            if(p==*a) *a=(*a)->next;
            StaticList::Remove(p);
         }
      }
      
      /// Create new empty node. 
      /// @param a Reference to node. 
      /// @see Head Tail Free Remove RemoveData Append AppendTail Insert InsertHead  
      /// Find Seek
      static void CreateEmpty( StaticList::Node **a ) {
         if(a) {
            *a=NEW_NODE;
            if((*a)) {
               (*a)->previous=0;
               (*a)->next=0;
               (*a)->data=0;
            } else {
#ifdef TKS_DCON
               dcon.printf("[---] StaticList::CreateEmpty: ERROR: cannot allocate node\n");
#endif
            }
         }
      }
      
      /// Insert node before node <b>a</b>
      /// @param a Node
      /// @param b UserData
      /// @return New node
      /// @see Head Tail Free Remove RemoveData CreateEmpty Append AppendTail InsertHead  
      /// Find Seek
      static StaticList::Node *Insert( StaticList::Node *a, void *b ) {
         StaticList::Node *p;
         p= NEW_NODE;
         if(!p) {
#ifdef TKS_DCON
            dcon.printf("Insert: ERROR: cannot allocate new node.\n");
#endif
            return 0;
         }
         p->data=b;
         p->previous=a->previous;
         p->next=a;
         if(a->previous) a->previous->next=p;
         a->previous=p;
         return p;
      }
      
      /// Insert node before node <b>a</b>
      /// The new node is stored at <b>*a</b>.
      /// If <b>*a</b> is 0, a new list will be created.
      /// <br> Using Insert usually increases speed (no need to seek to list tail) but
      /// results in a 'reversed' FIFO list.
      /// @param a Node
      /// @param b UserData
      /// @return New node
      /// @see Head Tail Free Remove RemoveData CreateEmpty Append AppendTail InsertHead  
      /// Find Seek
      static StaticList::Node *Insert( StaticList::Node **a, void *b ) {
         if(a) {
            if(!*a) {
               StaticList::CreateEmpty(a);
               (*a)->data=b;
               return *a;
            }
            return *a=StaticList::Insert(*a, b);
         }
         return 0;
      }
      
      /// Insert node before head of list that node <b>a</b> belongs to.
      /// The new node is stored at <b>*a</b>.
      /// If <b>*a</b> is 0, a new list will be created.
      /// @param a Node
      /// @param b UserData
      /// @return New node
      /// @see Head Tail Free Remove RemoveData CreateEmpty Append AppendTail Insert
      /// Find Seek
      static StaticList::Node *InsertHead( StaticList::Node **a, void *b) {
         if(a) {
            if(!*a) {
               StaticList::CreateEmpty(a);
               (*a)->data=b;
               return *a;
            }
            else StaticList::Insert(a,b);
         }
         return 0;
      }
      
      /// Append node after node <b>a</b>.
      /// @param a Node
      /// @param b UserData
      /// @return New node
      /// @see Head Tail Free Remove RemoveData CreateEmpty AppendTail Insert InsertHead
      /// Find Seek 
      static StaticList::Node *Append( StaticList::Node *a, void *b ) {
         if(a->next) return Insert(a->next, b);
         a->next= NEW_NODE;
         if(!a->next) 
         {
#ifdef TKS_DCON
            dcon.printf("Append: ERROR: cannot allocate node.\n");
#endif
            return 0;
         }
         a->next->previous=a;
         a=a->next;
         a->next=0;
         a->data=b;
         return a;
      }
      
      /// Append node after node <b>a</b>.
      /// The new node is stored at <b>*a</b>.
      /// If <b>*a</b> is 0, a new list will be created.
      /// @param a Node
      /// @param b UserData
      /// @return New node
      /// @see Head Tail Free Remove RemoveData CreateEmpty AppendTail Insert InsertHead Find Seek
      static StaticList::Node *Append( StaticList::Node **a, void *b ) {
         if(a) {
            if(!(*a)) {
               CreateEmpty(a);
               (*a)->data=b;
               return *a;
            }
            return StaticList::Append(*a, b);
         }
         return 0;
      }
      
      /// Append node at tail of list starting with node <b>a</b>
      /// @param a Node
      /// @param b UserData
      /// @return New node
      /// @see Head Tail Free Remove RemoveData CreateEmpty Append Insert InsertHead
      /// Find Seek 
      static StaticList::Node *AppendTail( StaticList::Node **a, void *b );
      
      /// Find node with UserData <b>b</b> in list starting with node <b>a</b>.
      /// @param a Node
      /// @param b UserData
      /// @return Node pointer or NULL if UserData was not found.
      /// @see Head Tail Free Remove RemoveData CreateEmpty Append AppendTail
      /// Insert InsertHead Seek 
      static StaticList::Node *Find( StaticList::Node *n , void *b ) {
         while(n) {
            if(n->data==b) return n;
            n=n->next;
         }
         return 0;
      }
      
      /// Seek node at index <b>i</b> in list starting with node <b>a</b>
      /// @param Node
      /// @param Index
      /// @return Node or 0 (ListSubscriptError)
      /// @see Head Tail Free Remove RemoveData CreateEmpty Append AppendTail
      /// Insert InsertHead Find 
      static StaticList::Node *Seek( StaticList::Node *a, sS32 _index) {
         sS32 i=0;
         for(;i<_index && a; i++) a=a->next;
         return a;
      }
      
      /// Print content of list starting with node <b>a</b>
      /// @param a Node
      /// @see Head Tail Free Remove RemoveData CreateEmpty Append AppendTail
      /// Insert InsertHead Seek Find
      static void Print( StaticList::Node *a ) {
#ifdef TKS_DCON
         sS32 i=0;
#endif
         if(!a) 
         {
#ifdef TKS_DCON
            dcon.printf("Print: list pointer is NULL.\n");
#endif
         }
         
         while(a) 
         {
#ifdef TKS_DCON
            dcon.printf("%4i:<<:%08x|*:%08x|>>:%08x|data:%08x\n",
               i,
               (sS32) a->previous,
               (sS32) a,
               (sS32) a->next,
               (sS32) a->data
               );
            i++;
#endif
            a=a->next;
         }
      }
      
      /// Return number of nodes in list starting with node <b>a</b>.
      static sS32 Length( StaticList::Node *a ) {
         sS32 ret=0;
         StaticList::Node *c=a;
         while(c) {
            ret++;
            c=c->next;
         }
         return ret;
      }
      
      /// Convert contents of list starting with node <b>a</b> to array. 
      /// This will create an array of unsigned ints
      /// that has as many elements as the list has entries.
      /// The <b>array must be deleted</b>.
      /// @param a First node of list
      /// @param p Reference to array pointer (must be <b>0</b> to create a <b><i>new</i></b> array</b>, otherwise an existing array is used)
      /// @return Number of elements in array. The <b>p</b> pointer is set also.
      static sS32 ListToArray(StaticList::Node *a, void***p) {
         if(p) {
            sS32 ret=StaticList::Length(a);
            if(ret) {
               if(!(*p)) *p=new void*[ret];
               sS32 i=0;
               for(; i<ret; i++)
               {
                  (*p)[i]= (*a).data;
                  a=(*a).next;
               }
            }
            return ret;
         }
         return 0;
      }
      
      /// Convert contents of array to list. The created list will contain
      /// <b>n</b> nodes.
      ///
      /// @param a Reference to pointer to first node of list (reference to 0 pointer)
      /// @param n Number of elements in array
      /// @param p Pointer to array from which the elements will be taken
      static void ArrayToList(StaticList::Node **a, sS32 n, void **p) {
         if(p)
            if(n>0)
               for(;n>0;)  StaticList::Insert(a, p[--n]);
      }
      
      /// Reverse list starting with node <b>a</b>.
      /// @param a Pointer to first node of list
      static void Reverse(StaticList::Node *a) {
         if(a) {
            StaticList::Node *ln=a;
            sS32 length=1;
            // seek last node and determine list length
            while(ln->next) {
               ln=ln->next;
               length++;
            }
            StaticList::Node *c=ln;
            length=length>>1;
            for(; length>0; length--) {
               void* t=c->data;
               c->data=a->data;
               a->data=t;
               c=c->previous;
               a=a->next;
            }
         }
      }
      
};

#endif
