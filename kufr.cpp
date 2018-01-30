/* kufr.cpp */

#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>

using namespace std;

#endif /* __PROGTEST_ */

class CItem
{
  protected:
    int m_Weight;

  public:
   CItem ( int w )
     : m_Weight ( w ) {}

   CItem ( const CItem & obj )
     : m_Weight ( obj.m_Weight )
   {}

   virtual ~CItem ( void ) {}

   int get_weight ( void ) const
   {
     return m_Weight;
   }

   virtual shared_ptr <CItem> clone ( void ) const { return nullptr; }
   virtual string stringify ( void ) const { return string ( "dummy" ); }
   virtual bool is_dangerous ( void ) const { return false; }
   virtual string class_name ( void ) const { return string ( "CItem" ); }

   virtual bool is_equal ( const CItem & other ) const = 0;

   friend bool operator == ( const CItem & lhs, const CItem & rhs )
   {
     if ( lhs . class_name () != rhs . class_name () )
       return false;
     return lhs . is_equal ( rhs );
   }
};

class CKnife : public CItem
{
  private: 
    int m_bladeLength;

  public:
    CKnife ( int bladeLength )
      : CItem ( 100 ), m_bladeLength ( bladeLength )
    {}

    ~CKnife ( void ) {}

    shared_ptr <CItem> clone ( void ) const
    {
      shared_ptr <CKnife> ret ( new CKnife ( m_bladeLength ) );
      return ret;
    }

    string stringify ( void ) const
    {
      string ret ( string ( "Knife, blade: " ) + to_string ( m_bladeLength ) + string ( " cm" ) );
      return ret; 
    }

    bool is_dangerous ( void ) const
    {
      return m_bladeLength > 7;
    }

    string class_name ( void ) const { return string ( "CKnife" ); }

    virtual bool is_equal ( const CItem & other ) const
    {
      const CKnife & other_derived = dynamic_cast <const CKnife &> ( other );
      return ( m_bladeLength == other_derived . m_bladeLength );
    }
};

class CClothes : public CItem
{
  private:
    string m_Desc;

  public:
    CClothes ( const string & desc )
      : CItem ( 500 ), m_Desc ( desc )
    {}

    ~CClothes ( void ) {}

    shared_ptr <CItem> clone ( void ) const
    {
      shared_ptr <CClothes> ret ( new CClothes ( m_Desc ) );
      return ret;
    }

    string stringify ( void ) const
    {
      string ret ( string ( "Clothes (" ) + m_Desc + string ( ")" ) );
      return ret; 
    }

    string class_name ( void ) const { return string ( "CClothes" ); }

    virtual bool is_equal ( const CItem & other ) const
    {
      const CClothes & other_derived = dynamic_cast <const CClothes &> ( other );
      return ( m_Desc == other_derived . m_Desc );
    }
};

class CShoes : public CItem
{
  public:
    CShoes ( void )
      : CItem ( 750 )
    {}

    ~CShoes ( void ) {}

    shared_ptr <CItem> clone ( void ) const
    {
      shared_ptr <CShoes> ret ( new CShoes () );
      return ret;
    }

    string stringify ( void ) const
    {
      string ret ( string ( "Shoes" ) );
      return ret; 
    }

    string class_name ( void ) const { return string ( "CShoes" ); }

    virtual bool is_equal ( const CItem & ) const
    {
      return true;
    }
};

class CMobile : public CItem
{
  private:
    string m_Manufacturer,
           m_Model;
  public:
    CMobile ( const string & manufacturer, const string & model )
      : CItem ( 150 ), m_Manufacturer ( manufacturer ), m_Model ( model )
    {}

    ~CMobile ( void ) {}

    shared_ptr <CItem> clone ( void ) const
    {
      shared_ptr <CMobile> ret ( new CMobile ( m_Manufacturer, m_Model ) );
      return ret;
    }

    string stringify ( void ) const
    {
      string ret ( string ( "Mobile " ) + m_Model + string ( " by: " ) + m_Manufacturer );
      return ret; 
    }

    bool is_dangerous ( void ) const
    {
      return ( m_Manufacturer == "Samsung" && m_Model == "Galaxy Note S7" );
    }

    string class_name ( void ) const { return string ( "CMobile" ); }

    virtual bool is_equal ( const CItem & other ) const
    {
      const CMobile & other_derived = dynamic_cast <const CMobile &> ( other );
      return (( m_Model == other_derived . m_Model ) && ( m_Manufacturer == other_derived . m_Manufacturer ));
    }
};

/*------------------------------------------------------------------------------------------------------------------------*/

class CLuggage
{
  protected:
    vector < shared_ptr<CItem> > m_Items;

  public:
    CLuggage ( void ) {}

    CLuggage & Add ( const CItem & add )
    { 
      shared_ptr<CItem> tmp = add.clone ( );
      m_Items . push_back ( tmp );
      return *this;
    }

    virtual int Weight ( void ) const = 0;

    int Count ( void ) const
    {
      return m_Items . size ();
    }

    bool Danger ( void ) const
    {
      for ( auto it = m_Items.begin(); it != m_Items.end(); it++ )
        if ( (*it)->is_dangerous() )
          return true;
      return false;
    }

    bool find ( const CItem & needle, set <size_t> & found ) const
    {
      size_t cnt = 0; // iterator index counter
      string needle_class = needle . class_name ();

      for ( auto it = m_Items.begin(); it != m_Items.end(); it++, cnt++ )
        if ( needle_class == (*it)->class_name() && needle == **it )
        {
          set <size_t>::iterator lookup = found . find ( cnt );
          if ( lookup == found . end() )
          {
            found . insert ( cnt );
            return true;
          }
        }

      return false;
    }

    bool IdenticalContents ( const CLuggage & other ) const
    {
      if ( Count() != other.Count() )
        return false;

      set <size_t> found; // indexy nalezených vecí

      for ( auto it = m_Items.begin(); it != m_Items.end(); it++ )
        if ( other . find ( **it, found ) == false )
          return false;

      return true; 
    }
};

/*------------------------------------------------------------------------------------------------------------------------*/

class CSuitcase : public CLuggage
{
  private:
    int m_W, m_H, m_D;

  public:
    CSuitcase ( int w, int h, int d )
      : m_W ( w ), m_H ( h ), m_D ( d ) {}

    int Weight ( void ) const
    {
      int ret = 0;
      for ( auto it = m_Items.begin(); it != m_Items.end(); it++ )
        ret += (*it)->get_weight();
      return ret + 2000;
    }

    friend ostream & operator << ( ostream & os, const CSuitcase & obj )
    {
      os << "Suitcase " << obj.m_W << "x" << obj.m_H << "x" << obj.m_D << endl;
      for ( auto it = obj.m_Items.begin (); it != obj.m_Items.end (); it++ )
      {
        if ( *it == obj.m_Items.back () )
          os << "\\-" << (*it)->stringify () << "\n";
        else
          os << "+-" << (*it)->stringify () << "\n";
      }
      return os;
    }
};

class CBackpack : public CLuggage
{
  public:
    CBackpack ( void ) {}

    int Weight ( void ) const
    {
      int ret = 0;
      for ( auto it = m_Items.begin(); it != m_Items.end(); it++ )
        ret += (*it)->get_weight();
      return ret + 1000;
    }

    friend ostream & operator << ( ostream & os, const CBackpack & obj )
    {
      os << "Backpack" << endl;

      for ( auto it = obj.m_Items.begin (); it != obj.m_Items.end (); it++ )
        if ( *it == obj.m_Items.back () )
          os << "\\-" << (*it)->stringify () << "\n";
        else
          os << "+-" << (*it)->stringify () << "\n";

      return os;
    }
};

#ifndef __PROGTEST__
int main ( void )
{
  CSuitcase x ( 1, 2, 3 );
  CBackpack y;
  ostringstream os;
  x . Add ( CKnife ( 7 ) );
  x . Add ( CClothes ( "red T-shirt" ) );
  x . Add ( CClothes ( "black hat" ) );
  x . Add ( CShoes () );
  x . Add ( CClothes ( "green pants" ) );
  x . Add ( CClothes ( "blue jeans" ) );
  x . Add ( CMobile ( "Samsung", "J3" ) );
  x . Add ( CMobile ( "Tamtung", "Galaxy Note S7" ) );
  os . str ( "" );
  os << x;
  //cout << os.str();
  assert ( os . str () == "Suitcase 1x2x3\n"
           "+-Knife, blade: 7 cm\n"
           "+-Clothes (red T-shirt)\n"
           "+-Clothes (black hat)\n"
           "+-Shoes\n"
           "+-Clothes (green pants)\n"
           "+-Clothes (blue jeans)\n"
           "+-Mobile J3 by: Samsung\n"
           "\\-Mobile Galaxy Note S7 by: Tamtung\n" );
  assert ( x . Count () == 8 );
  assert ( x . Weight () == 5150 );
  assert ( !x . Danger () );
  x . Add ( CKnife ( 8 ) );
  os . str ( "" );
  os << x;
  assert ( os . str () == "Suitcase 1x2x3\n"
           "+-Knife, blade: 7 cm\n"
           "+-Clothes (red T-shirt)\n"
           "+-Clothes (black hat)\n"
           "+-Shoes\n"
           "+-Clothes (green pants)\n"
           "+-Clothes (blue jeans)\n"
           "+-Mobile J3 by: Samsung\n"
           "+-Mobile Galaxy Note S7 by: Tamtung\n"
           "\\-Knife, blade: 8 cm\n" );
  assert ( x . Count () == 9 );
  assert ( x . Weight () == 5250 );
  assert ( x . Danger () );
  y . Add ( CKnife ( 7 ) )
    . Add ( CClothes ( "red T-shirt" ) )
    . Add ( CShoes () );
  y . Add ( CMobile ( "Samsung", "Galaxy Note S7" ) );
  y . Add ( CShoes () );
  y . Add ( CClothes ( "blue jeans" ) );
  y . Add ( CClothes ( "black hat" ) );
  y . Add ( CClothes ( "green pants" ) );
  os . str ( "" );
  os << y;
  assert ( os . str () == "Backpack\n"
           "+-Knife, blade: 7 cm\n"
           "+-Clothes (red T-shirt)\n"
           "+-Shoes\n"
           "+-Mobile Galaxy Note S7 by: Samsung\n"
           "+-Shoes\n"
           "+-Clothes (blue jeans)\n"
           "+-Clothes (black hat)\n"
           "\\-Clothes (green pants)\n" );
  assert ( y . Count () == 8 );
  assert ( y . Weight () == 4750 );
  assert ( y . Danger () );
  y . Add ( CMobile ( "Samsung", "J3" ) );
  y . Add ( CMobile ( "Tamtung", "Galaxy Note S7" ) );
  y . Add ( CKnife ( 8 ) );
  os . str ( "" );
  os << y;
  assert ( os . str () == "Backpack\n"
           "+-Knife, blade: 7 cm\n"
           "+-Clothes (red T-shirt)\n"
           "+-Shoes\n"
           "+-Mobile Galaxy Note S7 by: Samsung\n"
           "+-Shoes\n"
           "+-Clothes (blue jeans)\n"
           "+-Clothes (black hat)\n"
           "+-Clothes (green pants)\n"
           "+-Mobile J3 by: Samsung\n"
           "+-Mobile Galaxy Note S7 by: Tamtung\n"
           "\\-Knife, blade: 8 cm\n" );
  assert ( y . Count () == 11 );
  assert ( y . Weight () == 5150 );
  assert ( y . Danger () );
  assert ( !x . IdenticalContents ( y ) );
  assert ( !y . IdenticalContents ( x ) );
  x . Add ( CMobile ( "Samsung", "Galaxy Note S7" ) );
  assert ( !x . IdenticalContents ( y ) );
  assert ( !y . IdenticalContents ( x ) );
  x . Add ( CShoes () );
  assert ( x . IdenticalContents ( y ) );
  assert ( y . IdenticalContents ( x ) );
  assert ( y . IdenticalContents ( y ) );
  assert ( x . IdenticalContents ( x ) );
  return 0;
}
#endif /* __PROGTEST__ */
