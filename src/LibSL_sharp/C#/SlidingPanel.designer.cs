namespace LibSL_sharp
{
    partial class SlidingPanel
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
      this.timer1 = new System.Windows.Forms.Timer(this.components);
      this.SuspendLayout();
      // 
      // timer1
      // 
      this.timer1.Interval = 17;
      this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
      // 
      // SlidingPanel
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Name = "SlidingPanel";
      this.Size = new System.Drawing.Size(377, 569);
      this.EnabledChanged += new System.EventHandler(this.SlidingPanel_EnabledChanged);
      this.Paint += new System.Windows.Forms.PaintEventHandler(this.SlidingPanel_Paint);
      this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.SlidingPanel_MouseDown);
      this.MouseEnter += new System.EventHandler(this.SlidingPanel_MouseEnter);
      this.MouseLeave += new System.EventHandler(this.SlidingPanel_MouseLeave);
      this.MouseHover += new System.EventHandler(this.SlidingPanel_MouseHover);
      this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.SlidingPanel_MouseMove);
      this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.SlidingPanel_MouseUp);
      this.ParentChanged += new System.EventHandler(this.SlidingPanel_ParentChanged);
      this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Timer timer1;
    }
}
