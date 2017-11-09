// SL 2010-06-08

namespace LibSL {
  namespace DataStructures {
    namespace GraphAlgorithms {

      enum e_Side {Source,Sink,Disconnected};

      template <class T_Graph, class T_EdgeCost>
      class RndCut
      {
      private:

        typedef struct s_EqClass {
          struct s_EqClass *parent;
        } t_EqClass;

        class SelectNfo 
        {
        public:
          SelectNfo() : eid(-1), cdf(0.0) { }
          SelectNfo(t_EdgeId eid_,double cdf_) : eid(eid_), cdf(cdf_) { }
          t_EdgeId eid;
          double   cdf;
        };

        typedef Array<SelectNfo> t_EdgeSelect;

        Array<t_EqClass>              m_ClassIds;
        t_EdgeSelect                  m_EdgeSelect;
        double                        m_TotCdf;
        double                        m_CutCost;
        t_EqClass                    *m_EqSource;
        t_EqClass                    *m_EqSink;
        int                           m_NumTests;
        int                           m_NumCalls;
        int                           m_NumRebuilds;

        t_EqClass *gotoParent(t_EqClass *eq)
        {
          if (eq->parent == eq) {
            return eq;
          } else {
            return gotoParent( eq->parent );
          }
        }

        t_EqClass *classId(t_NodeId n)
        {
          t_EqClass *c         = gotoParent( &(m_ClassIds[n]) );
          m_ClassIds[n].parent = c; // update the parent record
          return c;
        }

        void mergeClasses(const T_Graph& g,t_EqClass *eq_master,t_EqClass *eq_merged)
        {
          sl_assert( eq_master->parent == eq_master );
          sl_assert( eq_merged->parent == eq_merged );
          eq_merged->parent = eq_master;
        }

        t_EdgeId selectEdge(const T_Graph& g)
        {
          int    ntests   = 0;
          int    selected = -1;
          m_NumCalls ++;
          while (1) {
            // count tests for this call
            ntests ++;
            // find edge
            double t = rnd() * m_TotCdf;
            int    l = 0;
            int    r = m_EdgeSelect.size()-1;
            while (r > l) {
              int m = (l+r)/2;
              // cerr << l << ' ' << m << ' ' << r << endl;
              if ( t < m_EdgeSelect[m].cdf ) {
                r = m;
              } else {
                l = m+1;
              }
            }
            selected = m_EdgeSelect[l].eid;
            if ( isCollapsed( g , selected ) || isInCut( g , selected ) ) {
              /// failed
              // -> must update array?
              if ( (m_NumCalls > /*m_EdgeSelect.size()/100*/ 128 && (m_NumTests / (float)m_NumCalls) > 2.0f)
                || ntests >= m_EdgeSelect.size() /* can happen since everything may quickly collapse */ ) {
                if (ntests >= m_EdgeSelect.size()) { cerr << Console::red << "WARNING" << endl; }
                cerr << (m_NumTests / (float)m_NumCalls) << ' ';
                cerr << m_NumTests << ' ';
                cerr << ntests << endl;
                rebuildEdgeSelectInfo( g );
                m_NumRebuilds ++;
                m_NumTests = 0;
                m_NumCalls = 1;
                if (m_EdgeSelect.size() == 0) { return -1; }
              }
            } else {
              /// success, stop here
              break;
            }
          }
          m_NumTests += ntests; // count tests globally
          return selected;
        }

        bool isCollapsed( const T_Graph& g, t_EdgeId e )
        {
          t_EqClass *eqA = classId( g.edges()[e].nodeA() );
          t_EqClass *eqB = classId( g.edges()[e].nodeB() );
          return (eqA == eqB);
        }

        bool isInCut( const T_Graph& g, t_EdgeId e )
        {
          t_EqClass *eqA = classId( g.edges()[e].nodeA() );
          t_EqClass *eqB = classId( g.edges()[e].nodeB() );
          return ( (eqA == m_EqSource && eqB == m_EqSink) || (eqB == m_EqSource && eqA == m_EqSink) );
        }

        void rebuildEdgeSelectInfo( const T_Graph& g )
        {
          T_EdgeCost ecost;
          if ( m_EdgeSelect.empty() ) {
            // first build
            m_EdgeSelect.allocate( g.edges().size() );
            m_TotCdf = 0.0;
            ForIndex(e,g.edges().size()) {
              t_EdgeId eid = (t_EdgeId)e;
              m_TotCdf           += ecost( g , g.edges()[eid].from() , eid );
              m_EdgeSelect[e]     = SelectNfo( eid , m_TotCdf );
            }
          } else {
            // maintainance
            int num  = 0;
            // remove collapsed and cut edges

            double cdfused = 0.0;
            ForIndex(i,m_EdgeSelect.size()) {
              t_EdgeId eid = m_EdgeSelect[i].eid;
              if ( isCollapsed( g , eid ) || isInCut( g , eid ) ) {
                cdfused += ecost( g , g.edges()[eid].from() , eid );
              }
            }
            cdfused = cdfused * 100.0 / m_TotCdf;
            
            m_TotCdf = 0.0;
            ForIndex(i,m_EdgeSelect.size()) {
              t_EdgeId eid = m_EdgeSelect[i].eid;
              if ( isCollapsed( g , eid ) ) {
                // skip this edge
              } else if ( isInCut( g , eid ) ) {
                // skip this edge
                m_CutCost += ecost( g , g.edges()[eid].from() , eid );
              } else {
                // keep this edge
                m_TotCdf         += ecost( g , g.edges()[eid].from() , eid );
                m_EdgeSelect[num] = SelectNfo( eid , m_TotCdf );
                num ++;
              }
            }

            cerr << Console::white << sprint("cdf used: %.2f %%    array used: %.2f %%\n",cdfused, (m_EdgeSelect.size() - num) * 100.0f / m_EdgeSelect.size() ) << Console::gray << endl;

            std::cerr << "[rebuildEdgeSelectInfo] " << num << "  cdf = " << m_TotCdf << std::endl;
            m_EdgeSelect.truncate(num);
          }
        }

      public:

        RndCut() : m_CutCost(0.0), m_TotCdf(0.0), m_EqSource(NULL), m_EqSink(NULL), m_NumCalls(0), m_NumTests(0), m_NumRebuilds(0) { }

        // compute a cut
        void cut( const T_Graph& g , t_NodeId source , t_NodeId sink, Array<int>& _sides )
        {
          // init internals
          m_EdgeSelect.erase();
          m_NumCalls = 0;
          m_NumTests = 0;
          m_CutCost  = 0.0;
          m_NumRebuilds = 0;
          // init eq classes
          m_ClassIds.erase();
          m_ClassIds.allocate( g.nodes().size() );
          ForArray(m_ClassIds,i) {
            m_ClassIds[i].parent = &(m_ClassIds[i]);
          }
          // get class ids of source / sink
          m_EqSource = &(m_ClassIds[source]);
          m_EqSink   = &(m_ClassIds[sink  ]);
          // prepare edge selection
          rebuildEdgeSelectInfo( g );
          // cerr << "tot_cdf: " << tot_cdf << endl;
          while (1) {
            // select an edge          
            t_EdgeId e        = selectEdge( g );
            if ( e < 0 ) { /* no more edges, stop here */ break; }
            // get nodes and class ids
            t_NodeId to       = g.edges()[e].to();
            t_NodeId from     = g.edges()[e].from();
            t_EqClass *eqto   = classId( to   );
            t_EqClass *eqfrom = classId( from );
            sl_assert( ! (eqto   == m_EqSink && eqfrom == m_EqSource) );
            sl_assert( ! (eqfrom == m_EqSink && eqto   == m_EqSource) );
            // collapse
            if (eqto == m_EqSink || eqto == m_EqSource) {
              mergeClasses( g, eqto, eqfrom);
            } else {
              mergeClasses( g, eqfrom, eqto);
            }
            // both nodes now belong to the same class
            sl_assert( classId( to   ) == classId( from ) );
          };
          // cerr << "tot_cdf: " << tot_cdf << endl;
          // fill-in result
          _sides.allocate( g.nodes().size() );
          _sides.fill( Disconnected );
          ForIndex(n, g.nodes().size() ) {
            t_EqClass *eq = classId( n );
            if (eq == m_EqSource) {
              _sides[n] = Source;
            } else if (eq == m_EqSink) {
              _sides[n] = Sink;
            }            
          }
          // done!
          m_ClassIds.erase();
          cerr << "Num edge array rebuilds: " << m_NumRebuilds << endl;
          cerr << "Num edges              : " << g.edges().size() << endl;
        }

        // return the cost of the last cut
        double cutCost() const { return m_CutCost; }

      };

    } //namespace LibSL::DataStructures::GraphAlgorithms
  } //namespace LibSL::DataStructures
} //namespace LibSL
