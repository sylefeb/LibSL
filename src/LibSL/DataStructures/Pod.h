// ----------------------------------------------------------
// (c) Sylvain Lefebvre 2009-04-16
// Pod class
//
// TODO: efficient dynamic access from member rank
//
// ----------------------------------------------------------

#pragma once

#include <loki/Typelist.h>
#include <LibSL/CppHelpers/CppHelpers.h>

#include <vector>
#include <string>
#include <sstream>

#ifdef BOOST
#include <boost/type_traits/is_array.hpp>
#endif

// ----------------------------------------------------------

namespace LibSL
{
  namespace DataStructures
  {
    namespace POD
    {

      // -------------------------------------------
      // SizeOf
      //   Computes the size of a typelist, summing up
      //   the size of each type within the list
      //
      //   SizeOf< LOKI_TYPELIST_2<float,char> >::value
      // -------------------------------------------

      template <class TList> class SizeOf;

      template <> class SizeOf< Loki::NullType > {public: enum {value = 0}; };

      template <class Head,class Tail> class SizeOf< Loki::Typelist<Head,Tail> >
      {
      private:

      public:

        enum {value = sizeof(typename Head::t_Type) + SizeOf<Tail>::value};
      };

      // -------------------------------------------
      // Address
      //   Returns the address at which a given type
      //   would be located in an array of bytes
      // -------------------------------------------

      template <typename> class ERROR_access_on_unkown_member; // error message

      template <typename T_Pod,typename T_Member> class AddressOf;

      template <typename T_Member> class AddressOf< Loki::NullType, T_Member >
      {
      public:
        enum { value = ERROR_access_on_unkown_member<T_Member>::value };
      };

      template <class Head,class Tail,typename T_Member> class AddressOf< Loki::Typelist<Head,Tail>, T_Member >
      {
      public:
        enum { value = sizeof(typename Head::t_Type) + AddressOf<Tail,T_Member>::value };
      };

      template <class Tail,typename T_Member> class AddressOf< Loki::Typelist<T_Member,Tail>, T_Member >
      {
      public:
        enum { value = 0 };
      };

      // -------------------------------------------
      // TypeAt
      //   TypeAt from Loki modified to report an error
      //   in case the requested index is out of bounds
      // -------------------------------------------

      template <int> class ERROR_access_out_of_bounds; // error message

      template <class TList, unsigned int index> struct TypeAt;

      template <unsigned int i>
      struct TypeAt<Loki::NullType, i>
      {
        enum { value = ERROR_access_out_of_bounds<i>::value };
        typedef Loki::NullType Result;
      };

      template <class Head, class Tail>
      struct TypeAt<Loki::Typelist<Head, Tail>, 0>
      {
        typedef Head Result;
      };

      template <class Head, class Tail, unsigned int i>
      struct TypeAt<Loki::Typelist<Head, Tail>, i>
      {
        typedef typename TypeAt<Tail, i - 1>::Result Result;
      };

      // -------------------------------------------
      // Pod
      //   The Pod main class, key of the Pod system
      // -------------------------------------------

      template <typename T_Pod> struct Pod
      {
        // Pod non-static members must be public so that the Pod
        // is indead a POD in the C++ sense

        /// Members as typelist
        typedef T_Pod   t_Members;

        /// Sizeof Pod data
        enum {size_of = SizeOf<T_Pod>::value};

        /// Array holding the Pod data
        unsigned char data[ size_of ];

        /// Copy operator
        Pod< T_Pod >& operator = (const Pod< T_Pod >& pod)
        {
          memcpy( data, pod.data, size_of );
          return (*this);
        }

        /// First access method:    pod._<MemberName>()

        template <typename T_Member>
        typename T_Member::t_Type& _()
        {
          return *reinterpret_cast<typename T_Member::t_Type*>(data + AddressOf<T_Pod,T_Member>::value);
        }

        template <typename T_Member>
        const typename T_Member::t_Type& _() const
        {
          return *reinterpret_cast<const typename T_Member::t_Type*>(data + AddressOf<T_Pod,T_Member>::value);
        }

        /// Second access method:    pod._<1>()

        template <int T_N>
        typename TypeAt<T_Pod,T_N>::Result::t_Type& _()
        {
          typedef typename TypeAt<T_Pod,T_N>::Result   Result;
          return *reinterpret_cast<typename Result::t_Type*>(data + AddressOf< T_Pod , Result >::value);
        }

        template <int T_N>
        const typename TypeAt<T_Pod,T_N>::Result::t_Type& _() const
        {
          typedef typename TypeAt<T_Pod,T_N>::Result   Result;
          return *reinterpret_cast<const typename Result::t_Type*>(data + AddressOf< T_Pod , Result >::value);
        }

        /// Third access method:     pod(MemberName())

        template <typename T_Member>
        typename T_Member::t_Type& operator()(const T_Member&)
        {
          return *reinterpret_cast<typename T_Member::t_Type*>(data + AddressOf<T_Pod,T_Member>::value);
        }

        template <typename T_Member>
        const typename T_Member::t_Type& operator()(const T_Member&) const
        {
          return *reinterpret_cast<const typename T_Member::t_Type*>(data + AddressOf<T_Pod,T_Member>::value);
        }

      };

      // -------------------------------------------
      // t_MemberNfo
      //   Struct holding information on Pod members
      //   Returned by PodIntrospect
      // -------------------------------------------

      typedef struct {
        const char *name;
        const char *type;
        int         size_of;
        int         address;
        int         attributes;
      } t_MemberNfo;

      // -------------------------------------------
      // Introspect
      //   Builds a table holding Pod member information
      //   for dynamic introspection
      // -------------------------------------------

      template <typename> class Introspect;

      template <> class Introspect< Loki::NullType >
      {
      public:
        Introspect(std::vector<t_MemberNfo>&,int) { }
      };

      template <class Head,class Tail> class Introspect< Loki::Typelist<Head,Tail> >
      : public Introspect< Tail >
      {
      public:
        Introspect(std::vector<t_MemberNfo>& nfos,int address)
          : Introspect< Tail >(nfos,address + sizeof(typename Head::t_Type))
        {
          t_MemberNfo m;
          m.name       = Head::memberName();
          m.type       = Head::memberTypeName();
          m.size_of    = sizeof(typename Head::t_Type);
          m.address    = address;
          m.attributes = Head::attributes();
          nfos.push_back(m);
        }
      };

      // -------------------------------------------
      // IPodIntrospect
      //   Interface to Pod dynamic instrospection
      // -------------------------------------------

      class IPodIntrospect
      {
      public:
        virtual unsigned int       numMembers()     const =0;
        virtual const t_MemberNfo& memberNfo(int n) const =0;
      };

      // -------------------------------------------
      // PodIntrospect
      //   Pod dynamic instrospection
      // -------------------------------------------

      template <typename> class PodIntrospect;

      template <typename T_Pod> class PodIntrospect<Pod<T_Pod> > : public IPodIntrospect
      {
      private:

        std::vector<t_MemberNfo> m_MemberNfos;

      public:

        PodIntrospect()
        {
          Introspect<T_Pod> introspect(m_MemberNfos,0);
          std::reverse(m_MemberNfos.begin(),m_MemberNfos.end());
        }

        unsigned int       numMembers() const
        {
          return Loki::TL::Length<T_Pod>::value;
        }

        const t_MemberNfo& memberNfo(int n) const
        {
          return m_MemberNfos[n];
        }

      };

      // -------------------------------------------
      // IPodStringInterface
      //   Generic interface to access pod members
      //   through pairs of (member_name,value) strings
      // -------------------------------------------

      class IPodStringInterface
      {
      public:
        virtual unsigned int       numMembers()     const =0;
        virtual const t_MemberNfo& memberNfo(int n) const =0;
        virtual void               set(const std::string& memberName,const std::string& value) =0;
        virtual std::string        get(const std::string& memberName) const                    =0;
      };

      // -------------------------------------------
      // PodStringInterface
      //   Interface to access pod members
      //   through pairs of (member_name,value) strings
      // -------------------------------------------

      namespace PodStringInterfaceDetail
      {
        // -------------------------------------------
        // Setter for string interface
        // -------------------------------------------

        template <typename T_list, typename T_list2 = T_list> class Setter;

        template <typename T_Pod> class Setter < T_Pod, Loki::NullType >
        {
        protected:
          bool        m_Success;
        public:
          Setter(Pod<T_Pod> &_pod,const std::string& memberName,const std::string& value) { m_Success = false; }
          bool        success() const { return m_Success; }
        };

        template <typename T_Pod, class Head,class Tail> class Setter<T_Pod, Loki::Typelist<Head,Tail> > : public Setter<T_Pod, Tail >
        {
        protected:
          typedef Setter< T_Pod, Tail > Base;
          using   Base::m_Success;
        public:
          Setter(Pod<T_Pod> &_pod,const std::string& memberName,const std::string& value)
            : Base(_pod,memberName,value)
          {
            std::string strval = value;
            if (memberName == std::string(Head::memberName())) {
#ifdef BOOST
              if ( boost::is_array< Head::t_Type >::value ) { // FIXME TODO this is a sanity check for char[]
                //                                               but it is fragile. Ideally other array types should trigger an error
                if ( value.length() >= sizeof(Head::t_Type) ) {
                  // clamp the string
                  strval = strval.substr( 0, sizeof(Head::t_Type)-1 );
                }
              }
#endif
              std::istringstream is(strval);
              is >> _pod.template _< Head >();
              m_Success = true;
            }
          }

        };

        // -------------------------------------------
        // Getter for string interface
        // -------------------------------------------

        template <typename T_list, typename T_list2 = T_list> class Getter;

        template <typename T_Pod> class Getter< T_Pod, Loki::NullType >
        {
        protected:
          std::string m_Value;
          bool        m_Success;
        public:
          Getter(const Pod<T_Pod> &pod,const std::string& memberName) { m_Success = false; }
          std::string getValue()      { return m_Value;   }
          bool        success() const { return m_Success; }
        };

        template <typename T_Pod, class Head,class Tail> class Getter< T_Pod, Loki::Typelist<Head,Tail> >
        : public Getter< T_Pod, Tail >
        {
        protected:
          typedef Getter< T_Pod, Tail > Base;
          using Base::m_Value;
          using Base::m_Success;
        public:
          Getter(const Pod<T_Pod> &pod,const std::string& memberName)
            : Base(pod,memberName)
          {
            if (memberName == std::string(Head::memberName())) {
              std::ostringstream os;
              os << pod.template _<Head>();
              m_Value   = os.str();
              m_Success = true;
            }
          }
        };

        // -------------------------------------------
        // Indexer for string interface
        // -------------------------------------------

        template <typename T_list, typename T_list2 = T_list> class Indexer;

        template <typename T_Pod> class Indexer< T_Pod, Loki::NullType >
        {
        protected:
          std::string m_Value;
          int         m_Index;
        public:
          Indexer(const Pod<T_Pod> &pod,const std::string& memberName,int) { m_Index = -1; }
          int         result() const { return m_Index; }
        };

        template <typename T_Pod, class Head,class Tail> class Indexer< T_Pod, Loki::Typelist<Head,Tail> >
        : public Indexer< T_Pod, Tail >
        {
        protected:
          typedef Indexer< T_Pod, Tail > Base;
          using Base::m_Index;
        public:
          Indexer(const Pod<T_Pod> &pod,const std::string& memberName,int n = 0)
            : Base(pod,memberName,n + 1)
          {
            if (memberName == std::string(Head::memberName())) {
              m_Index = n;
            }
          }
        };

      } // namespace PodStringInterfaceDetail

      template <typename> class PodStringInterface;

      template <typename T_Pod> class PodStringInterface<Pod<T_Pod> > : public IPodStringInterface
      {
      private:

        typedef PodIntrospect<T_Pod> t_Introspect;

        Pod<T_Pod>&                m_Pod;
        PodIntrospect<Pod<T_Pod> > m_Introspect;

      public:

        PodStringInterface(Pod<T_Pod> &_pod) : m_Pod(_pod)  {  }

        unsigned int       numMembers() const
        {
          return m_Introspect.numMembers();
        }

        const t_MemberNfo& memberNfo(int n) const
        {
          return m_Introspect.memberNfo(n);
        }

        const t_MemberNfo& memberNfo(const std::string& memberName) const
        {
          PodStringInterfaceDetail::Indexer<T_Pod> indexer(m_Pod,memberName);
          int n = indexer.result();
          return m_Introspect.memberNfo(n);
        }

        void set(const std::string& memberName,const std::string& value)
        {
          // make sure strings contain no spaces as istringstream would stop at them
          std::string strsafevalue = value;
          LibSL::CppHelpers::replaceAll(strsafevalue, "&", "&1");
          LibSL::CppHelpers::replaceAll(strsafevalue, " ", "&2");
          PodStringInterfaceDetail::Setter<T_Pod> setter(m_Pod,memberName,strsafevalue);
          if ( ! setter.success() ) {
            //throw Errors::Fatal("[PodStringInterface::set] - could not find member '%s'",memberName.c_str());
          }
        }

        std::string get(const std::string& memberName) const
        {
          PodStringInterfaceDetail::Getter<T_Pod> getter(m_Pod,memberName);
          if ( ! getter.success() ) {
            //throw Errors::Fatal("[PodStringInterface::get] - could not find member '%s'",memberName.c_str());
            return "";
          }
          std::string strsafevalue = getter.getValue();
          LibSL::CppHelpers::replaceAll(strsafevalue, "&2", " ");
          LibSL::CppHelpers::replaceAll(strsafevalue, "&1", "&");
          return strsafevalue;
        }

        bool exists(const std::string& memberName) const
        {
          PodStringInterfaceDetail::Indexer<T_Pod> indexer(m_Pod,memberName);
          return indexer.result() > -1;
        }

        int index(const std::string& memberName) const
        {
          PodStringInterfaceDetail::Indexer<T_Pod> indexer(m_Pod, memberName);
          return indexer.result();
        }

      };

      // -------------------------------------------
      // Append two pods to form a new, larger one
      // -------------------------------------------

      template < class, class > class PodAppend;

      template <class T_Pod> class PodAppend< T_Pod, Loki::NullType >
      {
      public:
        typedef T_Pod Result;
      };

      template <class T_Pod> class PodAppend< Loki::NullType , T_Pod >
      {
      public:
        typedef T_Pod Result;
      };

      template < class T_PodA, class T_PodB > class PodAppend
      {
      public:
        typedef Pod< typename ::Loki::TL::Append< typename T_PodA::t_Members, typename T_PodB::t_Members >::Result > Result;
      };

    } //namespace POD
  } //namespace DataStructures
} //namespace LibSL

// -------------------------------------------
// POD_MEMBER
//   Macro to declare new Pod members
// -------------------------------------------

#define POD_MEMBER(T,N)                                \
  typedef struct {                                     \
  typedef T t_Type;                                    \
  static const char *memberTypeName()   { return #T; } \
  static const char *memberName()       { return #N; } \
  static int         attributes()       { return 0;  } \
} N;

#define POD_MEMBER_ATTR(T,N,A)                         \
  typedef struct {                                     \
  typedef T t_Type;                                    \
  static const char *memberTypeName()   { return #T; } \
  static const char *memberName()       { return #N; } \
  static int         attributes()       { return A;  } \
} N;

// -------------------------------------------
// PODn
//   Macros to declare Pods
// -------------------------------------------

#define POD1(P0                                ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_1(P0                                ) >
#define POD2(P0,P1                             ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_2(P0,P1                             ) >
#define POD3(P0,P1,P2                          ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_3(P0,P1,P2                          ) >
#define POD4(P0,P1,P2,P3                       ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_4(P0,P1,P2,P3                       ) >
#define POD5(P0,P1,P2,P3,P4                    ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_5(P0,P1,P2,P3,P4                    ) >
#define POD6(P0,P1,P2,P3,P4,P5                 ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_6(P0,P1,P2,P3,P4,P5                 ) >
#define POD7(P0,P1,P2,P3,P4,P5,P6              ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_7(P0,P1,P2,P3,P4,P5,P6              ) >
#define POD8(P0,P1,P2,P3,P4,P5,P6,P7           ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_8(P0,P1,P2,P3,P4,P5,P6,P7           ) >
#define POD9(P0,P1,P2,P3,P4,P5,P6,P7,P8        ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_9(P0,P1,P2,P3,P4,P5,P6,P7,P8        ) >
#define POD10(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9    ) LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_10(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9    ) >
#define POD11(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10)                  LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_11(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10) >
#define POD12(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11)              LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_12(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11) >
#define POD13(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12)          LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_13(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12) >
#define POD14(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13)      LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_14(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13) >
#define POD15(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14)  LibSL::DataStructures::POD::Pod< LOKI_TYPELIST_15(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14) >

// ----------------------------------------------------------
