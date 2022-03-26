///
/// Singly linked list of input files to be encoded in a strip
///

#ifndef FILELIST_H
#define FILELIST_H

#include <stddef.h>  // NULL
#include <stdlib.h>  // malloc(), free()
#include <time.h>    // time_t

#include "stdtypes.h" // bool, true, false
#include "txdirect.h"  // InputFileType

//  CZFListNode
//  +----------------+
//  | data           |
//  |          next--+->
//  +----------------+

#define DATA_TYPE                  struct InputFileType*
#define COUNTER_TYPE               uint16_t

typedef DATA_TYPE                  CZFListDataType;
typedef COUNTER_TYPE               CZFListCounterType;


// ADT allocation/deallocation/clear
void CZFList_initialize( CZFListCounterType initialReserveCount, CZFListCounterType initialReserveCapacity );
void CZFList_relinquish( void );
void CZFList_empty( void );


// ADT instrumentation
bool CZFList_isEmpty( void );
CZFListCounterType CZFList_itemCount( void );
CZFListCounterType CZFList_reserveCount( void );

// List<->Item Operations
// O(1) immediate examine/update operations: insertHead/Tail, removeHead/Tail, peekHead/Tail
bool CZFList_insertAtHead( CZFListDataType dataIn );
bool CZFList_insertAtTail( CZFListDataType dataIn );
bool CZFList_removeFromHead( CZFListDataType* dataOut );
bool CZFList_removeFromTail( CZFListDataType* dataOut );
bool CZFList_peekAtHead( CZFListDataType* dataOut );
bool CZFList_peekAtTail( CZFListDataType* dataOut );


// O(n) indexed operations: get, set, insertBefore, removeFrom
bool CZFList_get( CZFListCounterType rowIn, CZFListDataType* dataOut );

bool CZFList_set( CZFListCounterType rowIn, CZFListDataType dataIn, CZFListDataType* dataOut );

bool CZFList_insertBefore( CZFListCounterType rowIn, CZFListDataType dataIn );

bool CZFList_removeFrom( CZFListCounterType rowIn, CZFListDataType* dataOut );


// O(n) examine/update operations: findFirstInOrder/NextInOrder, removeFirstInOrder/NextInOrder, removeAll
bool CZFList_findFirstInOrder( CZFListDataType dataIn,
                               CZFListCounterType* rowOut,
                               bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ) );

bool CZFList_findNextInOrder( CZFListDataType dataIn,
                              CZFListCounterType* rowInOut,
                              bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ) );

bool CZFList_removeFirstInOrder( CZFListDataType dataIn,
                                 CZFListCounterType* rowOut,
                                 bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ),
                                 CZFListDataType* dataOut );

bool CZFList_removeNextInOrder( CZFListDataType dataIn,
                                CZFListCounterType* rowInOut,
                                bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ),
                                CZFListDataType* dataOut );

bool CZFList_removeAll( CZFListDataType dataIn,
                        bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ) );


// visit items: applyInOrder/ReverseOrder O(n), O(2n)
void CZFList_applyInOrder( void ( *function )( CZFListDataType* item ) );

void CZFList_applyReverseOrder( void ( *function )( CZFListDataType* item ) );

// selection sort items O(n^2)
void CZFList_sort( bool ( *CZFListDataType_lessThan )( CZFListDataType lhs, CZFListDataType rhs ) );


#endif // FILELIST_H

