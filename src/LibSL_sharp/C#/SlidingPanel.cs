// SL: 2010, Cecill-C as part of LibSL

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace LibSL_sharp
{
  public partial class SlidingPanel : UserControl
  {
    static private int m_BorderWidth = 25;
    private Control m_Parent = null;

    public delegate void OnSlidingHandler(SlidingPanel p);
    public event OnSlidingHandler OnSliding;

    public delegate void OnFoldedHandler(SlidingPanel p);
    public event OnFoldedHandler OnFolded;

    public delegate void OnUnfoldedHandler(SlidingPanel p);
    public event OnUnfoldedHandler OnUnfolded;

    private EventHandler m_ParentResizeEv = null;
    private OnSlidingHandler m_SiblingSlidingHandler = null;

    private bool m_Unfold = false;
    private bool m_Fold = false;
    private bool m_Folded = true;
    public  bool Folded { get { return m_Folded; } }
    private bool m_Resizable = true;
    private int m_UnfoldSpeed = 10; // pixels per second
    private int m_FoldSpeed = 10;   // pixels per second
    private string m_Title = "No title";
    private Font m_Font = new Font("Arial", 12);
    private SlidingPanel m_Sibling = null;
    private int m_FoldPos = 0;
    private int m_UnfoldPos = 0;
    private Brush m_BorderBrush = Brushes.Black;

        public enum Side
    {
      Left, Right
    }

    private Side m_Side = Side.Right;
    public Side AnchorSide { get { return m_Side; } set { m_Side = value; ComputeSizesAndPositions(); } }

    public string Title 
    { 
      get { return m_Title; }
      set { if (m_Title != value) { m_Title = value; Refresh(); } } 
    }

    public Brush BorderBrush
    {
      get { return m_BorderBrush; }
      set { if (m_BorderBrush != value) { m_BorderBrush = value; Refresh(); } }
    }

    public bool Resizable { get { return m_Resizable; } set { m_Resizable = value; } }

    static public int BorderWidth { get {return m_BorderWidth;} }

    public SlidingPanel Sibling
    {
      set
      {
        if (value.AnchorSide != m_Side) throw new Exception("AnchorSide of sibling must be the same!");
        if (m_Sibling != null)
        {
          m_Sibling.OnSliding -= m_SiblingSlidingHandler;
        }
        m_Sibling = value;
        m_SiblingSlidingHandler = new OnSlidingHandler(sibling_Sliding);
        m_Sibling.OnSliding += m_SiblingSlidingHandler;
      }
    }

    public SlidingPanel()
    {
      InitializeComponent();
      Width = m_BorderWidth;
    }

    private void SlidingPanel_ParentChanged(object sender, EventArgs e)
    {
      if (m_Parent != null)
      {
        m_Parent.Resize -= m_ParentResizeEv;
      }
      m_Parent = Parent;
      m_ParentResizeEv = new EventHandler(parent_Resize);
      Parent.Resize += m_ParentResizeEv;
      ComputeSizesAndPositions();
    }

    void parent_Resize(object sender, EventArgs e)
    {
      ComputeSizesAndPositions();
    }

    private void sibling_Sliding(SlidingPanel p)
    {
      if (!m_Fold && !m_Unfold)
      {
        ComputeSizesAndPositions();
        if (OnSliding != null)
        {
          OnSliding(this);
        }
      }
    }

    public void AddControl(Control c)
    {
      c.Height = Height;
      if (AnchorSide == Side.Right)
      {
        Width = Math.Max(Width, c.Width + m_BorderWidth);
        c.Left = m_BorderWidth;
        c.Top = 0;
        Controls.Add(c);
      }
      else
      {
        Width = Math.Max(Width, c.Width + m_BorderWidth);
        c.Left = 0;
        c.Top = 0;
        Controls.Add(c);
      }
    }

    void ComputeSizesAndPositions()
    {
      if (Parent == null) return;
      ComputeFoldPositions();
      if (m_Folded)
      {
        Left = m_FoldPos;
      }
      else
      {
        Left = m_UnfoldPos;
      }
    }

    void ComputeFoldPositions()
    {
      if (m_Sibling != null)
      {
        if (AnchorSide == Side.Right)
        {
          m_UnfoldPos = m_Sibling.Left - Width;
          m_FoldPos = m_Sibling.Left - m_BorderWidth;
        }
        else
        {
          m_UnfoldPos = m_Sibling.Right;
          m_FoldPos = m_Sibling.Right - Width + m_BorderWidth;
        }
      }
      else
      {
        if (AnchorSide == Side.Right)
        {
          m_UnfoldPos = Parent.ClientRectangle.Left + Parent.ClientSize.Width - Width;
          m_FoldPos = Parent.ClientRectangle.Left + Parent.ClientSize.Width - m_BorderWidth;
        }
        else
        {
          m_UnfoldPos = Parent.ClientRectangle.Left;
          m_FoldPos = Parent.ClientRectangle.Left - Width + m_BorderWidth;
        }
      }
    }

    private void SlidingPanel_MouseHover(object sender, EventArgs e)
    {
    }

    private void timer1_Tick(object sender, EventArgs e)
    {
      ComputeFoldPositions();
      int targetPos = Left;
      int speed = 0;
      if (m_Unfold)
      {
        targetPos = m_UnfoldPos;
        speed = m_UnfoldSpeed;
      }
      else if (m_Fold)
      {
        targetPos = m_FoldPos;
        speed = m_FoldSpeed;
      }
      int dir = Math.Sign(targetPos - Left);
      Left += (int)(dir * Math.Ceiling(Math.Max(1, Math.Abs(targetPos - Left)) * speed * timer1.Interval / 1000.0f));
      if (Math.Abs(Left - targetPos) < 2)
      {
        Left = targetPos;
        if (m_Unfold)
        {
          m_Folded = false;
          if (OnUnfolded != null)
          {
            OnUnfolded(this);
          }
        }
        else if (m_Fold)
        {
          m_Folded = true;
          if (OnFolded != null)
          {
            OnFolded(this);
          }
        }
        m_Unfold = false;
        m_Fold = false;
        timer1.Stop();
      }
      if (OnSliding != null)
      {
        OnSliding(this);
      }
    }

    public void Fold()
    {
      if (!m_Folded)
      {
        m_Unfold = false;
        m_Fold = true;
        timer1.Start();
      }
    }

    public void Unfold()
    {
      if (!Enabled) return;
      if (m_Folded)
      {
        m_Fold = false;
        m_Unfold = true;
        timer1.Start();
      }
    }

    private void SlidingPanel_MouseLeave(object sender, EventArgs e)
    {
    }

    private void SlidingPanel_MouseEnter(object sender, EventArgs e)
    {
    }

    private bool m_Grabbing = false;
    private bool m_Grabbed = false;
    private Point m_GrabbingScreenPos;
    private int m_GrabbingStartWidth;
    private int m_GrabbingStartLeft;

    private void SlidingPanel_MouseDown(object sender, MouseEventArgs e)
    {      
      m_Grabbing = true;
      m_Grabbed = false;
      m_GrabbingScreenPos = PointToScreen(new Point(e.X, e.Y));
      m_GrabbingStartWidth = Width;
      m_GrabbingStartLeft = Left;
    }

    private void SlidingPanel_MouseMove(object sender, MouseEventArgs e)
    {
      if (m_Grabbing && m_Resizable)
      {
        Point screenPos = PointToScreen(new Point(e.X, e.Y));
        if (Math.Abs(m_GrabbingScreenPos.X - screenPos.X) > 5)
        {
          m_Grabbed = true;
          if (!m_Folded && !m_Fold && !m_Unfold)
          {
            if (AnchorSide == Side.Left)
            {
              Width = Math.Max(m_BorderWidth, m_GrabbingStartWidth + screenPos.X - m_GrabbingScreenPos.X);
            }
            else
            {
              Width = Math.Max(m_BorderWidth, m_GrabbingStartWidth - (screenPos.X - m_GrabbingScreenPos.X));
              Left = m_GrabbingStartLeft - Width + m_GrabbingStartWidth;
            }
            if (OnSliding != null)
            {
              OnSliding(this);
            }
            Refresh();
          }
        }
      }
    }

    private void SlidingPanel_MouseUp(object sender, MouseEventArgs e)
    {
      m_Grabbing = false;
      if (m_Grabbed)
      {
        // do nothing to fold
        return;
      }
      if (m_Folded)
      {
        Unfold();
      }
      else if (AnchorSide == Side.Right)
      {
        if (e.X < m_BorderWidth)
        {
          Fold();
        }
      }
      else
      {
        if (e.X > Width - m_BorderWidth)
        {
          Fold();
        }
      }
    }

    private void SlidingPanel_Paint(object sender, PaintEventArgs e)
    {
      int pos = 0;
      if (AnchorSide == Side.Right)
      {
        pos = 0;
      }
      else
      {
        pos = Width - m_BorderWidth;
      }
      Rectangle rect = new Rectangle(pos, 0, m_BorderWidth - 1, Height - 1);
      if (Enabled)
      {
        e.Graphics.FillRectangle(m_BorderBrush, rect);
      }
      else
      {
        e.Graphics.FillRectangle(Brushes.Gray, rect);
      }
      e.Graphics.DrawRectangle(Pens.White, rect);
      Font drawFont = new Font("Arial", 12);
      StringFormat drawFormat = new StringFormat();
      drawFormat.FormatFlags = StringFormatFlags.DirectionVertical;
      e.Graphics.DrawString(m_Title, m_Font, Brushes.White, pos, 0, drawFormat);
    }

    private void SlidingPanel_EnabledChanged(object sender, EventArgs e)
    {
      if (!Enabled)
      {
        Fold();
      }
    }

  }
}
