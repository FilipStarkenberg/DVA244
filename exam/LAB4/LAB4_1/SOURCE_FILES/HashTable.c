#define _CRT_SECURE_NO_WARNINGS // Behovs for vissa funktioner i visual studio
#include "HashTable.h"
#include "Bucket.h"
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>

	// Anvands for att markera en ledig plats i Hashtabellen


/* Denna funktion tar en nyckel och returnerar ett hash-index
dvs ett index till arrayen som �r Hashtabellen */
static int hash(Key key, int tablesize)
{
	return key % tablesize;
}

/*Leta fram�t enligt principen �ppen adressering
 Antalet krockar returneras via pekaren col i parameterlistan*/
static int linearProbe(const HashTable* htable, Key key, unsigned int *col)
{
    int keyHash = hash(key, getSize(htable));
    *col = 0;
    while(1){
        if(htable->table[keyHash].key == key || htable->table[keyHash].key == UNUSED){
            return keyHash;
        }
        keyHash = (keyHash + 1) % getSize(htable);
        (*col)++;
        if(*col > getSize(htable)){
            printf("Hash table is full!\n");
            //printHashTable(htable);
            return -1;
        }
    }
    printf("Error in linear probe\n");
    return -1;
}

/*Allokera minne f�r hashtabellen*/
HashTable createHashTable(unsigned int size)
{
    HashTable htable;
    htable.size = size;
    struct Bucket* temp = (struct Bucket*)malloc(sizeof(struct Bucket)*size);
    if(temp != NULL){
        htable.table = temp;
        for (int i = 0; i < size; i++)
        {
            htable.table[i].key = UNUSED;
        }
        
        return htable;
    }
    printf("Memmory allocation failed, exiting with code 1.\n");
    exit(1);

}

//EXAMINATION
HashTable changeHashTableSize(HashTable *htable, int newSize){

    assert(htable != NULL); //Make sure htable not null
    assert(newSize > 0);// Make sure the new size is greater than 0

    int elementsInOriginal = getElementsInTable(htable);        // How many elements are in the original htable. 

    // if we are trying to set size to same size as before or size < ammount of elements => Do nothing. 
    if(newSize < elementsInOriginal || newSize == htable->size){ 
        return *htable;
    }
    
    HashTable newTable = createHashTable(newSize); // Create new table

    // Insert from old to new htable
    for(int i = 0; i < htable->size; i++){
        if(htable->table[i].key != UNUSED){
            insertElement(&newTable, htable->table[i].key, htable->table[i].value);
        }
    }

    //Free old table
    freeHashTable(htable);

    assert(newTable.size == newSize); // make sure new size is correct
    assert(elementsInOriginal == getElementsInTable(&newTable)); // make sure we still have same amount of elements as before
    return newTable;

}

//EXAMINATION
//Get the number of elements in hashtable
int getElementsInTable(const HashTable *htable){
    if(htable->table == NULL){
        return 0;
    }
    int counter = 0;
    for(int i = 0; i < htable->size; i++){
        if(htable->table[i].key != UNUSED){
            counter++;
        }
    }
    return counter;
}

/* Satter in paret {key,data} i Hashtabellen, om en nyckel redan finns ska vardet uppdateras */
/* Returnerar antalet krockar (som r�knas i linearProbe() )*/
unsigned int insertElement(HashTable* htable, const Key key, const Value value)
{
    struct Bucket bucket;
    bucket.key = key;
    bucket.value = value;
    unsigned int col;
    int index = linearProbe(htable, key, &col);
    //printf("Linear probe returned: %d\n", index);
    if(index != -1){
        htable->table[index] = bucket;
    }

    assert(lookup(htable, key) != NULL);
    return col; //Ersatt med ratt varde
}

/* Tar bort datat med nyckel "key" */
void deleteElement(HashTable* htable, const Key key)
{
    unsigned int col;
    int index = linearProbe(htable, key, &col);
    if(index != -1){
        htable->table[index].key = UNUSED;
        while(1){
            index = (index + 1) % getSize(htable);
            if(htable->table[index].key != UNUSED){
                Key keyTemp = htable->table[index].key;
                htable->table[index].key = UNUSED;
                insertElement(htable, keyTemp, htable->table[index].value);
            }
            else{
                return;
            }
        }
    }

    assert(NULL == lookup(htable, key));
}

/* Returnerar en pekare till vardet som key ar associerat med eller NULL om ingen sadan nyckel finns */
const Value* lookup(const HashTable* htable, const Key key)
{
    unsigned int col;
    int index = linearProbe(htable, key, &col);
    if(index != -1){
        if(htable->table[index].key != UNUSED)
            return &(htable->table[index].value);
    }
    return NULL;
}


/* Tommer Hashtabellen */
void freeHashTable(HashTable* htable)
{
	free(htable->table);
    htable->table = NULL;
    htable->size = 0;
}

/* Ger storleken av Hashtabellen */
unsigned int getSize(const HashTable* htable)
{
    return htable->size; 
}

/* Denna for att ni enkelt ska kunna visualisera en Hashtabell */
void printHashTable(const HashTable* htable)
{
    for (int i = 0; i < getSize(htable); i++)
    {
        //printf("Index: %d, Key: %d", i, htable->table[i].key);
        printPerson(&(htable->table[i].value), i);
    }
    
	// Tips: anvand printPerson() i Person.h for att skriva ut en person
}
