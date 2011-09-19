#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

#include "rbtree.h"
struct rb_root        key_user_tree;

struct key_user
{
    struct rb_node	node;
    int			key;                
};

struct key_user root_key_user;

struct key_user * RBTreeInsert(int32_t key)
{
    struct rb_node **p;
    struct key_user *candidate = NULL, *user;
    struct rb_node *parent = NULL;
    
    p = &key_user_tree.rb_node;
    
	while (*p) 
	{
        	parent = *p;
        	user = (struct key_user *)rb_entry((struct rb_node *)parent, struct key_user, node);
        
        	if (key < user->key)
		{
            		p = &(*p)->rb_left;
        		printf ("<<\n");
		}
		else if (key > user->key)
        	{
			printf (">>\n");	
			p = &(*p)->rb_right;
        	}
		else
        	{
            		printf("RBTreeInsert : exist\n");
            		return NULL;
        	}
    }   
    
	candidate = (struct key_user *)malloc( sizeof(struct key_user) );

	if ( NULL == candidate )
	{
        	return NULL;
	}
    
	candidate->key = key;
	rb_link_node(&candidate->node, parent, p);
	rb_insert_color(&candidate->node, &key_user_tree);    
    	return candidate;
}


struct key_user * RBTreeSerach(int32_t key)
{
	
	struct key_user *user;
	struct rb_node *parent = NULL;
	struct rb_node **p;
    
	p = &key_user_tree.rb_node;
    
	while (*p) 
	{
        	parent = *p;
        	user = rb_entry(parent, struct key_user, node);
        
		if (key < user->key)
		p = &(*p)->rb_left;
		else if (key > user->key)
		p = &(*p)->rb_right;
		else
		{                        
			return user;
		}	
	}   
    
	printf("RBTreeSerach : NULL\n");
	return NULL;
}

void BRTreeErase( struct key_user *user )
{

	rb_erase(&user->node, &key_user_tree);
    	free (user );
}

void RBTreeTest()
{
	struct key_user *user;
    
    	root_key_user.key = 0;
    
    	rb_link_node(&root_key_user.node, NULL, &key_user_tree.rb_node);
    
    	rb_insert_color(&root_key_user.node, &key_user_tree);    
    
    	if ( NULL != RBTreeInsert( 2 ) )
    	{
        	printf("RBTreeTest : insert 1 ok\n");
   	}
    
    	if ( NULL != RBTreeInsert( 1 ) )
    	{
        	printf("RBTreeTest : insert 2 ok\n");
    	}

	if ( NULL != RBTreeInsert( 3 ) )
    	{
        	printf("RBTreeTest : insert 3 ok\n");
    	}

    
    	if ( NULL == RBTreeInsert( 1 ) )
    	{
        	printf("RBTreeTest : insert 1 exist\n");
    	}    
    
    	user = RBTreeSerach( 1 );
    	if ( NULL != user )
    	{
        	BRTreeErase( user );
        	printf("RBTreeTest : 1 find and delete\n");
    	}
    
    	user = RBTreeSerach( 1 );
    	if ( NULL != user )
    	{
        	BRTreeErase( user );
    	}
    	else
    	{
        	printf("RBTreeTest : can't find\n");
    	}
    
    	user = RBTreeSerach( 2 );
    	if ( NULL != user )
    	{
        	BRTreeErase( user );
        	printf("RBTreeTest : 2 find and delete\n");
    	}
	user = RBTreeSerach( 3 );
    	if ( NULL != user )
    	{
        	BRTreeErase( user );
        	printf("RBTreeTest : 3 find and delete\n");
    	}

}

int main (int argc, char* argv[])
{
	RBTreeTest();
	return 1;
}

