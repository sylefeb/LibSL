// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::BezierCurve()
{
  m_Dirty  = true;
  m_Closed = false;
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
void   LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::addControlPoint(
  const T_Vertex& t0,
  const T_Vertex& p,
  const T_Vertex& t1)
{
  m_Dirty  = true;
  m_Closed = false;
  m_ControlPoints.push_back(ControlPoint(t0,p,t1));
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
void  LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::clear()
{
  m_ControlPoints.clear();
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
void   LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::computeArcLength()
{

  if (m_ControlPoints.size() < 2) {
    throw LibSL::Errors::Fatal("BezierCurve::computeArcLength() - invalid curve !");
  }

  // store extremity points
  m_In  = pointAt(0.0f);
  m_Out = pointAt(1.0f - 1e-10f);

  // compute param <-> arclenth table
  float ps  = 0.0f;
  T_Vertex vps = pointAt(ps);
  float al  = 0.0f;
  // compute param/arclen table
  m_Param2Arc.clear();
  int sz = T_ArcLenSampling;
  for (int i=0;i<sz;i++) {
    float pe  = i/(float)(sz-1);
    T_Vertex vpe = pointAt(pe);

    al += length(vps-vpe);
    m_Param2Arc.push_back(std::make_pair(pe,al));

    ps  = pe;
    vps = vpe;
  }
  m_ArcLen = al;
  m_Dirty  = false;
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
T_Vertex LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::pointAt(float t) const
{
  T_Vertex v;

  sl_assert(m_ControlPoints.size() >= 2);

  int nCtrl  = (int)m_ControlPoints.size();
  if ( m_Closed ) nCtrl += 1;

  int it   = (int)(t*(float)(nCtrl-1));
  int p0   = Math::min(it,nCtrl-2);
  int p1   = (p0 + 1) % m_ControlPoints.size();

  float l  = (t * (float)(nCtrl-1)) - p0;
  float l2 = l*l;
  float l3 = l2*l;

  T_Vertex c = 3.0f * (m_ControlPoints[p0].t1 - m_ControlPoints[p0].p );
  T_Vertex b = 3.0f * (m_ControlPoints[p1].t0 - m_ControlPoints[p0].t1) - c;
  T_Vertex a = m_ControlPoints[p1].p - m_ControlPoints[p0].p - c - b;

  v          = a * l3 + b * l2 + c * l + m_ControlPoints[p0].p;

  return (v);
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
T_Vertex LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::tangentAt(float t) const
{
  T_Vertex v;

  sl_assert(m_ControlPoints.size() >= 2);

  int nCtrl  = (int)m_ControlPoints.size();
  if ( m_Closed ) nCtrl += 1;

  int it     = (int)(t*(float)(nCtrl-1));
  int p0     = Math::min(it,nCtrl-2);
  int p1     = (p0 + 1) % m_ControlPoints.size();

  float l    = (t * (float)(nCtrl-1)) - p0;
  float l2   = l*l;

  T_Vertex c = 3.0f * (m_ControlPoints[p0].t1 - m_ControlPoints[p0].p );
  T_Vertex b = 3.0f * (m_ControlPoints[p1].t0 - m_ControlPoints[p0].t1) - c;
  T_Vertex a = m_ControlPoints[p1].p - m_ControlPoints[p0].p - c - b;

  v          = 3.0f * a * l2 + 2.0f * b * l + c;

  return (v);
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
void  LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::tesselate(uint res,LibSL::Memory::Array::Array<T_Vertex>& array)
{
  array.erase();
  array.allocate(res);
  ForArray(array,n) {
    float p  = n / float(array.size()-1) - (n > 0 ? 1e-10f : 0);
    array[n] = pointAt(p);
  }
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
int  LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::locateParam(float q) const
{
  return (locateParam(q,0,(int)m_Param2Arc.size()-1));
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
int  LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::locateParam(float q,int l,int r) const
{
  if (r - l <= 1)
    return (l);
  int m = (l+r)/2;
  if (q < m_Param2Arc[m].first) { // param
    return (locateParam(q,l,m));
  } else {
    return (locateParam(q,m,r));
  }
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
int  LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::locateArc(float q) const
{
  return (locateArc(q,0,(int)m_Param2Arc.size()-1));
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
int  LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::locateArc(float q,int l,int r) const
{
  if (r - l <= 1)
    return (l);
  int m = (l+r)/2;
  if (q < m_Param2Arc[m].second) { // arc
    return (locateArc(q,l,m));
  } else {
    return (locateArc(q,m,r));
  }
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
float  LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::param2arc(float p)
{
  if (m_Dirty) {
    computeArcLength();
  }
  float mp = moduloParam(p);
  int   s0 = locateParam(mp);
  float v0 = m_Param2Arc[s0].second;
  float p0 = m_Param2Arc[s0].first;
  int   s1 = s0 + 1;
  if (s1 >= (int)m_Param2Arc.size()) {
    throw LibSL::Errors::Fatal("BezierCurve::param2arc - error !");
  }
  float v1 = m_Param2Arc[s1].second;
  float p1 = m_Param2Arc[s1].first;
  float  i = (p-p0)/(p1-p0);
  return (v0 * (1.0-i) + v1 * i);
}

// ------------------------------------------------------

template <typename T_Vertex,int T_ArcLenSampling>
float  LibSL::Geometry::BezierCurve<T_Vertex,T_ArcLenSampling>::arc2param(float a)
{
  if (m_Dirty) {
    computeArcLength();
  }
  float ma = moduloArc(a);
  int   s0 = locateArc(ma);
  float v0 = m_Param2Arc[s0].first;
  float a0 = m_Param2Arc[s0].second;
  int   s1 = s0 + 1;
  if (s1 >= (int)m_Param2Arc.size()) {
    throw LibSL::Errors::Fatal("LibSL::Geometry::BezierCurve::arc2param - error !");
  }
  float v1 = m_Param2Arc[s1].first;
  float a1 = m_Param2Arc[s1].second;
  sl_assert( fabs(a1-a0) > 1e-16f );
  float  i = (a-a0)/(a1-a0);
  return (v0 * (1.0f-i) + v1 * i);
}

// ------------------------------------------------------
