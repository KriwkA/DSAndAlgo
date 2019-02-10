#pragma once
#include <set>
#include <map>
#include <algorithm>
#include <list>
#include "segment.hpp"

/*
 * ВНИМАНИЕ const_cast в SegmentTree::ShiftInsert и SegmentTree::ShiftErase !!!
 * В set iterator(НЕ const) указывает на const KeyT&, дабы кривые руки не испортили порядок сортировки
 * Т.к. мы последовательно сдвигаем элементы, не нарушая сортивроку, в целях эффективности используем const_cast.
 * В случае, если тут стрельнет - удалить #define ST_SHIFT_WITH_CONST_INSERT
 */
#define ST_SHIFT_WITH_CONST_INSERT

template<typename VType>
class SegmentSetBase : private std::set<SegmentBase<VType>>
{

   typedef std::set<SegmentBase<VType>> MyParent;

   using typename MyParent::iterator;
   using MyParent::insert;
   using MyParent::erase;

public:
   typedef SegmentBase<VType> Segment;
   enum SegmentExistsType{ No, Yes, Particaly };

   using typename MyParent::const_iterator;

   using MyParent::lower_bound;
   using MyParent::upper_bound;
   using MyParent::cbegin;
   using MyParent::cend;


   SegmentSetBase() = default;
   SegmentSetBase( SegmentSetBase&& r) = default;
   SegmentSetBase( const SegmentSetBase& l ) = default;
   SegmentSetBase& operator=( SegmentSetBase&& r ) = default;
   SegmentSetBase& operator=( const SegmentSetBase& r ) = default;

   inline const_iterator SegmentForVal( const VType& val ) const { return Exists_( Segment( val, val ) ); }

   bool Next( VType& val, const_iterator& it );

   bool Prev( VType& val, const_iterator& it );

   const_iterator FirstSegmentLessThen( const VType& then ) const;

   size_t CountLessThen( const VType& then ) const;

   void Insert( Segment&& segment );

   bool Insert( const VType& value );

   inline void Insert( const VType& l, const VType& r ) { Insert( Segment(l, r) ); }

   inline void Insert( const Segment& segment ) { Insert( std::move( Segment( segment ) ) ); }

   void ShiftInsert( Segment&& segment );

   inline void ShiftInsert( const VType& l, const VType& r ) { ShiftInsert( std::move( Segment( l, r ) ) ); }

   inline void ShiftInsert( const VType& val ) { ShiftInsert( std::move( Segment( val, val ) ) ); }

   inline void ShiftInsert( const Segment& segment ) { ShiftInsert( std::move( Segment( segment ) ) ); }

   bool Erase( const VType& value );

   void Erase( const Segment& value );

   inline void Erase( const VType& l, const VType& r ) { Erase( Segment(l, r) ); }

   void ShiftErase( const Segment& segment );

   inline void ShiftErase( const VType& val ) { ShiftErase( Segment( val, val ) ); }

   inline void ShiftErase( const VType& l, const VType& r ) { ShiftErase( Segment(l, r) ); }

   inline bool Exists( const VType& value ) const { return Exists_( Segment( value, value ) ) != cend(); }

   SegmentExistsType Exists( const Segment& segment ) const;

   inline VType Length() const { return m_length; }
   inline size_t size() const { return  MyParent::size(); }

   inline const_iterator begin() const { return MyParent::begin(); }
   inline const_iterator end() const { return MyParent::end(); }
   inline const_iterator rbegin() const { return MyParent::rbegin(); }
   inline const_iterator rend() const { return MyParent::rend(); }

   inline bool empty() const { return Length() == 0; }

   inline void clear() { MyParent::clear(); m_length = Segment::ZERO_LENGTH; }

private:

   void Erase_(const Segment& segment );

   inline bool TryMerge( Segment& s, iterator& with ) { return with != end() && s.Merge( *with ); }

   size_t CountLessThen(const Segment &segment) const;

   const_iterator Exists_(const Segment &val ) const;

   VType m_length = Segment::ZERO_LENGTH;
   mutable std::map<Segment, size_t> m_lessThen;
};

template<typename VType>
bool SegmentSetBase<VType>::Next(VType& val, const_iterator& it)
{
   if( it != end() )
   {
      if( val != it->second )
         ++val;
      else
      {
         ++it;
         if( it != end() )
            val = it->first;
         else
            return true;
      }
      return true;
   }
   return false;
}

template<typename VType>
bool SegmentSetBase<VType>::Prev(VType& val, const_iterator& it)
{
   if( !( it == begin() && val == it->first  ) )
   {
      if( val != it->first )
         --val;
      else
      {
         --it;
         val = it->second;
      }
      return true;
   }
   return false;
}

template<typename VType>
typename SegmentSetBase<VType>::const_iterator SegmentSetBase<VType>::FirstSegmentLessThen(const VType& then) const
{
   if( !empty() )
   {
      auto r = lower_bound( Segment( then, then ) );
      if( r == cend() )
         --r;
      if( !r->Contains(then) && r->second < then )
         return r;
      if( r != cbegin() )
         return --r;
   }
   return end(); // invalid
}

template<typename VType>
size_t SegmentSetBase<VType>::CountLessThen(const VType& then) const
{
   if( Length() != 0 )
   {
      auto prev = then - Segment::IncVal();
      auto at = Exists_( Segment( prev, prev ) );
      if( at != end() )
      {
         Segment s( at->first, prev );
         return CountLessThen( *at ) + (s.IsValid() ? s.Length() : 0);
      }

      if( then > begin()->first )
      {
         auto prev = FirstSegmentLessThen( then );
         if( prev != end() )
            return CountLessThen( *prev ) + prev->Length();
      }
   }
   return 0;
}

template<typename VType>
void SegmentSetBase<VType>::Insert( Segment&& segment )
{
   auto l = lower_bound( Segment( segment.first, segment.first ) );
   auto r = upper_bound( Segment( segment.second, segment.second ) );
   if( r != end() && segment.Merge( *r ) )
      ++r;
   if( l != begin() )
   {
      auto pre_l = l;
      --pre_l;
      if( segment.Merge( *pre_l ) )
         l = pre_l;
   }

   std::list<const Segment*> to_erase;

   for(; l != r; ++l )
      to_erase.push_back( &(*l) );

   for( auto it = to_erase.begin(); it != to_erase.end(); ++it )
      Erase_( **it );

   auto res = insert( std::move( segment ) );
   if( res.second )
      m_length += res.first->Length();
}

template<typename VType>
inline bool SegmentSetBase<VType>::Insert( const VType& value )
{
   if( !Exists( value ) )
   {
      Insert( Segment( value, value ) );
      return true;
   }
   return false;
}


template<typename VType>
void SegmentSetBase<VType>::ShiftInsert(Segment&& segment)
{
   auto l = lower_bound( Segment( segment.first, segment.first ) );
   bool need_insert = true;
   if( l != end() )
   {
      m_lessThen.erase( m_lessThen.find( *l ), m_lessThen.end() );
      auto shift_len = segment.Length();
#ifdef ST_SHIFT_WITH_CONST_INSERT
      if( l->Contains( segment.first ) ) // увеличиваем отрезок, имеющий общую точку с началом вставляемого отрезка, на длину вставляемого
      {
         const_cast<Segment&>(*l).second += shift_len;
         ++l;
         need_insert = false;
      }
      for(; l != end(); ++l ) // сдвигаем не пересекающиеся отрезки, на длину вставляемого
         const_cast<Segment&>(*l).Shift( shift_len, SegmentBase<VType>::mdRight );
#else
      // Этот код не проверялся
      std::list<Segment> shift_seg;
      for(auto it = l; it != end(); ++it)
         shift_seg.emplace_back( std::move( *it ) );
      erase(l, end());

      need_insert = false;
      auto it = shift_seg.begin();
      if( it->Contains( segment.first ) )
      {
         it->second += shift_len;
         Insert( std::move( *it ) );
         ++it;
      }

      for(; it != shift_seg.end(); ++it )
      {
         it->Shift( shift_len );
         Insert( std::move( *it ) );
      }
#endif
      if( !need_insert )
         m_length += shift_len;
   }
   if( need_insert )
      Insert( std::move( segment ) );
}

template<typename VType>
bool SegmentSetBase<VType>::Erase(const VType& value)
{
   // TODO : После того как будет переделан egmentTreeBase<VType>::Erase(const Segment& value) вызвать Erase( Segment( val, val ) ).
   auto r = Exists_( Segment( value, value ) );
   if( r != cend() )
   {
      auto splitted = r->Split( value );
      Erase_( *r );

      if( splitted.first.IsValid() )
         Insert( std::move( splitted.first ) );

      if( splitted.second.IsValid() )
         Insert( std::move( splitted.second ) );
      return true;
   }
   return false;
}

template<typename VType>
void SegmentSetBase<VType>::Erase(const Segment& value)
{
   // TODO: Оптимизирорвать как в SegmentTree<VType>::Insert( Segment segment )
   for( auto i = value.first; i <= value.second; ++i )
      Erase( i );
}


template<typename VType>
void SegmentSetBase<VType>::ShiftErase(const Segment& segment)
{
   auto shift_len = Length();
   Erase( segment );
   shift_len -= Length();

#ifdef ST_SHIFT_WITH_CONST_INSERT
   for( auto it = upper_bound( segment ); it != end(); ++it )
      const_cast<Segment&>(*it).Shift( shift_len, Segment::mdLeft );
#else
   static_assert( false, "Не реализовано" );
#endif
}


template<typename VType>
typename SegmentSetBase<VType>::SegmentExistsType SegmentSetBase<VType>::Exists(const Segment& segment) const
{
   if( find( segment ) != cend() )
      return Yes;

   auto bound = lower_bound( segment );

   if( bound == end() )
   {
      if( (--bound)->Intersect( segment ) )
         return bound->Contains( segment ) ? Yes : Particaly;
   }
   else
   {
      if( bound->Intersect( segment ) || (--bound)->Intersect( segment ) )
         return bound->Contains( segment ) ? Yes : Particaly;
   }

   return No;
}

template<typename VType>
void SegmentSetBase<VType>::Erase_(const Segment& segment)
{
   m_lessThen.erase( m_lessThen.find( segment ), m_lessThen.end() );
   m_length -= segment.Length();
   erase( segment );
}

template<typename VType>
size_t SegmentSetBase<VType>::CountLessThen(const Segment &segment) const
{
   if( m_lessThen.find( segment ) != m_lessThen.end() )
      return m_lessThen.at( segment );

   auto it = find( segment );
   if( it != cbegin() )
   {
      auto& prev = it == end() ? *rbegin() : *(--it);
      if( it != end() )
         return m_lessThen[segment] = CountLessThen( prev ) + prev.Length();
      return CountLessThen( prev ) + prev.Length();
   }

   return m_lessThen[segment] = 0;
}

template<typename VType>
typename SegmentSetBase<VType>::const_iterator SegmentSetBase<VType>::Exists_(const Segment &val) const
{
   if( Length() != 0 )
   {
      auto r = lower_bound( val );
      if( r == cend() )
      {
         if( (--r)->Intersect( val ) )
            return r;
      }
      else if( r->Intersect( val ) )
         return r;

   }
   return cend();
}

#ifdef ST_SHIFT_WITH_CONST_INSERT
#undef ST_SHIFT_WITH_CONST_INSERT
#endif
