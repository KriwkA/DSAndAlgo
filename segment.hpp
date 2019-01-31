#pragma once
#include <utility>
#include <algorithm>

template<typename T>
struct SegmentBase : std::pair<T, T>
{
   enum ShiftDirection { mdLeft, mdRight };

   using std::pair<T, T>::first;
   using std::pair<T, T>::second;

   struct iterator
   {
      iterator() = default;
      iterator( iterator &&r ) = default;
      iterator( const iterator &l ) = default;
      iterator& operator=( iterator &&r ) = default;
      iterator& operator=( const iterator &l ) = default;

      inline const T& operator*() const { return m_val; }
      inline bool operator==( const iterator& r ) const { return m_val == r.m_val; }
      inline bool operator!=( const iterator& r ) const { return !operator==(r); }
      inline void operator++() { ++m_val; }
      inline void operator--() { --m_val; }
   private:
      iterator( const T& v ) : m_val(v) {}
      iterator( T&& v ) : m_val(std::move(v)) {}
      T m_val;
      friend struct SegmentBase<T>;
   };

   inline SegmentBase() : std::pair<T,T>( ZERO_LENGTH, ZERO_LENGTH - IncVal() )
   {
      static_assert( sizeof( SegmentBase<T> ) > 0, "SegmentBase<T> unknown size" );
   }

   SegmentBase( const SegmentBase& l ) = default;
   SegmentBase( SegmentBase&& r ) = default;
   SegmentBase& operator=( const SegmentBase& l ) = default;
   SegmentBase& operator=( SegmentBase&& r )  = default;

   SegmentBase( const T& l, const T& r );
   bool Merge( const SegmentBase& s );
   void ForceMerge( const SegmentBase& s );
   void Shift( const T& val, ShiftDirection md );
   std::pair<SegmentBase, SegmentBase> Split(T mid ) const;
   std::pair<SegmentBase, SegmentBase> Split( const SegmentBase& SegmentBase ) const;
   T Length() const;
   bool IsValid() const;
   bool Nearby( const SegmentBase& s ) const;
   bool Contains( const T& i ) const;
   bool Contains( const SegmentBase& s ) const;
   bool Intersect( const SegmentBase& s ) const;
   bool operator<( const SegmentBase& r ) const;
   bool operator ==( const SegmentBase& r ) const;
   bool operator !=( const SegmentBase& r ) const;

   inline iterator begin() const { return iterator(first); }
   inline iterator end() const { return iterator(second + IncVal()); }

   static constexpr T IncVal();
   static const T ZERO_LENGTH;
};

template<typename T>
constexpr T SegmentBase<T>::ZERO_LENGTH = SegmentBase<T>::IncVal() - SegmentBase<T>::IncVal();

template<typename T> inline SegmentBase<T>::SegmentBase( const T& l, const T& r)
   : std::pair<T,T>( l, r ) {}

template<typename T> inline void SegmentBase<T>::ForceMerge(const  SegmentBase<T> &s)
{ second = std::max( second, s.second ); first = std::min( first, s.first ); }

template<typename T> inline void SegmentBase<T>::Shift( const T& val, ShiftDirection md )
{
   if( md == mdRight )
   {
      second += val;
      first += val;
   }
   else
   {
      second -= val;
      first -= val;
   }
}

template<typename T> inline std::pair<SegmentBase<T>, SegmentBase<T>> SegmentBase<T>::Split(T mid) const
{ return std::make_pair( SegmentBase<T>( first, mid - IncVal() ), SegmentBase<T>( mid + IncVal(), second ) ); }

template<typename T> inline std::pair< SegmentBase<T>,  SegmentBase<T>> SegmentBase<T>::Split(const SegmentBase<T>& s ) const
{ return std::make_pair( SegmentBase<T>( first, s.first - IncVal() ), SegmentBase<T>( s.second + IncVal(), second ) ); }

template<typename T> inline T SegmentBase<T>::Length() const
{ return second - first + IncVal(); }

template<typename T> inline bool SegmentBase<T>::IsValid() const
{ return first <= second; }

template<typename T> inline bool SegmentBase<T>::Nearby(const SegmentBase<T> &s) const
{ return first - IncVal() == s.second || second + IncVal() == s.first; }

template<typename T> inline bool SegmentBase<T>::Contains( const T& i) const
{ return first <= i && i <= second; }

template<typename T> inline bool SegmentBase<T>::Contains(const SegmentBase& s) const
{ return first <= s.first && s.second <= second; }

template<typename T> inline bool SegmentBase<T>::Intersect(const SegmentBase<T> &s) const
{ return *this < s ? (second >= s.first) : (s.second >= first); }

template<typename T> inline bool SegmentBase<T>::operator<(const SegmentBase<T> &r) const
{ return second < r.second || (r.second == second && r.first < first); }

template<typename T> inline bool SegmentBase<T>::operator ==(const SegmentBase<T> &r) const
{ return first == r.first && second == r.second; }

template<typename T> inline bool SegmentBase<T>::operator !=(const SegmentBase<T> &r) const
{ return !(first == r.first && second == r.second); }

template<typename T>
inline constexpr T SegmentBase<T>::IncVal()
{
   static_assert( std::is_integral<T>::value || std::is_floating_point<T>::value, "T SegmentBase<T>::IncVal() not implemented" );
   return static_cast<T>(1);
}

template<typename T> inline bool SegmentBase<T>::Merge( const SegmentBase<T> &s )
{
   if( Intersect( s ) || Nearby( s ) )
   {
      ForceMerge( s );
      return true;
   }
   return false;
}
