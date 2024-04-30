///
/// Singly linked list of input files to be encoded in a strip
///
#include "filelist.h"

struct CZFListNode
{
   struct CZFListNode*      next;

   CZFListDataType          data;
};


struct CZFListType
{
   struct CZFListNode*      head;
   struct CZFListNode*      tail;

   CZFListCounterType       itemCounter;

   struct CZFListNode*      reserve;
   CZFListCounterType       reserveCounter;
   CZFListCounterType       reserveCapacity;
};


struct CZFListType CZFList;


void CZFList_initialize( CZFListCounterType initialReserveCount, CZFListCounterType initialReserveCapacity )
{
   CZFListCounterType i;

   CZFList.head = NULL;
   CZFList.tail = NULL;

   CZFList.itemCounter = 0;

   CZFList.reserve = NULL;
   CZFList.reserveCounter = 0;
   CZFList.reserveCapacity = ( initialReserveCapacity > 0 ) ? initialReserveCapacity : 0;

   for ( i = 0; i <= initialReserveCount; i++ )
   {
      struct CZFListNode* index =
         ( struct CZFListNode* )calloc( 1, sizeof( struct CZFListNode ) );

      if ( index )
      {
         index->next = CZFList.reserve;

         CZFList.reserve = index;

         ++CZFList.reserveCounter;
      }
      else
      {
         break;
      }
   }

   if ( 0 != CZFList.reserveCounter )
   {
      CZFList.head = CZFList.tail = CZFList.reserve;

      CZFList.reserve = CZFList.reserve->next;

      CZFList.head->next = NULL;

      --CZFList.reserveCounter;
   }
}


void CZFList_relinquish()
{
   while ( CZFList.reserve )
   {
      CZFList.tail = CZFList.reserve->next;

      free( CZFList.reserve );

      CZFList.reserve = CZFList.tail;
   }

   while ( CZFList.head )
   {
      CZFList.tail = CZFList.head->next;

      free( CZFList.head );

      CZFList.head = CZFList.tail;
   }
}


void CZFList_empty()
{
   if ( 0 != CZFList.itemCounter )
   {
      while ( CZFList.head->next )
      {
         CZFList.tail = CZFList.head->next;

         if ( CZFList.reserveCapacity == CZFList.reserveCounter )
         {
            free( CZFList.head );
         }
         else
         {
            CZFList.head->next = CZFList.reserve;

            CZFList.reserve = CZFList.head;

            ++CZFList.reserveCounter;
         }

         CZFList.head = CZFList.tail;
      }

      CZFList.itemCounter = 0;
   }
}


bool CZFList_isEmpty()
{
   return ( 0 != CZFList.itemCounter ) ? false : true;
}


CZFListCounterType CZFList_itemCount()
{
   return CZFList.itemCounter;
}


CZFListCounterType CZFList_reserveCount()
{
   return CZFList.reserveCounter;
}


bool CZFList_insertAtHead( CZFListDataType dataIn )
{
   if ( 0 != CZFList.itemCounter )
   {
      CZFListCounterType nextCounter = CZFList.itemCounter + 1;

      if ( 0 < nextCounter )
      {
         struct CZFListNode* head;

         if ( 0 != CZFList.reserveCounter )
         {
            head = CZFList.reserve;

            CZFList.reserve = CZFList.reserve->next;

            --CZFList.reserveCounter;
         }
         else
         {
            head = ( struct CZFListNode* )malloc( sizeof( struct CZFListNode ) );

            if ( NULL == head ) return false;
         }

         head->data = dataIn;

         head->next = CZFList.head;

         CZFList.head = head;

         CZFList.itemCounter = nextCounter;

         return true;
      }
   }
   else if ( NULL != CZFList.head )
   {
      CZFList.head->data = dataIn;

      CZFList.itemCounter = 1;

      return true;
   }

   return false;
}


bool CZFList_insertAtTail( CZFListDataType dataIn )
{
   if ( 0 != CZFList.itemCounter )
   {
      CZFListCounterType nextCounter = CZFList.itemCounter + 1;

      if ( 0 < nextCounter )
      {
         struct CZFListNode* tail;

         if ( 0 != CZFList.reserveCounter )
         {
            tail = CZFList.reserve;

            CZFList.reserve = CZFList.reserve->next;

            --CZFList.reserveCounter;
         }
         else
         {
            tail = ( struct CZFListNode* )malloc( sizeof( struct CZFListNode ) );

            if ( NULL == tail ) return false;
         }

         tail->data = dataIn;

         tail->next = NULL;

         CZFList.tail->next = tail;

         CZFList.tail = tail;

         CZFList.itemCounter = nextCounter;

         return true;
      }
   }
   else if ( NULL != CZFList.head )
   {
      CZFList.head->data = dataIn;

      CZFList.itemCounter = 1;

      return true;
   }

   return false;
}


bool CZFList_removeFromHead( CZFListDataType* dataOut )
{
   if ( 0 != CZFList.itemCounter )
   {
      --CZFList.itemCounter;

      *dataOut = CZFList.head->data;

      if ( 0 != CZFList.itemCounter )
      {
         struct CZFListNode* head = CZFList.head;

         CZFList.head = CZFList.head->next;

         if ( CZFList.reserveCounter != CZFList.reserveCapacity )
         {
            head->next = CZFList.reserve;

            CZFList.reserve = head;

            ++CZFList.reserveCounter;

            return true;
         }
         else
         {
            free( head );

            return true;
         }
      }

      return true;
   }

   return false;
}


bool CZFList_removeFromTail( CZFListDataType* dataOut )
{
   if ( 0 != CZFList.itemCounter )
   {
      --CZFList.itemCounter;

      *dataOut = CZFList.tail->data;

      if ( 0 != CZFList.itemCounter )
      {
         struct CZFListNode* tail = CZFList.tail;

         CZFList.tail = CZFList.head;

         while ( CZFList.tail->next != tail )
         {
            CZFList.tail = CZFList.tail->next;
         }

         CZFList.tail->next = NULL;

         if ( CZFList.reserveCounter != CZFList.reserveCapacity )
         {
            tail->next = CZFList.reserve;

            CZFList.reserve = tail;

            ++CZFList.reserveCounter;

            return true;
         }
         else
         {
            free( tail );

            return true;
         }
      }

      return true;
   }

   return false;
}


bool CZFList_peekAtHead( CZFListDataType* dataOut )
{
   if ( 0 != CZFList.itemCounter )
   {
      *dataOut = CZFList.head->data;

      return true;
   }

   return false;
}


bool CZFList_peekAtTail( CZFListDataType* dataOut )
{
   if ( 0 != CZFList.itemCounter )
   {
      *dataOut = CZFList.tail->data;

      return true;
   }

   return false;
}


bool CZFList_get( CZFListCounterType rowIn, CZFListDataType* dataOut )
{
   if ( ( rowIn >= 0 ) && ( rowIn < CZFList.itemCounter ) )
   {
      struct CZFListNode* index = CZFList.head;

      while ( rowIn > 0 )
      {
         --rowIn;

         index = index->next;
      }

      *dataOut = index->data;

      return true;
   }

   return false;
}


bool CZFList_set( CZFListCounterType rowIn, CZFListDataType dataIn, CZFListDataType* dataOut )
{
   if ( ( rowIn >= 0 ) && ( rowIn < CZFList.itemCounter ) )
   {
      struct CZFListNode* index = CZFList.head;

      while ( rowIn > 0 )
      {
         --rowIn;

         index = index->next;
      }

      if ( NULL != dataOut )
      {
         *dataOut = index->data;
      }

      index->data = dataIn;

      return true;
   }
   else if ( rowIn == CZFList.itemCounter )
   {
      if ( CZFList_insertAtTail( dataIn ) )
      {
         if ( NULL != dataOut )
         {
            *dataOut = dataIn;
         }

         return true;
      }
   }

   return false;
}


bool CZFList_insertBefore( CZFListCounterType rowIn, CZFListDataType dataIn )
{
   if ( ( rowIn >= 0 ) && ( rowIn < CZFList.itemCounter ) )
   {
      CZFListCounterType nextCounter = CZFList.itemCounter + 1;

      if ( 0 < nextCounter )
      {
         struct CZFListNode* index = CZFList.head;

         struct CZFListNode* previousNode = NULL;

         struct CZFListNode* splitNode;

         while ( rowIn > 0 )
         {
            --rowIn;

            previousNode = index;
            index = index->next;
         }

         if ( 0 != CZFList.reserveCounter )
         {
            splitNode = CZFList.reserve;

            CZFList.reserve = splitNode->next;

            --CZFList.reserveCounter;
         }
         else
         {
            splitNode = ( struct CZFListNode* )malloc( sizeof( struct CZFListNode ) );

            if ( NULL == splitNode ) return false;
         }

         if ( NULL != previousNode )
         {
            previousNode->next = splitNode;
         }
         else
         {
            CZFList.head = splitNode;
         }

         splitNode->next = index;

         splitNode->data = dataIn;

         CZFList.itemCounter = nextCounter;

         return true;
      }
   }
   else if ( rowIn == CZFList.itemCounter )
   {
      return CZFList_insertAtTail( dataIn );
   }

   return false;
}


bool CZFList_removeFrom( CZFListCounterType rowIn, CZFListDataType* dataOut )
{
   if ( 0 != CZFList.itemCounter )
   {
      if ( ( rowIn >= 0 ) && ( rowIn < CZFList.itemCounter ) )
      {
         struct CZFListNode* index = CZFList.head;

         struct CZFListNode* previousNode = NULL;

         --CZFList.itemCounter;

         while ( rowIn > 0 )
         {
            --rowIn;

            previousNode = index;
            index = index->next;
         }

         if ( NULL != dataOut )
         {
            *dataOut = index->data;
         }

         if ( 0 != CZFList.itemCounter )
         {
            if ( NULL != previousNode )
            {
               if ( NULL != index->next )
               {
                  previousNode->next = index->next;
               }
               else
               {
                  previousNode->next = NULL;

                  CZFList.tail = previousNode;
               }
            }
            else
            {
               CZFList.head = index->next;
            }

            if ( CZFList.reserveCounter != CZFList.reserveCapacity )
            {
               index->next = CZFList.reserve;

               CZFList.reserve = index;

               ++CZFList.reserveCounter;

               return true;
            }
            else
            {
               free( index );

               return true;
            }
         }

         return true;
      }
   }

   return false;
}


bool CZFList_findFirstInOrder( CZFListDataType dataIn,
                               CZFListCounterType* rowOut,
                               bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ) )
{
   if ( 0 != CZFList.itemCounter )
   {
      struct CZFListNode* index = CZFList.head;

      CZFListCounterType offset = 0;

      do
      {
         if ( CZFListDataType_equalTo( dataIn, index->data ) )
         {
            *rowOut = offset;

            return true;
         }

         ++offset;
      }
      while ( NULL != ( index = index->next ) );
   }

   return false;
}


bool CZFList_findNextInOrder( CZFListDataType dataIn,
                              CZFListCounterType* rowInOut,
                              bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ) )
{
   if ( ( *rowInOut >= 0 ) && ( *rowInOut + 1 < CZFList.itemCounter ) )
   {
      struct CZFListNode* index = CZFList.head;

      CZFListCounterType offset = *rowInOut;

      while ( offset > 0 )
      {
         --offset;

         index = index->next;
      }

      offset = *rowInOut;

      while ( NULL != ( index = index->next ) )
      {
         ++offset;

         if ( CZFListDataType_equalTo( dataIn, index->data ) )
         {
            *rowInOut = offset;

            return true;
         }
      }

      *rowInOut = offset;
   }

   return false;
}


bool CZFList_removeFirstInOrder( CZFListDataType dataIn,
                                 CZFListCounterType* rowOut,
                                 bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ),
                                 CZFListDataType* dataOut )
{
   if ( 0 != CZFList.itemCounter )
   {
      struct CZFListNode* index = CZFList.head;

      struct CZFListNode* previousNode = NULL;

      CZFListCounterType offset = 0;

      do
      {
         if ( CZFListDataType_equalTo( dataIn, index->data ) )
         {
            *rowOut = offset;

            if ( NULL != dataOut )
            {
               *dataOut = index->data;
            }

            --CZFList.itemCounter;

            if ( 0 != CZFList.itemCounter )
            {
               if ( NULL != previousNode )
               {
                  if ( NULL != index->next )
                  {
                     previousNode->next = index->next;
                  }
                  else
                  {
                     previousNode->next = NULL;

                     CZFList.tail = previousNode;
                  }
               }
               else
               {
                  CZFList.head = index->next;
               }

               if ( CZFList.reserveCounter != CZFList.reserveCapacity )
               {
                  index->next = CZFList.reserve;

                  CZFList.reserve = index;

                  ++CZFList.reserveCounter;

                  return true;
               }
               else
               {
                  free( index );

                  return true;
               }
            }

            // ( 0 == CZFList.itemCounter )
            return true;
         }

         ++offset;

         previousNode = index;
      }
      while ( NULL != ( index = index->next ) );
   }

   return false;
}


bool CZFList_removeNextInOrder( CZFListDataType dataIn,
                                CZFListCounterType* rowInOut,
                                bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ),
                                CZFListDataType* dataOut )
{
   if ( ( *rowInOut >= 0 ) && ( *rowInOut < CZFList.itemCounter ) )
   {
      struct CZFListNode* index = CZFList.head;

      struct CZFListNode* previousNode = NULL;

      CZFListCounterType offset = *rowInOut;

      while ( offset > 0 )
      {
         --offset;

         previousNode = index;
         index = index->next;
      }

      offset = *rowInOut;

      do
      {
         if ( CZFListDataType_equalTo( dataIn, index->data ) )
         {
            *rowInOut = offset;

            if ( NULL != dataOut )
            {
               *dataOut = index->data;
            }

            --CZFList.itemCounter;

            if ( 0 != CZFList.itemCounter )
            {
               if ( NULL != previousNode )
               {
                  if ( NULL != index->next )
                  {
                     previousNode->next = index->next;
                  }
                  else
                  {
                     previousNode->next = NULL;

                     CZFList.tail = previousNode;
                  }
               }
               else
               {
                  CZFList.head = index->next;
               }

               if ( CZFList.reserveCounter != CZFList.reserveCapacity )
               {
                  index->next = CZFList.reserve;

                  CZFList.reserve = index;

                  ++CZFList.reserveCounter;

                  return true;
               }
               else
               {
                  free( index );

                  return true;
               }
            }

            return true;
         }

         ++offset;

         previousNode = index;
      }
      while ( NULL != ( index = index->next ) );

      *rowInOut = offset - 1;
   }

   return false;
}


bool CZFList_removeAll( CZFListDataType dataIn,
                        bool ( *CZFListDataType_equalTo )( CZFListDataType lhs, CZFListDataType rhs ) )
{
   bool result = false;

   if ( 0 != CZFList.itemCounter )
   {
      struct CZFListNode* index = CZFList.head;

      struct CZFListNode* previousNode = NULL;

      struct CZFListNode* nextNode;

      do
      {
         nextNode = index->next;

         if ( CZFListDataType_equalTo( dataIn, index->data ) )
         {
            if ( false == result )
            {
               result = true;
            }

            --CZFList.itemCounter;

            if ( 0 != CZFList.itemCounter )
            {
               if ( NULL != previousNode )
               {
                  if ( NULL != index->next )
                  {
                     previousNode->next = index->next;
                  }
                  else
                  {
                     previousNode->next = NULL;

                     CZFList.tail = previousNode;
                  }
               }
               else
               {
                  CZFList.head = index->next;
               }

               if ( CZFList.reserveCounter != CZFList.reserveCapacity )
               {
                  index->next = CZFList.reserve;

                  CZFList.reserve = index;

                  ++CZFList.reserveCounter;
               }
               else
               {
                  free( index );
               }
            }
            else
            {
               previousNode = index;
            }
         }
      }
      while ( NULL != ( index = nextNode ) );
   }

   return result;
}


void CZFList_applyInOrder( void ( *function )( CZFListDataType* item ) )
{
   if ( 0 != CZFList.itemCounter )
   {
      struct CZFListNode* index = CZFList.head;

      do
      {
         ( *function )( &( index->data ) );
      }
      while ( NULL != ( index = index->next ) );
   }
}


void CZFList_applyReverseOrder( void ( *function )( CZFListDataType* item ) )
{
   if ( 0 != CZFList.itemCounter )
   {
      struct CZFListNode* index = CZFList.head;

      struct CZFListNode* reverseNode = NULL;

      struct CZFListNode* nextNode;

      do
      {
         nextNode = index->next;

         index->next = reverseNode;

         reverseNode = index;
      }
      while ( NULL != ( index = nextNode ) );

      index = reverseNode;

      reverseNode = NULL;

      do
      {
         ( *function )( &( index->data ) );

         nextNode = index->next;

         index->next = reverseNode;

         reverseNode = index;
      }
      while ( NULL != ( index = nextNode ) );
   }
}


void CZFList_sort( bool ( *CZFListDataType_lessThan )( CZFListDataType lhs, CZFListDataType rhs ) )
{
   if ( 1 < CZFList.itemCounter )
   {
      struct CZFListNode* index = CZFList.head;

      struct CZFListNode* min_index;

      struct CZFListNode* sel_index;

      CZFListDataType swap;

      // find first out-of-order item
      do
      {
         sel_index = index;

         index = index->next;

         if ( NULL == index )
         {
            return;
         }
      }
      while ( !CZFListDataType_lessThan( index->data, sel_index->data ) );

      // selection sort
      sel_index = CZFList.head;

      do
      {
         min_index = sel_index;

         index = sel_index->next;

         // find the min to the right of select
         do
         {
            if ( CZFListDataType_lessThan( index->data, min_index->data ) )
            {
               min_index = index;
            }
         }
         while ( NULL != ( index = index->next ) );

         if ( min_index != sel_index )
         {
            swap = sel_index->data;

            sel_index->data = min_index->data;

            min_index->data = swap;
         }

         sel_index = sel_index->next;
      }
      while ( sel_index != CZFList.tail );
   }
}
