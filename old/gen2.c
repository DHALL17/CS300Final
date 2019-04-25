#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "queue.h"

double clock_time , btime;
int processCount , memorySize , diskSize , blockedCount , pname , tlr, b , a;
int pCount [ 4 ];

typedef struct Process {
	FILE *fp;
	char fname[ 7 ];
	int point,
	priority, 
	memory,
	blocked;
	double runtime,
	exec_time,
	block_time,
	start_time,
	end_time,
	max_int;
} Process;

void dam ( char *s ) {
	printf ( "%s\n" , s );
	exit ( 1 );
}

Process *newProcess ( FILE *fp , char name[ ] ) {
	Process *p = malloc ( sizeof( Process ) );
	for ( int x = 0 ; x < 7 ; ++x )
		p->fname[ x ] = name[ x ];
	fscanf ( fp , "%i" , &p->point );
	fscanf ( fp , "%i" , &p->priority );
	fscanf ( fp , "%i" , &p->memory );
	fscanf ( fp , "%lf" , &p->runtime );
	pCount[ p->priority ]++;
	p->exec_time = 0;
	p->blocked = 0;
	p->block_time = 0;
	p->start_time = clock_time;
	p->max_int = 0;
	return p;
}

void freeProcess ( void *item ) {
	Process *p = ( Process * ) item;
	processCount--;
	memorySize--;
	p->end_time = clock_time;
	pCount[ p->priority ]--;
	memorySize--;
	p->end_time = clock_time;
	remove ( p->fname );
	fclose ( p->fp );
	if ( p->start_time <= 0.020)
		p->start_time = 0;
	p->end_time = clock_time;
	free ( p );
}

void process_memory ( QUEUE **Memory , QUEUE **Disk , double *memory , Process *p ){
	if ( *memory - ( p->memory * 0.1 ) >= 0 ) {
		*memory = *memory - ( p->memory * 0.1 );
		enqueue ( Memory[ p->priority ] , p );
		memorySize++;
	}
	else{
		enqueue ( Disk[ p->priority ] , p );
		diskSize++;
	}

}

void proc_gen ( QUEUE **Memory , QUEUE **Disk , double *memory ) {
	int blocks;
	char fname [ 7 ];

	sprintf ( fname , "p%i", pname );
	FILE *fp = fopen ( fname , "w" );
	if ( !fp ) {	
		printf( "Pname : %i" , pname );
		dam ( "File did not open - proc_gen()" );
	}

	fprintf ( fp , "%i\n" , 0 );					  		// execution point
	fprintf ( fp , "%i\n" , rand ( ) % 4 );  				// priority
	fprintf ( fp , "%i\n" , ( rand ( ) % 191 ) + 10 ); 		// memory
	fprintf ( fp , "%i\n" , ( rand ( ) % 39601 ) + 400 );	// run time

	blocks = ( rand ( ) % 99001 ) + 1000;			  		// instruction blocks
	for ( int i = 0 ; i < blocks ; ++i )
		fprintf ( fp , "%2i\n" , ( rand( ) % 20 ) + 1 );

	fclose ( fp );
	fp = fopen ( fname , "r" );
	if( !fp ) dam ( "File did not open - proc_gen()" );
    fclose(fp);
	Process *p = newProcess ( fp , fname );
	process_memory( Memory , Disk , memory , p );
	pname++;
	processCount++;
}

int blocked ( void ) {
	int x = ( rand ( ) % 100 ) + 1;
	if ( x <= 2)
		return 1;
	
	return 0;
}

void delay ( Process *p ) {
	int x = rand ( ) % 100 + 1;
	if ( x <= 10 ) {
		int e = ( rand ( ) % 307 ) + 10;
		p->block_time = clock_time + ( e * e );
	}
	
	else {
		int e = ( rand ( ) % 9 ) + 2;
		p->block_time = clock_time + ( e * e );
	}
}

double quantum_creation ( Process *p ) {
	if ( p->priority == 0 )
		return clock_time + 10 ;
	else if ( p->priority == 1 )
		return clock_time + 5;
	else if ( p->priority == 2 )
		return clock_time + 2.5;
	else
		return clock_time + 1.25;
}

void proc_exec ( QUEUE **M , QUEUE **D , QUEUE *q , Process *p , double *memory , double *new_process ) {
	double block, quantum;	

	if ( p->blocked ) {
		if ( btime > p->block_time || !btime )
				btime = p->block_time;

		if ( clock_time >= p->block_time ) {
			p->blocked = 0;
			blockedCount--;
		}
		enqueue ( q , p );
		return;
	}

	if ( p->max_int == 0.0)
		p->max_int = clock_time - p->start_time;
	
	else {
		int temp = clock_time - p->max_int;
		if( temp > clock_time - p->max_int )
			p->max_int = temp;
	}
    p->fp = fopen(p->fname, 'r');
	fscanf ( p->fp , "%lf" , &block );
	if ( feof ( p->fp ) ) {
		*memory = *memory + ( p->memory * 0.1 );
		p->end_time = clock_time;
		processCount--;
		memorySize--;
		p->end_time = clock_time;
		enqueue( M[ 4 ] , p );
		return;
	}

	if ( p->exec_time + block > p->runtime ) {
		*memory = *memory + ( p->memory * 0.1 );
		tlr++;
		processCount--;
		memorySize--;
		p->end_time = clock_time;
		enqueue( M[ 4 ] , p );
		return;
	}

	block = block / 1000;
	quantum = quantum_creation( p );
	while ( p->exec_time + block <= quantum ) {
		if ( p->start_time == 0.000 )
			p->start_time = clock_time;

		if ( feof ( p->fp ) ) {
			*memory = *memory + ( p->memory * 0.1 );
			processCount--;
			memorySize--;
			p->end_time = clock_time;
			enqueue( M[ 4 ] , p );
			return;
		}

		if ( p->exec_time + block > p->runtime ) {
			*memory = *memory + ( p->memory * 0.1 );
			tlr++;
			processCount--;
			memorySize--;
			p->end_time = clock_time;
			enqueue( M[ 4 ] , p );
			return;
		}
		
		if ( blocked ( ) ) {
			p->blocked = 1;
			delay ( p );
			if ( btime > p->block_time || !btime )
				btime = p->block_time;
			blockedCount++;
			break;
		}

		if ( *new_process <= clock_time  && pname < 1000 )  {
			proc_gen ( M , D , memory );
			*new_process = clock_time + ( ( rand ( ) % 9981 ) + 20 );
		}
		
		p->exec_time += block;
		clock_time += block;
		fscanf ( p->fp , "%lf" , &block );
		block = block / 1000;

	}
	enqueue ( q , p );
}

void disk_handler ( QUEUE **Memory , QUEUE **Disk , double *memory , Process *p , double *new_process ) {
	if ( *memory - ( p->memory * 0.1 ) >= 0 ) {
		*memory = *memory - ( p->memory * 0.1 );
		if ( p->blocked )
			blockedCount++;
		proc_exec ( Memory, Disk , Memory[ p->priority ], p , memory , new_process );
		memorySize++;
		diskSize--;
		return;
	}
	else {
		for ( int x = 3 ; x >= 0 ; --x ) {
			while( sizeQUEUE ( Memory[ x ] ) ) {
				Process *temp = dequeue ( Memory[ x ] );
				*memory = *memory + ( temp->memory * 0.1 );
				if ( temp->blocked )
					blockedCount--;
				enqueue ( Disk[ temp->priority ] , temp);
				memorySize--;
				diskSize++;
				p->end_time = clock_time;

				if ( *memory - ( p->memory * 0.1 ) >= 0 ) {
					clock_time += 100;
					*memory = *memory - ( p->memory * 0.1 );
					if ( p->blocked )
						blockedCount++;
					proc_exec ( Memory , Disk , Memory[ p->priority ], p , memory , new_process );
					memorySize++;
					diskSize--;
					return;
				}
			}
		}
		dam( "Error: Memory space was not allocated correctly" );
	}
}

int	main ( void ) {
	tlr = 0 , clock_time = 0 , memorySize = 0 , diskSize = 0 , processCount = 0 , blockedCount = 0 , pname = 0 , btime = 0;
	pCount[ 0 ] = 0;
	pCount[ 1 ] = 0;
	pCount[ 2 ] = 0;
	pCount[ 3 ] = 0;
	srand(time(NULL));
	double memory = 100;	// 100 Mb of system memory
	QUEUE **Memory = malloc ( sizeof( QUEUE* ) * 5 );
	QUEUE *P0I = newQUEUE ( freeProcess );
	QUEUE *P1I = newQUEUE ( freeProcess );
	QUEUE *P2I = newQUEUE ( freeProcess );
	QUEUE *P3I = newQUEUE ( freeProcess );
	QUEUE *end = newQUEUE ( freeProcess );
	Memory[ 0 ] = P0I;
	Memory[ 1 ] = P1I;
	Memory[ 2 ] = P2I;
	Memory[ 3 ] = P3I;
	Memory[ 4 ] = end;
	

	QUEUE **Disk = malloc ( sizeof( QUEUE* ) * 4 );
	QUEUE *P0N = newQUEUE ( freeProcess );
	QUEUE *P1N = newQUEUE ( freeProcess );
	QUEUE *P2N = newQUEUE ( freeProcess );
	QUEUE *P3N = newQUEUE ( freeProcess );
	Disk[ 0 ] = P0N;
	Disk[ 1 ] = P1N;
	Disk[ 2 ] = P2N;
	Disk[ 3 ] = P3N;

	for ( int x = 0 ; x < 50 ; ++x )
		proc_gen ( Memory , Disk , &memory );
	

	double new_process = clock_time + ( ( rand ( ) % 9981 ) + 20 );
	do {
		if ( processCount == blockedCount ) {
			clock_time = btime;
			btime = 0;
		}

		for ( int x = 0 ; x < 4 ; ++x ) {
			int tempSize = sizeQUEUE ( Memory[ x ] );
			for ( int y = 0 ; y < tempSize && sizeQUEUE ( Memory[ x ] ) ; ++y ) {
				proc_exec ( Memory , Disk , Memory[ x ] , dequeue ( Memory[ x ] ) , &memory  , &new_process );
			}
		}

		if ( processCount - memorySize ) {
			for ( int x = 0 ; x < 4 ; ++x ) {
				int tempSize = sizeQUEUE ( Disk[ x ] );
				for ( int y = 0 ; y < tempSize && sizeQUEUE ( Disk[ x ] ) ; ++y ) {
					disk_handler ( Memory , Disk , &memory , dequeue ( Disk[ x ] ) , &new_process );
				}
			}
		}
	} while ( processCount );
	
	//printf ( "Memory: %.2f mB\nClock Time: %.6lf ms\nProcess Count: %i\n" , memory , clock_time, pname );

	for ( int x = 0 ; x < 4 ; ++x ) {
		freeQUEUE ( Memory[x] );
		freeQUEUE ( Disk[x] );
	}

	double count0 = 0 , count1 = 0 , count2 = 0 , count3 = 0,
			int0 = 0 , int1 = 0 , int2 = 0 , int3 = 0;

	for ( int x = pname ; x > 0 ; --x ) {
		Process *p = dequeue ( Memory[ 4 ] );
		if ( p->priority == 0 ) {
			count0 += p->end_time - p->start_time;
			int0 += p->max_int;
		}
		else if ( p->priority == 1 ) {
			count1 += p->end_time - p->start_time;
			int1 += p->max_int;
		}
		else if ( p->priority == 2 ) {
			count2 += p->end_time - p->start_time;
			int2 += p->max_int;
		}
		else {
			count3 += p->end_time - p->start_time;
			int3 += p->max_int;
		}

		enqueue( Memory[ 4 ] , p );
	}

	double avg0 = count0 / pCount[0],
	avg1 = count1 / pCount[1],
	avg2 = count2 / pCount[2],
	avg3 = count3 / pCount[3],
	avg4 = int0 / pCount[0],
	avg5 = int1 / pCount[1],
	avg6 = int2 / pCount[2],
	avg7 = int3 / pCount[3],
	StdDev0 = 0,
	StdDev1 = 0,
	StdDev2 = 0,
	StdDev3 = 0,
	StdDev4 = 0,
	StdDev5 = 0,
	StdDev6 = 0,
	StdDev7 = 0;

	for ( int x = pname ; x > 0 ; --x ) {
		Process *p = dequeue ( Memory[ 4 ] );
		if ( p->priority == 0 ) {
			StdDev0 += pow( ( ( p->end_time - p->start_time ) - avg0 ) , 2 );
			StdDev4 += pow( ( p->max_int  - avg4 ) , 2 );
		}
		else if ( p->priority == 1 ) {
			StdDev1 += pow( ( ( p->end_time - p->start_time ) - avg1 ) , 2 );
			StdDev5 += pow( ( p->max_int  - avg5 ) , 2 );
		}
		else if ( p->priority == 2 ) {
			StdDev2 += pow( ( ( p->end_time - p->start_time ) - avg2 ) , 2 );
			StdDev6 += pow( ( p->max_int  - avg6 ) , 2 );
		}
		else {
			StdDev3 += pow( ( ( p->end_time - p->start_time ) - avg3 ) , 2 );
			StdDev7 += pow( ( p->max_int  - avg7 ) , 2 );
		}

		enqueue( Memory[ 4 ] , p );
	}

	if( pCount[ 0 ] > 1) {
		StdDev0 = sqrt( ( StdDev0 / ( pCount[ 0 ] - 1 ) ) );
		StdDev4 = sqrt( ( StdDev4 / ( pCount[ 0 ] - 1 ) ) );
	}
	if( pCount[ 1 ] > 1){
		StdDev1 = sqrt( ( StdDev1 / ( pCount[ 1 ] - 1 ) ) );
		StdDev5 = sqrt( ( StdDev5 / ( pCount[ 1 ] - 1 ) ) );
	}
	if( pCount[ 2 ] > 1) {
		StdDev2 = sqrt( ( StdDev2 / ( pCount[ 2 ] - 1 ) ) );
		StdDev6 = sqrt( ( StdDev6 / ( pCount[ 2 ] - 1 ) ) );
	}
	if( pCount[ 3 ] > 1) {
		StdDev3 = sqrt( ( StdDev3 / ( pCount[ 3 ] - 1 ) ) );
		StdDev7 = sqrt( ( StdDev7 / ( pCount[ 3 ] - 1 ) ) );
	}


	printf ( "P0:\n  Clock-Time\n    avg:    %.6lf s\n    StdDev: %.6lf s\n" , avg0 / 1000 , StdDev0 / 1000 );
	printf ( "  Max Interval\n    avg:    %.6lf s\n    StdDev: %.6lf s\n" , avg4 / 1000 , StdDev4 / 1000 );
	printf ( "P1:\n  Clock-Time\n    avg:    %.6lf s\n    StdDev: %.6lf s\n" , avg1 / 1000 , StdDev1 / 1000 );
	printf ( "  Max Interval\n    avg:    %.6lf s\n    StdDev: %.6lf s\n" , avg5 / 1000 , StdDev5 / 1000 );
	printf ( "P2:\n  Clock-Time\n    avg:    %.6lf s\n    StdDev: %.6lf s\n" , avg2 / 1000 , StdDev2 / 1000 );
	printf ( "  Max Interval\n    avg:    %.6lf s\n    StdDev: %.6lf s\n" , avg6 / 1000 , StdDev6 / 1000 );
	printf ( "P3:\n  Clock-Time\n    avg:    %.6lf s\n    StdDev: %.6lf s\n" , avg3 / 1000 , StdDev3 / 1000 );
	printf ( "  Max Interval\n    avg:    %.6lf s\n    StdDev: %.6lf s\n" , avg7 / 1000 , StdDev7 / 1000 );
	printf( "\nReached End of File: %i\n" , tlr );

	freeQUEUE ( Memory[ 4 ] );
	free ( Memory );
	free ( Disk );

	return 0;
}
