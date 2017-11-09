namespace LibSL_sharp
{
  partial class GlPanel
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Component Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
        this.components = new System.ComponentModel.Container();
        this.timerRefresh = new System.Windows.Forms.Timer(this.components);
        this.SuspendLayout();
        // 
        // timerRefresh
        // 
        this.timerRefresh.Enabled = true;
        this.timerRefresh.Interval = 17;
        this.timerRefresh.Tick += new System.EventHandler(this.timerRefresh_Tick);
        // 
        // GlPanel
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.Name = "GlPanel";
        this.Size = new System.Drawing.Size(468, 358);
        this.Load += new System.EventHandler(this.GlPanel_Load);
        this.Paint += new System.Windows.Forms.PaintEventHandler(this.GlPanel_Paint);
        this.MouseClick += new System.Windows.Forms.MouseEventHandler(this.GlPanel_MouseClick);
        this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.GlPanel_MouseDown);
        this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.GlPanel_MouseMove);
        this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.GlPanel_MouseUp);
        this.Resize += new System.EventHandler(this.GlPanel_Resize);
        this.ParentChanged += new System.EventHandler(this.GlPanel_ParentChanged);
        this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Timer timerRefresh;
  }
}
