#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct Node {
    int value;
    struct Node *next;
} Node;

typedef struct Set {
    int minSize;
    int maxSize;
    Node **array;
    int length;
} Set;

Set init(int N) {
    Set set;
    set.minSize = N;
    set.maxSize = N;
    set.array = (Node **)malloc(N*sizeof(Node *));
    for (int i = 0; i < N; i++){
        set.array[i] = NULL;
    }
    set.length = 0;
    return set;
}

Set copy(Set set){
    Set new;
    new.minSize = set.minSize;
    new.maxSize = set.maxSize;
    new.array = (Node **)malloc(set.maxSize*sizeof(Node *));
    for (int i = 0; i < set.maxSize; i++) {
        new.array[i] = set.array[i];
    }
    new.length = set.length;
    return new;
}

void clear(Set *set) {
    Node *prev, *curr;
    for (int i = 0; i < set->maxSize; i++) {
        prev=NULL;
        curr = set->array[i];
        while(curr!=NULL){
            prev=curr;
            curr=curr->next;
            free(prev);
        }
        set->array[i]=NULL;
    }
    set->length=0;
}

void delete(Set *set) {
    Node *prev, *curr;
    for (int i = 0; i < set->maxSize; i++) {
        prev=NULL;
        curr = set->array[i];
        while(curr!=NULL){
            prev=curr;
            curr=curr->next;
            free(prev);
        }
    }
    set->minSize=0;
    set->maxSize=0;
    free(set->array);
    set->array=NULL;
    set->length=0;
}

int hash(int key, int maxSize) {
    float k=(pow(5,0.5)-1)/2;
    return (int)floor(maxSize*fmod(k*key,1));
}

float loadFactor(Set set){
    return (float)set.length/set.maxSize;
}

void rehash(Set *set){
    float currentLoadFactor=loadFactor(*set);
    int newSize;
    if(currentLoadFactor>=0.75)
        newSize=set->maxSize*2;
    else if(currentLoadFactor<=0.25 && set->minSize<set->maxSize)
        newSize=set->minSize>set->maxSize/2?set->minSize:set->maxSize/2;
    else return;
    
    Set newSet=init(newSize);
    newSet.minSize=set->minSize;
    for (int i = 0; i < set->maxSize; i++) {
        Node *curr = set->array[i];
        while(curr!=NULL){
            int index=hash(curr->value,newSize);
            Node *newNode = (Node *)malloc(sizeof(Node));
            newNode->value=curr->value;
            newNode->next=newSet.array[index];
            newSet.array[index]=newNode;
            newSet.length++;
            curr=curr->next;
        }
    }
    delete(set);
    *set=newSet;
}

bool isIn(Set set, int value) {
    int index = hash(value,set.maxSize);
    Node* curr = set.array[index];
    while(curr!=NULL){
        if(curr->value==value) return true;
        curr=curr->next;
    }
    return false;
}

void insert(Set *set, int value) {
    int index = hash(value,set->maxSize);
    Node *curr = set->array[index];

    while(curr!=NULL){
        if(curr->value==value) return;
        curr=curr->next;
    }

    Node *new = (Node *)malloc(sizeof(Node));
    new->value=value;
    new->next=set->array[index];
    set->array[index]=new;
    set->length++;

    rehash(set);
}

void discard(Set *set, int value) {
    int index = hash(value,set->maxSize);
    Node* prev = NULL;
    Node* curr = set->array[index];

    while(curr!=NULL){
        if(curr->value==value){
            if(prev==NULL){
                set->array[index]=curr->next;
            }else{
                prev->next=curr->next;
            }
            free(curr);
            set->length--;
            break;
        }
        prev=curr;
        curr=curr->next;
    }

    rehash(set);
}

void traverse(Set set) {
    printf("{");
    Node *curr;
    for (int i = 0; i < set.maxSize; i++) {
        curr = set.array[i];
        while(curr!=NULL){
            printf(" %d",curr->value);
            curr=curr->next;
        }
    }
    printf(" } : %.2f\n",loadFactor(set));
}

Set unionOf(Set set1, Set set2) {
    Set result = init(set1.maxSize+set2.maxSize);
    Node *curr;
    for (int i = 0; i < set1.maxSize; i++) {
        curr = set1.array[i];
        while (curr != NULL){
            insert(&result, curr->value);
            curr = curr->next;
        }
    }
    for (int i = 0; i < set2.maxSize; i++) {
        curr = set2.array[i];
        while (curr != NULL) {
            if (!isIn(result, curr->value))
                insert(&result, curr->value);
            curr = curr->next;
        }
    }
    return result;
}

Set intersectionOf(Set set1, Set set2) {
    Set result = init(set1.maxSize<set2.maxSize?set1.maxSize:set2.maxSize);
    Node *curr;
    for (int i = 0; i < set1.maxSize; i++) {
        curr = set1.array[i];
        while (curr != NULL) {
            if (isIn(set2, curr->value)) {
                insert(&result, curr->value);
            }
            curr = curr->next;
        }
    }
    return result;
}

Set differenceOf(Set set1, Set set2) {
    Set result = init(set1.maxSize);
    Node *curr;
    for (int i = 0; i < set1.maxSize; i++) {
        curr = set1.array[i];
        while (curr != NULL) {
            if (!isIn(set2, curr->value)) {
                insert(&result, curr->value);
            }
            curr = curr->next;
        }
    }
    return result;
}

Set symmetricDifferenceOf(Set set1, Set set2) {
    Set result = init(set1.maxSize + set2.maxSize);
    Node *curr;
    for (int i = 0; i < set1.maxSize; i++) {
        curr = set1.array[i];
        while (curr != NULL) {
            if (!isIn(set2, curr->value)) {
                insert(&result, curr->value);
            }
            curr = curr->next;
        }
    }
    for (int i = 0; i < set2.maxSize; i++) {
        curr = set2.array[i];
        while (curr != NULL) {
            if (!isIn(set1, curr->value)) {
                insert(&result, curr->value);
            }
            curr = curr->next;
        }
    }
    return result;
}

bool isSubset(Set set1, Set set2) {
    if(set1.length>set2.length) return false;
    Node *curr;
    for (int i = 0; i < set1.maxSize; i++) {
        curr = set1.array[i];
        while(curr!=NULL){
            if(!isIn(set2,curr->value)){
                return false;
            }
            curr=curr->next;
        }
    }
    return true;
}

bool isProperSubset(Set set1, Set set2) {
    if(set1.length>=set2.length) return false;
    Node *curr;
    for (int i = 0; i < set1.maxSize; i++) {
        curr = set1.array[i];
        while(curr!=NULL){
            if(!isIn(set2,curr->value)){
                return false;
            }
            curr=curr->next;
        }
    }
    return true;
}

bool isSuperset(Set set1, Set set2) {
    return isSubset(set2,set1);
}

bool isProperSuperset(Set set1, Set set2) {
    return isProperSubset(set2,set1);
}

bool isDisjoint(Set set1, Set set2) {
    Node *curr;
    for (int i = 0; i < set1.maxSize; i++) {
        curr = set1.array[i];
        while(curr!=NULL){
            if(isIn(set2,curr->value)){
                return false;
            }
            curr=curr->next;
        }
    }
    return true;;
}

bool isEqual(Set set1, Set set2){
    if(set1.length!=set2.length) return false;
    Node *curr;
    for (int i = 0; i < set1.maxSize; i++) {
        curr = set1.array[i];
        while(curr!=NULL){
            if(!isIn(set2,curr->value)){
                return false;
            }
            curr=curr->next;
        }
    }
    return true;
}

void describe(Set set) {
    Node *curr;
    for (int i = 0; i < set.maxSize; i++) {
        printf("%2d | ",i);
        curr = set.array[i];
        while(curr!=NULL){
            printf("{%d} ",curr->value);
            curr=curr->next;
            if(curr!=NULL) printf(" -> ");
        }
        printf("\n");
    }
}

int main() {
    Set set1 = init(10);

    // insert(&set1,1);
    // insert(&set1,3);
    insert(&set1,5);
    insert(&set1,7);
    // insert(&set1,9);
    insert(&set1,11);
    insert(&set1,13);
    // insert(&set1,15);

    traverse(set1);
    describe(set1);

    Set set2 = init(10);

    insert(&set2,1);
    insert(&set2,3);
    insert(&set2,9);
    insert(&set2,15);

    traverse(set2);
    describe(set2);

    printf(">%d",isDisjoint(set1,set2));

    return 0;
}