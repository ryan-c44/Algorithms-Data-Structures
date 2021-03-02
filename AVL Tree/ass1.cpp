#include<iostream>
#include<fstream>
#include<ostream>
#include<cstring>

using namespace std;

struct AVL {
    char word[100];
    int count;
    AVL *right;
    AVL *left;
};

char lowercase(char c);
int height(AVL *node);
int heightDifference(AVL *node);
AVL* rotate_right(AVL* parent_node);
AVL* rotate_left(AVL* parent_node);
AVL* double_left(AVL* parent_node);
AVL* double_right(AVL* parent_node);
AVL* balanceTree(AVL* node);
AVL* insert(AVL *root, char word[], int index, int length);
void merge(AVL arr[], int l, int m, int r);
void mergesort(AVL nodes[], int left, int right);
void printArray(AVL A[], int size);
void in_order(AVL* node);

char pool[500000];
AVL nodes[50000];
static int node_count = 0;

int main() {

    char file_name[20];
    ifstream inFile;

    int len = 0; 
    int index = 0;
    bool inword = false;
    char input;
    char text[20];
    AVL* root = NULL;

    /* Get user input for file name */
    cout << "Enter the file name: ";
    cin >> file_name;
    cout << endl;

    inFile.open(file_name);

    if(!inFile) {
        cout << "Error Opening File" << endl;
        return -1;
    } 

    while(inFile.get(input)) {

        if(inFile.eof()) {
            break;
        }

        if(isspace(input)) {

            if(inword) {
                root = insert(root, text, index, len); //insert the root node

                for(int i = 0; i < sizeof(text);i++) {
                    text[i] = '\0'; //clear the rest of the array with null byte characters
                }

                len = 0;
                inword = false; //no longer reading a word, next
            }

        } else if(isalpha(input)) { // if input is a letter
            input = lowercase(input);
            inword = true; //set bool to true as we're beginning to read a word in
            text[len++] = input; 
            pool[index++] = input;     
        }
    }

    //if length is still greater than 0 process the word
    if(len > 0) {
        root = insert(root, text, index, len); //insert the root node

        for(int i = 0; i < sizeof(text);i++) {
            text[i] = '\0'; //clear the rest of the array with null byte characters
        }

        len = 0;
        inword = false;
    }

    inFile.close();

    in_order(root);
    mergesort(nodes, 0, node_count - 1);
    printArray(nodes, node_count);

 }

/* convert to lowercase */
char lowercase(char c) {
    if(c >= 65 && c <= 90) {
        c += 32;
    }
    return c;
}

/* Get the height of the AVL Tree */
int height(AVL *node) {
    int h = 0;

    if(node != NULL) {
        int left_height = height(node->left);
        int right_height = height(node->right);
        int max_height = max(left_height, right_height);
        h = max_height + 1;
    }
        
    return h;
}

/* Get the difference in height between left and right node */
int heightDifference(AVL *node) {
    int left_height = height(node->left);
    int right_height = height(node->right);
    int balance = left_height - right_height;
    return balance;
}

/* Rotate the nodes right */
AVL* rotate_right(AVL* parent_node) {
    AVL* node;
    node = parent_node->right;
    parent_node->right = node->left;
    node->left = parent_node;
    return node;
}

/* Rotate the nodes left */
AVL* rotate_left(AVL* parent_node) {
    AVL* node;
    node = parent_node->left;
    parent_node->left = node->right;
    node->right = parent_node;
    return node;
}

/* double left rotation */
AVL* double_left(AVL* parent_node) {
    AVL* node;

    node = parent_node->left;
    parent_node->left = rotate_right(node);
    return rotate_left(parent_node);
}

/* double right rotation */
AVL* double_right(AVL* parent_node) {
    AVL* node;

    node = parent_node->right;
    parent_node->right = rotate_left(node);
    return rotate_right(parent_node);
}

AVL* balanceTree(AVL* node) {
    int bal_factor = heightDifference(node);

    if (bal_factor >= 2) {
        if (heightDifference(node->left) > 0)
            node = rotate_left(node);
        else
            node = double_left(node);
    }
    else if (bal_factor < -1) {
        if (heightDifference(node->right) > 0)
            node = double_right(node);
        else
            node = rotate_right(node);
    }
    return node;
}

AVL* insert(AVL *root, char word[], int index, int length) {

    if (root == NULL) {

        //Make a new AVL node if the tree is empty
        root = new AVL();

        memcpy(root->word, word, sizeof(root->word));
        root->right = NULL;
        root->left = NULL;
        root->count = 1;

        return root;
    }

    //Compare root->word and word to see which one is alphabetically less (Based on ASCII codes)
    else if (strcmp(word, root->word) < 0) {
        root->left = insert(root->left, word, index, length);
        root = balanceTree(root); //Balance the tree by using rotating to its new position
    //Compare root->word and word to see which one is alphabetically greater (Based on ASCII codes)
    } else if (strcmp(word, root->word) > 0) {
        root->right = insert(root->right, word, index, length);
        root = balanceTree(root); //Balance the tree by using rotating to its new position
    } 
    //Compare words to see if they are equal in value then increment the count
    if(strcmp(word, root->word) == 0) {
        root->count++;
    }

    return root;
}

void merge(AVL arr[], int l, int m, int r) { 

    int apos, bpos, cpos; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    // create temporary arrays 
    AVL LEFT[n1], RIGHT[n2]; 
  
    // Copy data to temp arrays LEFT[] and RIGHT[] 
    for (apos = 0; apos < n1; apos++)
        LEFT[apos] = arr[l + apos]; 
    for (bpos = 0; bpos < n2; bpos++) 
        RIGHT[bpos] = arr[m + 1 + bpos]; 
  
    //Merge the temp arrays back into arr[l..r]
    apos = 0; // Initial index of first subarray 
    bpos = 0; // Initial index of second subarray 
    cpos = l; // Initial index of merged subarray 
    
    while (apos < n1 && bpos < n2) { 
        if (LEFT[apos].count >= RIGHT[bpos].count) { //put greater counts into left array
            arr[cpos] = LEFT[apos]; 
            apos++; 
        } 
        else { 
            arr[cpos] = RIGHT[bpos]; 
            bpos++; 
        } 
        cpos++; 
    } 
  
    /* Copy the remaining elements of LEFT[], if there are any */
    while (apos < n1) { 
        arr[cpos] = LEFT[apos]; 
        apos++; 
        cpos++; 
    } 
  
    //copy remaining elements of RIGHT[] 
    while (bpos < n2) { 
        arr[cpos] = RIGHT[bpos]; 
        bpos++; 
        cpos++; 
    } 
} 

void mergesort(AVL nodes[], int left, int right) {
    if(left < right) {
        int centre = (left + right) / 2;
        mergesort(nodes, left, centre); //sort the left half
        mergesort(nodes, centre+1, right); // sort the right half
        merge(nodes, left, centre, right); // join the halves in sorted order
    }
}

void printArray(AVL A[], int size) { 
    cout << "First Ten Words: \n" << endl;
    for (int i = 0; i < 10; i++) 
        cout << "Word: " << A[i].word << " - Count: " << A[i].count << endl; 

    cout << "\nLast Ten Words: \n" << endl;
    for(int i = size - 10; i < size; i++) {
        cout << "Word: " << A[i].word << " - Count: " << A[i].count << endl; 
    }
} 

void in_order(AVL* node) {
    if(node == NULL) {
        return;
    }

    in_order(node->left); //traverse the left side of the tree
    nodes[node_count] = *node; //add the nodes to the array of structs
    node_count++; //increment the node count index
    in_order(node->right); //traverse the right side of the tree
}


