#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
pthread_t t[4]; 
pthread_mutex_t lock;
typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;

typedef struct Share{
  FILE *infile;
  dict_t *d;
  int test;
}share_t;

share_t* share;

char *
make_word( char *word ) {
  return strcpy( malloc( strlen( word )+1 ), word );
}

dict_t *
make_dict(char *word) {
  dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );
  nd->word = make_word( word );
  nd->count = 1;
  nd->next = NULL;
  return nd;
}

dict_t *
insert_word( dict_t *d, char *word ) {
  
  //   Insert word into dict or increment count if already there
  //   return pointer to the updated dict
  
  dict_t *nd;
  dict_t *pd = NULL;		// prior to insertion point 
  dict_t *di = d;		// following insertion point
  // Search down list to find if present or point of insertion
  while(di && ( strcmp(word, di->word ) >= 0) ) { 
    if( strcmp( word, di->word ) == 0 ) { 
      di->count++;		// increment count 
      return d;			// return head 
    }
    pd = di;			// advance ptr pair
    di = di->next;
  }
  nd = make_dict(word);		// not found, make entry 
  nd->next = di;		// entry bigger than word or tail 
  if (pd) {
    pd->next = nd;
    return d;			// insert beond head 
  }
  return nd;
}

void print_dict(dict_t *d) {
  while (d) {
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;
  }
}

int
get_word( char *buf, int n, FILE *infile) {
  int inword = 0;
  int c;  
 
  while( (c = fgetc(infile)) != EOF ) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	// terminate the word string
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
  return 0;			// no more words
}

#define MAXWORD 1024

void
words( FILE *infile ) {

  char wordbuf[MAXWORD];
   
  while( get_word( wordbuf, MAXWORD, infile ) ) 
    share->d = insert_word(share->d, wordbuf); // add to dict

}

void *threadF(void* share){
 
  pthread_mutex_lock(&lock); 
  share_t* Share=(share_t*)share;
  char wordbuf[MAXWORD];
  words(Share->infile);
  pthread_mutex_unlock(&lock);

}



int
main( int argc, char *argv[] ) {
  
  pthread_t thread[4],thread1;
  dict_t *d = NULL;
  FILE *infile = stdin;
  
  if (argc >= 2)
    infile = fopen (argv[1],"r");

  if( !infile ) {
    printf("Unable to open %s\n",argv[1]);
    exit( EXIT_FAILURE );
  }
   
  share =malloc(sizeof(share_t));
  share->d= NULL;  
  share->infile=infile;
  int count;
  
  if(pthread_mutex_init(&lock,NULL)!=0){
    printf("Failed to initialize lock\nPress any key to exit:");
    getchar();
  }  
 
//  for(count=0;count<4;count++)
    pthread_create(&thread1,NULL,threadF, share);
  
  //for(count=0;count<4;count++)
    pthread_join(thread1, NULL);
    
  print_dict( share->d );
  fclose( infile );
  pthread_mutex_destroy(&lock);
  free(share);

}

