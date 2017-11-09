// SL: 2010, Cecill-C as part of LibSL

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace LibSL_sharp
{
    public partial class GlPanel : UserControl
    {
        [DllImport(@"LibSL_sharp_binder.dll")]
        public static extern void GLPanelInit(System.IntPtr hwnd);
        [DllImport(@"LibSL_sharp_binder.dll")]
        public static extern void GLPaint(System.IntPtr hwnd, Int32 w, Int32 h);
        [DllImport(@"LibSL_sharp_binder.dll")]
        public static extern void GLReshape(System.IntPtr hwnd, Int32 w, Int32 h);
        [DllImport(@"LibSL_sharp_binder.dll")]
        public static extern void GLMouseMotion(System.IntPtr hwnd, Int32 x, Int32 y, Int32 w, Int32 h);
        [DllImport(@"LibSL_sharp_binder.dll")]
        public static extern void GLMouseButtonPressed(System.IntPtr hwnd, Int32 x, Int32 y, Int32 w, Int32 h, Int32 btn, Int32 flags);

        private bool m_Initialized = false;

        public GlPanel()
        {
            InitializeComponent();
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.ResizeRedraw, true);
            SetStyle(ControlStyles.Opaque, true);
            GLPanelInit((IntPtr)this.Handle);
            m_Initialized = true;
        }

        private void GlPanel_Load(object sender, EventArgs e)
        {
            if (!m_Initialized) return;
            GLReshape((IntPtr)this.Handle, Width, Height);
        }

        private void GlPanel_Paint(object sender, PaintEventArgs e)
        {
            if (!m_Initialized) return;
            GLPaint((IntPtr)this.Handle, Width, Height);
        }

        private void timerRefresh_Tick(object sender, EventArgs e)
        {
            if (!m_Initialized) return;
            Invalidate();
        }

        private void GlPanel_Resize(object sender, EventArgs e)
        {
            if (!m_Initialized) return;
            GLReshape((IntPtr)this.Handle, Width, Height);
        }

        private void GlPanel_MouseMove(object sender, MouseEventArgs e)
        {
            if (!m_Initialized) return;
            GLMouseMotion((IntPtr)this.Handle, e.X, e.Y, Width, Height);
        }

        private void GlPanel_MouseClick(object sender, MouseEventArgs e)
        {
            if (!m_Initialized) return;

        }

        private void GlPanel_MouseDown(object sender, MouseEventArgs e)
        {
            if (!m_Initialized) return;
            int btn = 0;
            switch (e.Button)
            {
                case MouseButtons.Left: btn = 1; break;
                case MouseButtons.Middle: btn = 2; break;
                case MouseButtons.Right: btn = 3; break;
            }
            GLMouseButtonPressed((IntPtr)this.Handle, e.X, e.Y, Width, Height, btn, 1);
        }

        private void GlPanel_MouseUp(object sender, MouseEventArgs e)
        {
            if (!m_Initialized) return;
            int btn = 0;
            switch (e.Button)
            {
                case MouseButtons.Left: btn = 1; break;
                case MouseButtons.Middle: btn = 2; break;
                case MouseButtons.Right: btn = 3; break;
            }
            GLMouseButtonPressed((IntPtr)this.Handle, e.X, e.Y, Width, Height, btn, 2);
        }

        private void GlPanel_ParentChanged(object sender, EventArgs e)
        {
        }

    }
}
