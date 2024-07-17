/*
FUNNY LIST LIBRARY
by Emika

BEWARE:
when you create a list, and then reassign it, *pleaseee* remember to free the list before doing that ;-;
it's like.. uhh.. the elements in the list aren't really replaced.. the elements keep existing, floating in the void, unfindable....
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE_FOR_READLIST 10000
#define MAX_SIZE_FOR_READITEM 100
/* I hope this is large enough for most cases :/ 
   feel free to change it though, if you need longer outputs */

typedef struct ListItem {
	struct ListItem* successor;
	char* data;
} ListItem;

typedef struct List {
	int data_size;
	struct ListItem* root;
} List;


/*
remove duplicates
search for value
*/



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

char*     readType            (void* pointer, int length, char* datatype);





List* createList(int data_size) {
	List* list = (List*) malloc (sizeof(List));
	if(list == NULL) {
		printf("Error in createList: memory allocation failed for some reason I've never had this happen but :þ");
		exit(1);
	}
	list->data_size = data_size;
	list->root = NULL;
	return list;
}

List* createListFromItem(int data_size, void* data) {
	List* list = createList(data_size);
	append(list, data);
	return list;
}

List* createListFromArray(int data_size, void* data, int num_of_data_points) {
	List* list = createList(data_size);
	extendByArray(list, data, num_of_data_points);
	return list;
}

ListItem* createListItem(void* data, int data_size) {
	if(data == NULL) {
		printf("Error at createListItem: the data pointer is null.");
		exit(1);
	}
	ListItem* new_item = (ListItem*) malloc (sizeof(ListItem));
	if(new_item == NULL) {
		printf("Error in createListItem: memory allocation failed for some reason I've never had this happen but :þ");
		exit(1);
	}

	new_item->successor = NULL;

	new_item->data = (char*) malloc (data_size);
	for(int i=0; i<data_size; i++) {
		new_item->data[i] = *(char*) (data + i);
	}
	return new_item;
}





List* subList (List* original_list, int start, int end) {
	if (listIsEmpty(original_list)) return createList(original_list->data_size);

	start %= listSize(original_list);
	end   %= listSize(original_list);
	if(start < 0) start += listSize(original_list);
	if(end   < 0) end   += listSize(original_list);
	if(start > end) {
		printf("Error in subList(): start (%d) > end (%d)", start, end);
		return NULL;
	}

	int amount_to_copy = end-start+1;
	List* sublist = createList(original_list->data_size);
	ListItem* item_to_clone = getListItem(original_list, start);

	for(int i=0; i<amount_to_copy; i++) {
		append(sublist, item_to_clone->data);
		item_to_clone = item_to_clone->successor;
	}

	return sublist;
}

List* cloneList (List* original_list) {
	return subList(original_list, 0, -1);
}

List* randomiseList (List* original_list) {
	List* randomised_list = cloneList(original_list);
	for(int i=listSize(randomised_list)-1; i>0; i--) swapListItems(randomised_list, i, randomised_list, rand()%i);
	return randomised_list;
}




ListItem* getListItem (List* list, int pos) {
	if (listIsEmpty(list)) {
		printf("Error at getListItem(). Cannot recover item from empty list.\n");
		return NULL;
	}

	pos %= listSize(list);
	if(pos < 0) pos += listSize(list);

	ListItem* current_item = list->root;
	while(pos != 0 && current_item->successor != NULL) { /*this ensures that if -1 (or any negative number) is given as pos argument, it takes the last one*/
		current_item = current_item->successor;
		pos--;
	}

	return current_item;
}

void* getListItemData (List* list, int pos) {
	return getListItem(list, pos) -> data;
}

void removeListItem (List* list, int pos) {
	pos %= listSize(list);
	if(pos < 0) pos += listSize(list);

	if (pos == 0) {
		ListItem* root = list->root;
		if(root != NULL) {
			list->root = root->successor;
			free(root);
			return;
		} else {
			printf("Error at removeListItem. Cannot remove item from empty list.\n");
			return;
		}
	}

	ListItem* item_before_pos = getListItem(list, pos-1);
	if(item_before_pos == NULL) {
		printf("Error at removeListItem(). getListItem() returned NULL.");
		return;
	}

	if(item_before_pos->successor == NULL) {
		printf("Error at removeListItem(). Tried to remove the item at the position right after the last item.");
		return;
	}

	ListItem* item_after_pos = item_before_pos->successor->successor;
	free (item_before_pos->successor);
	item_before_pos->successor = item_after_pos;
}

void insert (List* list, void* data, int pos) {
	if( listIsEmpty(list) ) {
		list->root = (ListItem*) {createListItem(data, list->data_size)};
		return;
	}


	ListItem* new_item = createListItem(data, list->data_size);

	if(pos == 0) {
		new_item->successor = list->root;
		list->root = new_item;
		return;
	}
	if(pos == -1) {
		append(list, data);
		return;
	}

	ListItem* item_before_pos = getListItem(list, pos-1);
	if(item_before_pos == NULL) {
		printf("Error at insert(). getListItem() returned NULL.");
		return;
	}


	new_item->successor = item_before_pos->successor;
	item_before_pos->successor = new_item;
}

void setListItem (List* list, void* data, int pos) {
	pos %= listSize(list);
	if(pos < 0) pos += listSize(list);
	if (pos >= listSize(list)) {
		printf("Error at setListItem(): tried to change position %d of the list, while the list is only %d long.\n", pos, listSize(list));
		return;
	}

	ListItem* item_to_set = getListItem(list, pos);
	for(int i=0; i<list->data_size; i++) {
		item_to_set->data[i] = ((char*) data)[i];
	}
}

void append (List* list, void* data) {
	if (listIsEmpty(list)) {
		list->root = (ListItem*){createListItem(data, list->data_size)};
	} else {
		ListItem* last_item = getListItem(list, -1);
		last_item->successor = (ListItem*){createListItem(data, list->data_size)};
	}
	return;
}

void extendByArray (List* list, void* data, int num_of_data_points) {
	for(int i=0; i<num_of_data_points; i++) append(list, data + i*(list->data_size));
}

void extendByList (List* list_one, List* list_two, int free_second_list) {
	if (list_one->data_size != list_two->data_size) {
		printf("Error at extendByList(): datatypes incompatible. The first list has data of size %d, and the second list of size %d.", list_one->data_size, list_two->data_size);
		return;
	}

	if( listIsEmpty(list_two) ) {
		if(free_second_list) freeList(list_two);
		return;
	}


	if(free_second_list) {
		if (listIsEmpty(list_one)) list_one                 ->root      = list_two->root;
		else                       getListItem(list_one, -1)->successor = list_two->root;
		free(list_two);
		return;
	} else {
		List* clone = cloneList(list_two);
		extendByList(list_one, clone, 1);
		return;
	}
}

void copyListItem (List* list_one, int pos_one, List* list_two, int pos_two) {
	if(list_one->data_size != list_two->data_size) {
		printf("Error at extendByList(): datatypes incompatible. The first list has data of size %d, and the second list of size %d.", list_one->data_size, list_two->data_size);
		return;
	}
	setListItem(list_two, getListItem(list_one, pos_one)->data, pos_two);
	return;
}

void swapListItems (List* list_one, int pos_one, List* list_two, int pos_two) {
	if(list_one->data_size != list_two->data_size) {
		printf("Error at extendByList(): datatypes incompatible. The first list has data of size %d, and the second list of size %d.", list_one->data_size, list_two->data_size);
		return;
	}
	char data_of_second_item[list_two->data_size];
	for(int i=0; i<list_two->data_size; i++) data_of_second_item[i] = (getListItem(list_two, pos_two)->data)[i];
	copyListItem(list_one, pos_one, list_two, pos_two);
	setListItem (list_one, data_of_second_item, pos_one);
	return;
}



char* readList (List* list, char* datatype) {
	char* output = (char*) malloc(MAX_SIZE_FOR_READLIST); 

	int write_as_string = 0;
	if (strcmp(datatype, "str") == 0) write_as_string = 1;

	if (listIsEmpty(list)) {
		if(write_as_string) return "";
		else                return "{}";
	}

	int length_so_far;
	if (!write_as_string) {
		strcpy(output, "{");
		length_so_far = 1;
	} else {
		length_so_far = 0;
	}


	int list_size = listSize(list);
	ListItem* current_item = list->root;

	while (current_item->successor != NULL) {
		if(strlen(output) > MAX_SIZE_FOR_READLIST-MAX_SIZE_FOR_READITEM-2) { /* this might not catch everything but good enough :þ */
			printf("Error at readList: the output was too big, or almost too big. Consider increasing MAX_SIZE_FOR_READLIST :þ");
			return NULL;
		}

		char* this_item = readType (current_item->data, list->data_size, datatype);
		strcpy(output + length_so_far, this_item);
		length_so_far += strlen(this_item);
		free(this_item);

		current_item = current_item->successor;
		if (!write_as_string) {
			strcpy(output+length_so_far, ", ");
			length_so_far += 2;
		}
	}

	if(strlen(output) > MAX_SIZE_FOR_READLIST-MAX_SIZE_FOR_READITEM-1) { /* this might not catch everything but good enough :þ */
		printf("Error at readList: the output was too big, or almost too big. Consider increasing MAX_SIZE_FOR_READLIST :þ");
		return NULL;
	}
	char* this_item = readType (current_item->data, list->data_size, datatype);
	strcpy(output + length_so_far, this_item);
	length_so_far += strlen(this_item);
	free(this_item);

	if (!write_as_string) strcpy(output+length_so_far, "}");
	length_so_far ++;
	return output;
}


void printList (List* list, char* datatype) {
	printf("%s\n", readList(list, datatype));
}

int listSize (List* list) {
	if (listIsEmpty(list)) return 0;

	int counter = 1;
	ListItem* current_item = list->root;

	while(current_item->successor != NULL) {
		current_item = current_item->successor;
		counter ++;
	}

	return counter;
}

void freeList (List* list) {
	while (list->root != NULL) removeListItem(list, -1);
	free(list);
}




int listIsEmpty (List* list) {
	return (list->root == NULL);
}

int findListItem (List* list, void* data) {
	/*returns the earliest position of the value we're looking for. If it doesn't exist, returns -1*/
	if( listIsEmpty(list) ) return -1;

	ListItem* current_item = list->root;
	int location = 0;
	while(current_item->successor != NULL) {
		int equals = 1;
		for(int i=0; i<list->data_size; i++) {
			if(*(char*)data != *(char*)current_item->data) {
				equals = 0;
				break;
			}
		}
		if(equals) return location;
		current_item = current_item->successor;
		location++;
	}
	int equals = 1;
	for(int i=0; i<list->data_size; i++) {
		if(*(char*)data != *(char*)current_item->data) {
			equals = 0;
			break;
		}
	}
	if(equals) return location;
	else       return -1;
}

int listContains (List* list, void* data) {
	return (findListItem(list, data) != -1);
}

int listEquals (List* list_one, List* list_two) {
	/* returns -3 if the data is of different sizes,
		returns -2 if the data equals for the entire shorter list, but one of the two is longer than the other
	   returns -1 if they are equal, 
	   and otherwise returns the first index where they differ */
	if(list_one->data_size != list_two->data_size) {
		return -3;
	}

	if(listIsEmpty(list_one)) {
		if(listIsEmpty(list_two)) {
			return -1;
		}
		return -2;
	}
	if(listIsEmpty(list_two)) {
		return -2;
	}

	ListItem* current_item_one = list_one->root;
	ListItem* current_item_two = list_two->root;
	int counter = 0;

	while (current_item_one->successor != NULL && current_item_two->successor != NULL) {
		for(int i=0; i<list_one->data_size; i++) {
			if(current_item_one->data[i] != current_item_two->data[i])
				return counter;
		}

		current_item_one = current_item_one->successor;
		current_item_two = current_item_two->successor;
		counter ++;
	}

	for(int i=0; i<list_one->data_size; i++) {
		if(current_item_one->data[i] != current_item_two->data[i])
			return counter;
	}


	if(current_item_one->successor != NULL || current_item_two->successor != NULL)
		return -2;


	return -1;
}



char* readType (void* pointer, int length, char* datatype) {
	/* str, char, int, hex */
	/*-1 ~ datatype incorrect. 0 ~ no error. 1 ~ incorrect size*/
	char* output = (char*) malloc(MAX_SIZE_FOR_READLIST); 
	int warning = -1;
	int length_so_far = 0;

	if (strcmp(datatype, "char") == 0 || strcmp(datatype, "str") == 0) {
		for(int i=0; i<length; i+=sizeof(char)) {
			output[length_so_far] = *((char*) pointer + i);
			length_so_far += sizeof(char);
		}
		warning = 0;

	} else if (strcmp(datatype, "num_char") == 0) {
		for(int i=0; i<length-sizeof(char); i+=sizeof(char)) {
			char new_item[100];
			sprintf(new_item, "%d ", *(char*) (pointer + i));
			strcpy(output+length_so_far, new_item);
			length_so_far += strlen(new_item);
		}
		char new_item[100] = "";
		sprintf(new_item, "%d", * (char*) (pointer + length-sizeof(char)));
		strcpy(output+length_so_far, new_item);
		warning = 0;

	} else if (strcmp(datatype, "int") == 0) {
		char new_item[100];
		sprintf(new_item, "%d", *(int*) pointer);
		strcpy(output, new_item);
		warning = 0;
		if(length % 4 != 0) warning = 1;

	} else if (strcmp(datatype, "hex") == 0) {
		char new_item[100];
		sprintf(new_item, "%x", *(int*) pointer);
		strcpy(output, new_item);
		warning = 0;
		if(length % 4 != 0) warning = 1;

	}

	if(warning != 0) {
		if(warning == -1) printf("Warning at printType(): datatype %s not supported.", datatype);
		if(warning ==  1) printf("Warning at printType(): data of incorrect size; %d does not split into %ss.", length, datatype);
	}
	return output;
}


