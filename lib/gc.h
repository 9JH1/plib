// basic garbage collector util  
#ifndef GARBAGE_COLLECTOR
#define GARBAGE_COLLECTOR 

// requirements
#include <stdlib.h>

#ifdef GARBAGE_COLLECTOR_VERBOSE
#include <stdio.h> // for print
#endif // GARBAGE_COLLECTOR_VERBOSE

typedef struct {
	char **str;
	int idx;
	int cap;
} gc;

static gc GC = {0};

static inline int gc_val(){
	if(GC.cap == 0){
		GC.cap = 2;
		GC.str = (char **)malloc(GC.cap * sizeof(char *));
		if(!GC.str){ // malloc error
			#ifdef GARBAGE_COLLECTOR_VERBOSE 
			printf("error initialized garbage collector\n");
			#endif
			GC.cap = 0;
			return 1;
		}
		#ifdef GARBAGE_COLLECTOR_VERBOSE 
		printf("successfully initialized garbage collector\n");
		#endif
	}

	if(GC.cap <= GC. idx){
		GC.cap *= 2;
		char **tmp = (char **)realloc(GC.str,GC.cap * sizeof(char *));
		if(!tmp){ // realloc error
			#ifdef GARBAGE_COLLECTOR_VERBOSE 
			printf("error reallocating GC");
			#endif
			return 1;
		} else GC.str = tmp;
		#ifdef GARBAGE_COLLECTOR_VERBOSE 
		printf("garbage collector was reallocated from a capacity of %d to %d\n",GC.cap/2,GC.cap);
		#endif
	}

	return 0;
}

static inline void *MALLOC(const size_t a){
	if(a <= 0){
		#ifdef GARBAGE_COLLECTOR_VERBOSE
		printf("input value %zu is too small, must be more then 0\n",a);
		#endif
		return NULL;
	}

	if(gc_val() != 0) return NULL; 
	GC.str[GC.idx] = (char *)malloc(a);
	if(GC.str[GC.idx]){
		void *out = GC.str[GC.idx++];
		#ifdef GARBAGE_COLLECTOR_VERBOSE
		printf("allocated %zu bytes to %p\n",a,out);
		#endif
		return out; 
	} else return NULL; // malloc error
}

// free garbage
static inline void FREE_ALL(){
	for(int i = 0; i < GC.idx; i++){
		#ifdef GARBAGE_COLLECTOR_VERBOSE
		printf( "free'd %zu from %p\n",sizeof GC.str[ i ], (void *)GC.str[ i ] );
		#endif

		free( GC.str[ i ] );
		GC.str[ i ] = NULL; // dangling pointer!!!
	}
	#ifdef GARBAGE_COLLECTOR_VERBOSE 
	printf("free'd all memory\n");
	#endif
	free(GC.str);
	GC.str = NULL;
	GC.idx = 0;
	GC.cap = 0;
}

#endif // GARBAGE_COLLECTOR 
