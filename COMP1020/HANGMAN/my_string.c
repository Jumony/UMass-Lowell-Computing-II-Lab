#include <stdio.h>
#include <stdlib.h>
#include "my_string.h"
#include <ctype.h>

struct my_string
{
    char* data;
    int size;
    int capacity;
};
typedef struct my_string My_string;

// IMPORTANT COMMENT!!!!
// Most of these functions define My_string* pString = (My_string*) hMyString for several reasons:
// 1. typecast so that it is way easier to reference the inputted parameter
// 2. Rather than creating a whole new struct, we are working with the same struct that was passed as a parameter.
// sure, we can create a whole new struct and rewrite our code to work around it but it would quickly become heavy
// and it would also become less easy to work with. But by declaring it as a pointer to a struct, we can just work
// with a single struct and manipulate that single struct rather than creating a whole new struct and copying
// the values from the old struct into the new struct.
// 3. WHENEVER POSSIBLE, SIZE IS INCREMENTED BY 1 AT THE END OF A FUNCTION TO PREPARE FOR THE NEXT.
// THIS IS A DESIGN CHOICE
MY_STRING my_string_init_default()
{
    My_string* pString = malloc(sizeof(My_string));
    if (pString == NULL)
    {
        printf("Unable to allocate memory, returning NULL");
        return NULL;
    }

    pString->capacity = 7;
    pString->size = 0;
    pString->data = (char*)malloc(sizeof(char) * pString->capacity);
    if (pString->data == NULL)
    {
        free(pString); // Free the My_string structure if data allocation fails
        printf("Unable to allocate memory, returning NULL");
        return NULL;
    }
    return pString;
}


void my_string_destroy(MY_STRING* phMy_string)
{
  // Pass by reference (passing in a pointer)
  // Sets a My_string pointer to struct (pString) to the inputted parameter. Allows us to modify the inputted parameter directly
  // By setting two pointers equal to each other, by modifying one pointer, you are effectively modifying the entire values at the address 
    My_string* pString = (My_string*) *phMy_string; //The right side of the expression equates to My_string* phMy_string
    free(pString->data);
    free(pString);
    *phMy_string = NULL;

}

/*
  void my_string_destroy2(void* phMy_string)
{
  // Does not work because we are supposed to pass in a pointer to a void* which would allow us to modify the referenced parameter.
  // Since we want to modify pString, we need to pass in a pointer to a void*
  // IN SUMMARY: we are not even working with the referenced parameter. This is currently pass by value and not pass by reference. 
  My_string* pString = (My_string*) phMy_string;
    free(pString->data);
    free(pString);
    *phMy_string = NULL;
}
*/
MY_STRING my_string_init_c_string(const char* c_string)
{
    //Finds length of string
    int length = 0;
    while (c_string[length] != '\0')
    {
        length++;
    }
    //----------------------
    My_string* pString;
    pString = (My_string*)malloc(sizeof(My_string)); // Prepares a My_string struct on the heap by allocating just enough space for all contents of it.
    if (pString == NULL)
    {
        printf("Failed to allocate memory. Returning NULL");
        return NULL;
    }
    pString->size = length;
    pString->capacity = length + 1;
    pString->data = (char*)malloc(sizeof(char) * pString->capacity); // Data contains a string with a specified size and capacity. 
    if (pString->data == NULL)
    {
        free(pString);
        printf("Failed to allocate memory. Returning NULL");
        return NULL;
    }
    for (int i = 0; i < pString->size; i++)
    {
        pString->data[i] = c_string[i];
    }
    return pString;
}

Status my_string_extraction(MY_STRING hMy_string, FILE* fp)
{
    My_string* pString = (My_string*) hMy_string;
    char c;

    if (fp == NULL || pString == NULL)
    {
        return FAILURE;
    }

    // Reset string size
    pString->size = 0;

    // Allocate memory for string data if not already allocated
    if (pString->data == NULL)
    {
        pString->data = (char*)malloc(sizeof(char) * 7);
        if (pString->data == NULL)
        {
            return FAILURE;
        }
        pString->capacity = 7;
    }

    // Read characters from file until EOF or whitespace
    while ((c = fgetc(fp)) != EOF && !isspace(c))
    {
        // Resize data buffer if needed
        if (pString->size >= pString->capacity - 1) // Leave space for null terminator
        {
            char* temp = (char*)realloc(pString->data, sizeof(char) * (pString->capacity * 2));
            if (temp == NULL)
            {
                return FAILURE;
            }
            pString->data = temp;
            pString->capacity *= 2;
        }

        // Store character in string
        pString->data[pString->size++] = c;
    }

    // Null-terminate the string
    pString->data[pString->size] = '\0';

    // Handle EOF
    if (feof(fp)) // Check for end of file
    {
        return FAILURE;
    }

    // Put back the whitespace character into the stream if it's not EOF
    if (!isspace(c) && ungetc(c, fp) == EOF)
    {
        return FAILURE;
    }

    return SUCCESS;
}

// The reason the parameter is a void* and not a void** is because
// we simply want to modify the values within the struct and not the
// struct itself. Making this into a void** is possible but then we
// will have to dereference which is an extra step
// CHECK THIS ONE FOR UNDERSTANDING
Status my_string_insertion(MY_STRING hMy_string, FILE* fp)
{
    if (fp == NULL)
    {
        return FAILURE;
    }

    // typecast to make it easier to reference
    My_string* pString = (My_string*)hMy_string;
    for (int i = 0; i < pString->size; i++)
    {
        // checks to see if we are at the end of the file
        if (fputc(pString->data[i], fp) == EOF)
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

int my_string_get_capacity(MY_STRING hMy_string)
{
    My_string* pString = (My_string*) hMy_string;
    return pString->capacity;
}

int my_string_get_size(MY_STRING hMy_string)
{
  My_string* pString = (My_string*) hMy_string;
  return pString->size;
}

int my_string_compare(MY_STRING hLeft_string, MY_STRING hRight_string)
{
    My_string* pLeftString = (My_string*) hLeft_string;
    My_string* pRightString = (My_string*) hRight_string;

    int i = 0;

    while (i < my_string_get_size(pLeftString) && i < my_string_get_size(pRightString) && pLeftString->data[i] == pRightString->data[i])
    {
        i++;
    }
    if (i < my_string_get_size(pLeftString) && i < my_string_get_size(pRightString) && pLeftString->data[i] != pRightString->data[i])
    {
        return pLeftString->data[i] - pRightString->data[i];
    }
    else if (i == my_string_get_size(pLeftString) && i == my_string_get_size(pRightString))
    {
        return 0;
    }
    return pLeftString->size - pRightString->size;
}

Status my_string_push_back(MY_STRING hString, char item)
{
    My_string* pString = (My_string*)hString;
    char* temp;
    int i;

      // checks to see if the size of the character exceeds the capacity.
      // if so, double the capacity of the struct, create a new string struct,
      // and then paste values of previous string struct into the new one which
      // has double the capacity of the old struct
    if (pString->size >= pString->capacity)
    {
        temp = (char*)malloc(sizeof(char) * pString->capacity * 2);
        if (temp == NULL)
        {
            printf("Unable to allocate memory");
            return FAILURE;
        }

        for (i = 0; i < pString->size; i++)
        {
            temp[i] = pString->data[i];
        }

        free(pString->data); // dont forget to free
        pString->data = temp; // temp is a c-string (pointer to beginning of string). data is ALSO a c-string
        pString->capacity *= 2;
    }

    pString->data[pString->size] = item;
    pString->size++;
    return SUCCESS;
}

// Should add an automatic resize function soon.
// Resize by 1/2 after size is 1/4 of capacity
Status my_string_pop_back(MY_STRING hString)
{
    My_string* pString = (My_string*) hString;

    if (pString->size == 0)
    {
        return FAILURE;
    }
    pString->size--;
    return SUCCESS;
}

//
char* my_string_at(MY_STRING hString, int index)
{
    My_string* pString = (My_string*) hString;
    if (index - 1 > pString->size || index < 0)
    {
        return NULL;
    }

    return &(pString->data[index]);
}

char* my_string_c_str(MY_STRING hString)
{
    My_string* pString = (My_string*) hString;

    if (pString->size == 0)
    {
        return NULL;
    }

    if (my_string_push_back((void*) pString, '\0') == FAILURE)
    {
      printf("Failed to end with null terminator\n");
        return NULL;
    }

    pString->size--; // simulates a c string
    return pString->data; // pString->data is char*
}

Status my_string_concat(MY_STRING hResult, MY_STRING hAppend)
{
    My_string* pString = (My_string*) hResult;
    char* temp;
    My_string* pAppend = (My_string*) hAppend;

    temp = malloc(sizeof(char) * pString->size + pAppend->size + 1);
    if (temp == NULL)
    {
        return FAILURE;
    }

    for (int i = 0; i < pString->size; i++)
    {
        temp[i] = pString->data[i];
    }

    free(pString->data);
    pString->capacity = pString->size + pAppend->size + 1;
    pString->data = temp;

    for (int i = 0; i < pAppend->size; i++)
    {
        pString->data[pString->size] = pAppend->data[i];
        pString->size++;
    }
    return SUCCESS;
}

Boolean my_string_empty(MY_STRING hMy_string)
{
    My_string* pString = (My_string*) hMy_string;

    if (pString->size <= 0)
    {
        return TRUE;
    }
    return FALSE;
}

Status my_string_assignment(MY_STRING hLeft, MY_STRING hRight)
{
    My_string* pLeft = (My_string*)hLeft;
    My_string* pRight = (My_string*)hRight;

    if (hLeft == NULL)
    {
        hLeft = my_string_init_c_string(my_string_c_str(hRight));
        if (hLeft == NULL)
        {
            return FAILURE;
        }
    }

    else
    {
        if (pLeft->capacity <= pRight->size)
        {
            char* temp = (char*)malloc(sizeof(char) * pRight->size + 1);
            if (temp == NULL)
            {
                printf("Failed to allocate memory\n");
                return FAILURE;
            }
            free(pLeft->data);
            pLeft->data = temp;
            pLeft->capacity = pRight->size + 1;
        }

        for (int i = 0; i < pRight->size; i++)
        {
            pLeft->data[i] = pRight->data[i];
        }
        pLeft->size = pRight->size;
    }
    return SUCCESS;
}

MY_STRING my_string_init_copy(MY_STRING hMy_string)
{
    My_string* pMy_string = (My_string*)hMy_string;
    My_string* pNewString = (My_string*)malloc(sizeof(My_string));
    if (pNewString == NULL)
    {
        printf("Failed to allocate memory\n");
        return NULL;
    }

    // Copies attributes of hMy_string to pNewString
    pNewString->capacity = pMy_string->capacity;
    pNewString->size = pMy_string->size;
    pNewString->data = (char*)malloc(sizeof(char) * pMy_string->capacity);
    
    if (pNewString->data == NULL)
    {
        free(pNewString);
        printf("Failed to allocate memory\n");
        return NULL;
    }

    for (int i = 0; i < pMy_string->size; i++)
    {
        pNewString->data[i] = pMy_string->data[i];
    }
    return pNewString;
}

void my_string_swap(MY_STRING hLeft, MY_STRING hRight)
{
    My_string* pLeft = (My_string*)hLeft;
    My_string* pRight = (My_string*)hRight;

    int pLSize = pLeft->size;
    int pLCapacity = pLeft->capacity;
    char* pLData = pLeft->data;

    int pRSize = pRight->size;
    int pRCapacity = pRight->capacity;
    char* pRData = pRight->data;

    pLeft->size = pRSize;
    pLeft->capacity = pRCapacity;
    pLeft->data = pRData;

    pRight->size = pLSize;
    pRight->capacity = pLCapacity;
    pRight->data = pLData;
}
