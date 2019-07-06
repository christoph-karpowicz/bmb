#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node Node;

struct Node {
    int Value;
    Node *Next;
};

Node *CreateNode(int value)
{
    Node *node = (Node*) malloc(sizeof(Node));
    node->Value = value;
    node->Next = node;
    return node;
}

Node *GetTail(Node* root)
{
    Node *tail_node = root;
    while (tail_node->Next != root) {
        tail_node = tail_node->Next;
    }
    return tail_node;
}

void AddNode(Node* root, int value) 
{
    Node *new_Node = CreateNode(value);
    GetTail(root)->Next = new_Node;
    new_Node->Next = root;
}

void PrintList(Node* root) 
{
    if (root->Next)
    {
        Node *list_tail = GetTail(root);
        Node *current_Node = root;
        int done = 0;
        do 
        {
            if (current_Node == list_tail) done = 1; 
            printf("%d%s", current_Node->Value, (done == 1) ? "" : " -> ");
            current_Node = current_Node->Next;
        }
        while (done == 0);
    }
    else {
        printf("Lista jest pusta.");
    }
    printf("\n");
}

void RemoveNode(Node* root, int value) 
{
    Node *list_tail = GetTail(root);
    Node *previous_Node = NULL;
    Node *current_Node = root;
    int deleted = 0, pos = 1;
    while (true) {
        if (current_Node->Value == value) 
        {
            previous_Node->Next = current_Node->Next;
            free(current_Node);
            current_Node->Next = NULL;
            deleted = 1;
            break;
        }
        if (current_Node == list_tail) break;
        else
        {
            previous_Node = current_Node;
            current_Node = current_Node->Next;
            pos++;
        }
    }

    // if (deleted != 0) printf("Removed node with value %d, from position %d.\n", value, pos);
    // else printf("Node with value %d not found.\n", value);
}

void FreeList(Node* root)
{
    // printf("FreeList();\n");
    Node *list_tail = GetTail(root);
    Node *previous_Node = list_tail;
    Node *tmp_Node = NULL;
    int pos = 0;
    
    do
    {
        pos++;
        tmp_Node = previous_Node->Next;
        // printf("Memory freed after element with value: %d, from position %d.\n", previous_Node->Next->Value, pos);
        free(previous_Node->Next);
        previous_Node->Next = NULL;
        previous_Node = tmp_Node;
    }
    while (previous_Node != list_tail);
}

int Count(Node *root)
{
    int count = 0;
    Node *list_tail = GetTail(root);
    Node *current_Node = root;
    while (true) {
        count++;
        if (current_Node == list_tail) break;
        current_Node = current_Node->Next;
    }
    return count;
}

void SaveList(Node *root, char filename[])
{
    FILE *file;
    file = fopen(filename, "w+");
    Node *list_tail = GetTail(root);
    Node *current_Node = root;
    while (true) {
        fprintf(file, "%d ", current_Node->Value);
        if (current_Node == list_tail) break;
        current_Node = current_Node->Next;
    }
    FreeList(root);
    fclose(file);
}

Node *SortList(Node *root)
{
    Node *list_tail = GetTail(root);
    int Node_count = Count(root);
    Node *Node_array_assorted[Node_count];
    Node *Node_array_sorted[Node_count];
    Node *current_Node = NULL;
    Node *tmp_Node = NULL;

    // Save all node addresses in an array.
    int i, j;
    for (i = 1; i <= Node_count; i++)
    {
        current_Node = root;
        for (j = 1; j <= i; j++)
        {
            tmp_Node = current_Node;
            current_Node = current_Node->Next;
        }
        Node_array_assorted[i - 1] = tmp_Node;
    }

    // Sort "Node_array_assorted" while moving its values to a new, sorted "Node_array_sorted" array.
    int highest_index = -1;
    for (i = 0; i < Node_count; i++)
    {
        for (j = 0; j < Node_count; j++)
        {
            if (highest_index < 0 && Node_array_assorted[j] != NULL) highest_index = j;
            if (Node_array_assorted[j] != NULL && Node_array_assorted[j]->Value < Node_array_assorted[highest_index]->Value) highest_index = j;
        }
        Node_array_sorted[i] = Node_array_assorted[highest_index];
        Node_array_assorted[highest_index] = NULL;
        highest_index = -1;
    }

    // Adjust relations between nodes.
    int next_position;
    for (i = 0; i < Node_count; i++)
    {
        next_position = i == Node_count - 1 ? 0 : i + 1;
        Node_array_sorted[i]->Next = Node_array_sorted[next_position];
    }

    // Change root address to the address of the first node in the array.
    root = Node_array_sorted[0];

    // Return new root address.
    return root;
}