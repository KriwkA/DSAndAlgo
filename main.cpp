#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#include <segment_set.hpp>

int main()
{
   SegmentSetBase<int> arr;

   arr.Insert( 1 );
   arr.Insert( 2 );
   arr.Insert( 3 );
   arr.Insert( 4 );
   arr.Insert( 5 ); // [(1, 5)]

   arr.Erase( 2 );  // [(1,1), (3,5)]
   arr.ShiftErase( 3 ); // [(1,1), (3,4)]

   return 0;
}
