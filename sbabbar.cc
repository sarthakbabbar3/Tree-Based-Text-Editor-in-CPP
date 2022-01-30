
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>

class text_t;
extern text_t *create_text();
extern int set_max_length(text_t *txt, int max_length);
extern int length_text(text_t *txt);
extern int get_line_size(text_t *txt, int index);
extern char *get_line(text_t *txt, int index);
extern void append_line(text_t *txt, char *new_line);
extern char *set_line(text_t *txt, int index, char *new_line);
extern void insert_line(text_t *txt, int index, char *new_line);
extern char *delete_line(text_t *txt, int index);
extern void append_to_line(text_t *txt, char *new_substring);



struct text_t { int      key;
                struct text_t    *left;
                struct text_t   *right;
                int num_left;  //no. of leaf nodes in the left subtree
                int num_right; //no. of leaf nodes in the right subtree
                int max_left;  //maximum amount of space available in any node of the left subtree
                int max_right; //maximum amount of space available in any node of the right subtree
                int height;
                int max_length;
               /* possibly additional information */ };



#define BLOCKSIZE 256

text_t *currentblock = NULL;
int size_left;
text_t *free_list = NULL;
int nodes_taken = 0;
int nodes_returned = 0;

//Creating Stack Object
typedef struct st_t
{ text_t *item;
  struct st_t *next; } stack_t;


text_t *get_node()
{
  text_t *tmp;
  nodes_taken += 1;

  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> right;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock = (text_t *) malloc( BLOCKSIZE * sizeof(text_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}

void return_node(text_t *node)
{  node->right = free_list;
   free_list = node;
   nodes_returned +=1;
}

//Creating Stack and related functions

stack_t *create_stack(void)
{
  stack_t *st;
  st = (stack_t *)get_node();
  st->next = NULL;
  return( st );
}

int stack_empty(stack_t *st)
{ return( st->next == NULL );
}

void push( text_t *x, stack_t *st)
{ stack_t *tmp;
tmp = (stack_t *)get_node();
tmp->item = x;
tmp->next = st->next;
st->next = tmp;
}

text_t* pop(stack_t *st)
{ stack_t *tmp; text_t *tmp_item;
tmp = st->next;
st->next = tmp->next;
tmp_item = tmp->item;
return_node( (text_t *)tmp );
return( tmp_item );
}

text_t* top_element(stack_t *st)
{ return( st->next->item );
}

void remove_stack(stack_t *st)
{ stack_t *tmp;
do
{ tmp = st->next;
  return_node((text_t *)st);
st = tmp;
}
while ( tmp != NULL );
}

void update_num_child(text_t *node)
{
    node->num_left = node->left->num_left + node->left->num_right;
    node->num_right = node->right->num_left + node->right->num_right;
}

int max(int num1, int num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

/*Function to update the max_left and max_right attributes of a node*/
void update_max_space(text_t *node)
{
    node->max_left = max(node->left->max_left, node->left->max_right);
    node->max_right = max(node->right->max_left,node->right->max_right);
}

void left_rotation(text_t *n)
{   text_t *tmp_node;
    int tmp_key;
    tmp_node = n->left;
    tmp_key = n->key;
    n->left = n->right;
    n->key = n->right->key;
    n->right = n->left->right;
    n->left->right = n->left->left;
    n->left->left = tmp_node;
    n->left->key = tmp_key;

    update_num_child(n->left);
    update_num_child(n);
    update_max_space(n->left);
    update_max_space(n);

}

void right_rotation(text_t *n)
{   text_t *tmp_node;
    int tmp_key;
    tmp_node = n->right;
    tmp_key = n->key;
    n->right = n->left;
    n->key = n->left->key;
    n->left = n->right->left;
    n->right->left = n->right->right;
    n->right->right = tmp_node;
    n->right->key = tmp_key;

    update_num_child(n->right);
    update_num_child(n);
    update_max_space(n->right);
    update_max_space(n);
}

///////////////////////////////////////////

void print_tree(text_t *tree)
{

    if(tree == NULL){
        return;
    }

    if (tree->right == NULL)
        printf("\object is %s, maxlen = %d ,obj length is = %d, number of left child = %d, number of right child = %d\n, max space left = %d, max space right = %d\n",(char *)tree->left,(int)tree->max_length,strlen((char *)tree->left),tree->num_left,tree->num_right,tree->max_left,tree->max_right);


    if (tree->right != NULL){
        printf("\n maxlen = %d, number of left child = %d, number of right child = %d max space left = %d, max space right = %d\n\n",tree->max_length,tree->num_left,tree->num_right,tree->max_left,tree->max_right);
        print_tree(tree->left);
        print_tree(tree->right);
    }
}




/////////////////////////////////////////////////////

char * get_line(text_t *tree, int index)
{
    // -1 since there is an end line marker
    if(index > length_text(tree))
        return NULL;

    else
    {   text_t *tmp_node = tree;

        while( tmp_node->right != NULL )
        {
            if( index <= tmp_node->num_left )
                tmp_node = tmp_node->left;

            else
            {   index = index - tmp_node->num_left;
                tmp_node = tmp_node->right;
            }

        }

        return (char*)tmp_node->left;
    }
}

char * set_line( text_t *tree, int index, char * new_line)
{
    if(index > length_text(tree))
        return NULL;

    else
    {   text_t *tmp_node = tree;

        while( tmp_node->right != NULL )
        {
            if( index <= tmp_node->num_left )
                tmp_node = tmp_node->left;

            else
            {   index = index - tmp_node->num_left;
                tmp_node = tmp_node->right;
            }

        }

        char *old_line = (char *)tmp_node->left;
        tmp_node->left = (text_t *)new_line;
        tmp_node->max_left = tmp_node->max_length-strlen(new_line);
        return old_line;

    }

}

////Functions Append to line and helper functions

//finds the earliest line with space at least as much as "space"


void append_to_line(text_t *tree, char *new_substring)
{
    //finding appropriate node with enough space
    text_t *tmp_node = tree;
    int space = strlen(new_substring);
    stack_t *st = create_stack();

    if (tmp_node->max_left < space && tmp_node->max_right < space)
    {
      //printf("Node not found\n");
      append_line(tree,new_substring);
      return;
    }

    while (tmp_node->right != NULL)
    {
        push(tmp_node,st);

        if (space <= tmp_node->max_left)
        {
            tmp_node = tmp_node->left;
        }

        else if (space <= tmp_node->max_right)
        {
            tmp_node = tmp_node->right;
        }

    }


    //printf("Inserting in node %s",tmp_node->left);

    char *new_string = new char[strlen((char *)tmp_node->left) + strlen(new_substring) + 1];
    strcpy(new_string, (char *)tmp_node->left);
    strcat(new_string, new_substring);

    tmp_node->left = (text_t *)new_string;
    tmp_node->max_left = tmp_node->max_length - strlen((char *)tmp_node->left);

    //updating max_left and max_right of ancestors

    while( !stack_empty(st))
    {
        tmp_node = pop(st);
        update_max_space(tmp_node);

    }

}


/* Function to insert line at the end of the text */
void append_line(text_t *tree, char *new_object)
{
    if (strlen(new_object) > tree->max_length)
        return ;

    int num_lines = length_text(tree);
    int index = num_lines+1;

    insert_line(tree,index,new_object);
}

void insert_line(text_t *tree, int index, char *new_object)
{
    if (strlen(new_object) > tree->max_length)
        return;

    text_t *tmp_node;
    int finished;
    int num_lines = length_text(tree);

    if (index > num_lines)
        index = num_lines+1;

    if( tree->left == NULL )
    {   tree->left = (text_t *) new_object;
        tree->num_left = 1;
        tree->num_right = 0;
        tree->max_left = tree->max_length - strlen(new_object);
        tree->max_right = 0;
        tree->height = 0;
        tree->right = NULL;
    }

    else
    {
        stack_t *st;
        st = create_stack();
        tmp_node = tree;

        while( tmp_node->right != NULL )
        {   push(tmp_node,st);

            if( index <= tmp_node->num_left )
            tmp_node = tmp_node->left;

            else
            {   index = index - tmp_node->num_left;
                tmp_node = tmp_node->right;
            }

        }

    {   text_t *old_leaf, *new_leaf;
        old_leaf = get_node();
        old_leaf->left = tmp_node->left;
        //old_leaf->key = tmp_node->key;
        old_leaf->right = NULL;
        old_leaf->height = 0;

        old_leaf->num_left = 1;
        old_leaf->num_right = 0;
        old_leaf->max_left = tmp_node->max_left;
        old_leaf->max_right = tmp_node->max_right;

        new_leaf = get_node();
        new_leaf->left = (text_t *)new_object;
        //new_leaf->key = new_key;
        new_leaf->right = NULL;
        new_leaf->height = 0;

        new_leaf->num_left = 1;
        new_leaf->num_right = 0;
        new_leaf->max_left = tree->max_length-strlen(new_object);
        new_leaf->max_right = 0;

        new_leaf->max_length = tree->max_length;
        old_leaf->max_length = tree->max_length;

        //Inserting new node before the old node
        tmp_node->left = new_leaf;
        tmp_node->right = old_leaf;

        update_num_child(tmp_node);
        update_max_space(tmp_node);
        tmp_node->height = 1;
    }

    /* rebalance */
    finished = 0;

    while( !stack_empty(st) && !finished )
    {
        int tmp_height, old_height;

        tmp_node = pop(st);

        update_num_child(tmp_node);
        update_max_space(tmp_node);

        //printf("Rebalancing %d,%d",tmp_node->num_left,tmp_node->num_right);

        old_height= tmp_node->height;

        if( tmp_node->left->height - tmp_node->right->height == 2 )
        {
            if( tmp_node->left->left->height -tmp_node->right->height == 1 )
            { right_rotation( tmp_node );
            tmp_node->right->height =
            tmp_node->right->left->height + 1;
            tmp_node->height =
            tmp_node->right->height + 1;
            }
            else
            { left_rotation( tmp_node->left );
            right_rotation( tmp_node );
            tmp_height =
            tmp_node->left->left->height;
            tmp_node->left->height =
            tmp_height + 1;
            tmp_node->right->height =
            tmp_height + 1;
            tmp_node->height = tmp_height + 2;
            }
        }
        else if( tmp_node->left->height - tmp_node->right->height == -2 )
        {   if( tmp_node->right->right->height - tmp_node->left->height == 1 )
            { left_rotation( tmp_node );
            tmp_node->left->height =
            tmp_node->left->right->height + 1;
            tmp_node->height =
            tmp_node->left->height + 1;
            }
            else
            { right_rotation( tmp_node->right );
            left_rotation( tmp_node );
            tmp_height =
            tmp_node->right->right->height;

            tmp_node->left->height =
            tmp_height + 1;
            tmp_node->right->height =
            tmp_height + 1;
            tmp_node->height = tmp_height + 2;
            }
        }
        else /* update height even if there
        was no rotation */
        {   if( tmp_node->left->height > tmp_node->right->height )
            tmp_node->height =tmp_node->left->height + 1;

            else
            tmp_node->height = tmp_node->right->height + 1;
        }

        if( tmp_node->height == old_height )
        finished = 1;
    }

    //Update the number of children of elements left in stack
    while( !stack_empty(st))
        {
            tmp_node = pop(st);
            //printf("Rebalancing %d,%d",tmp_node->num_left,tmp_node->num_right);
            update_num_child(tmp_node);

        }


    remove_stack(st);
    }

}



char *delete_line(text_t *tree, int index)
{
   if(index > length_text(tree))
        return NULL;

   text_t *tmp_node, *upper_node, *other_node;
   int finished;
   char *deleted_object;

   // printf("yes");
   if( tree->left == NULL )
      return( NULL );

   else if( tree->right == NULL )
   {
     //printf("yes2");
     deleted_object = (char *) tree->left;
     tree->left = NULL;

     tree->num_left = 0;
     tree->num_right = 0;
     tree->max_left = 0;
     tree->max_right = 0;

     return( deleted_object );
   }


   else
   {
       //printf("yes3");
        tmp_node = tree;
        stack_t *st;
        st = create_stack();

        while( tmp_node->right != NULL )
        {   push(tmp_node,st);
            upper_node = tmp_node;

            if( index <= tmp_node->num_left )
               {tmp_node = tmp_node->left;
                 other_node = upper_node->right;
               }

            else
            {   index = index - tmp_node->num_left;
                tmp_node = tmp_node->right;
                other_node = upper_node->left;
            }

        }

      {
          //printf("yes4");

         upper_node->left  = other_node->left;
         //printf("yes8");
         upper_node->right = other_node->right;
         upper_node->height = 0;

        // printf("yes7");
         upper_node->num_left = other_node->num_left;
         upper_node->num_right = other_node->num_right;
         upper_node->max_left = other_node->max_left;
         upper_node->max_right = other_node->max_right;


           // printf("yes5");
         pop(st); //popping the upper_node from stack since it is already balanced
         deleted_object = (char *) tmp_node->left;
         return_node( tmp_node );
         return_node( other_node );

       // printf("yes6");
        /* rebalance */
        finished = 0;
        while( !stack_empty(st) && !finished )
        {
            int tmp_height, old_height;

            tmp_node = pop(st);

            update_num_child(tmp_node);
            update_max_space(tmp_node);

            //printf("Rebalancing!");
            //printf("\n node %d new left = %d and new right %d \n",tmp_node->key,tmp_node->num_left,tmp_node->num_right);

            old_height= tmp_node->height;

            if( tmp_node->left->height - tmp_node->right->height == 2 )
            {
                if( tmp_node->left->left->height -tmp_node->right->height == 1 )
                { right_rotation( tmp_node );
                tmp_node->right->height =
                tmp_node->right->left->height + 1;
                tmp_node->height =
                tmp_node->right->height + 1;
                }
                else
                { left_rotation( tmp_node->left );
                right_rotation( tmp_node );
                tmp_height =
                tmp_node->left->left->height;
                tmp_node->left->height =
                tmp_height + 1;
                tmp_node->right->height =
                tmp_height + 1;
                tmp_node->height = tmp_height + 2;
                }
            }
            else if( tmp_node->left->height - tmp_node->right->height == -2 )
            {   if( tmp_node->right->right->height - tmp_node->left->height == 1 )
                { left_rotation( tmp_node );
                tmp_node->left->height =
                tmp_node->left->right->height + 1;
                tmp_node->height =
                tmp_node->left->height + 1;
                }
                else
                { right_rotation( tmp_node->right );
                left_rotation( tmp_node );
                tmp_height =
                tmp_node->right->right->height;

                tmp_node->left->height =
                tmp_height + 1;
                tmp_node->right->height =
                tmp_height + 1;
                tmp_node->height = tmp_height + 2;
                }
            }
            else /* update height even if there was no rotation */
            {   if( tmp_node->left->height > tmp_node->right->height )
                tmp_node->height =tmp_node->left->height + 1;

                else
                tmp_node->height =tmp_node->right->height + 1;
            }

            if( tmp_node->height == old_height )
            finished = 1;
        }

            //Update the number of children of elements left in stack
        while( !stack_empty(st))
        {
            tmp_node = pop(st);
            update_num_child(tmp_node);
            update_max_space(tmp_node);
        }
        remove_stack(st);
      }

       return( deleted_object );
    }
}

void remove_tree(text_t *tree)
{  text_t *current_node, *tmp;
   if( tree->left == NULL )
      return_node( tree );
   else
   {  current_node = tree;
      while(current_node->right != NULL )
      {  if( current_node->left->right == NULL )
         {  return_node( current_node->left );
            tmp = current_node->right;
            return_node( current_node );
            current_node = tmp;
         }
         else
         {  tmp = current_node->left;
            current_node->left = tmp->right;
            tmp->right = current_node;
            current_node = tmp;
         }
      }
      return_node( current_node );
   }
}

int length_text(text_t *tree)
{

    if (tree == NULL){
        return -1;
    }

    return tree->num_left+tree->num_right-1;
}

text_t *create_text(void)
{
   text_t *tmp_node = (text_t *) malloc(sizeof(text_t));
   tmp_node = get_node();
   tmp_node->left = NULL;
   tmp_node->num_left = 0;
   tmp_node->num_right = 0;
   char *end_marker = "";
   insert_line(tmp_node,1,end_marker);
   tmp_node->max_left = 0;
   tmp_node->max_right = 0;
   return( tmp_node );
}

int set_max_length(text_t *txt, int max_length)
{
    txt->max_length = max_length;
    return txt->max_length;
}

int get_line_size(text_t *txt, int index)
{
    if (length_text(txt) < index)
        return -1;

    else
    {
        char *line = get_line(txt,index);
        return strlen(line);
    }


}

