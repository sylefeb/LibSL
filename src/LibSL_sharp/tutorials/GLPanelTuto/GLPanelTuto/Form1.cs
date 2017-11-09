using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using LibSL_sharp;

namespace GLPanelTuto
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        GlPanel m_GLPanel;

        private void Form1_Load(object sender, EventArgs e)
        {
            m_GLPanel = new GlPanel();
            m_GLPanel.Dock = DockStyle.Fill;
            Controls.Add( m_GLPanel );
        }
    }
}
