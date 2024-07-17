#ifndef LIST_HEADER_H
#define LIST_HEADER_H

/*let's hope that these are large enough...*/
#define MAX_SIZE_FOR_READLIST 10000
#define MAX_SIZE_FOR_READITEM 100


typedef struct ListItem {
	struct ListItem* successor;
	char* data;
} ListItem;

typedef struct List {
	int data_size;
	struct ListItem* root;
} List;



List*     createList          (int data_size);
List*     createListFromItem  (int data_size, void* data);
List*     createListFromArray (int data_size, void* data, int num_of_data_points);
ListItem* createListItem      (void* data, int data_size);\

List*     subList             (List* original_list, int start, int end);
List*     cloneList           (List* original_list);
List*     randomiseList       (List* original_list);

ListItem* getListItem         (List* list, int pos);
void*     getListItemData     (List* list, int pos);
void      removeListItem      (List* list, int pos);
void      insert              (List* list, void* data, int pos);
void      setListItem         (List* list, void* data, int pos);
void      append              (List* list, void* data);
void      extendByArray       (List* list, void* data, int num_of_data_points);
void      extendByList        (List* list_one, List* list_two, int delete_second_list);
void      copyListItem        (List* list_one, int pos_one, List* list_two, int pos_two);
void      swapListItems       (List* list_one, int pos_one, List* list_two, int pos_two);

char*     readList            (List* list, char* datatype);
void      printList           (List* list, char* datatype);
int       listSize            (List* list);
void      freeList            (List* list);

int       listIsEmpty         (List* list);
int       findListItem        (List* list, void* data);
int       listContains        (List* list, void* data);
int       listEquals          (List* list_one, List* list_two);

char*     readType            (void* pointer, int length, char* datatype);






#endif