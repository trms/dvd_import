#define MPLEX
using System;
using System.Drawing;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using System.Text;
using System.Threading;
using System.Diagnostics;
using Microsoft.DirectX.AudioVideoPlayback;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Configuration;

namespace Utilities.DVDImport
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private Thread thread = null;
		private Dictionary<string, FileStream> writers = new Dictionary<string, FileStream>();
		private Dictionary<string, FileStream> readers = new Dictionary<string, FileStream>();
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.ProgressBar progressBar1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button button3;
		private System.Windows.Forms.Button button4;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tabPage1;
		private System.Windows.Forms.TabPage tabPage2;
		private System.Windows.Forms.ComboBox comboBox1;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label volumeLabel;
		private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
		private System.Windows.Forms.Button button5;
		private System.Windows.Forms.SaveFileDialog saveFileDialog1;
		private System.Windows.Forms.TextBox textBox2;
		private System.Windows.Forms.TextBox textBox3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.TextBox textBox4;
		private System.Windows.Forms.Button button6;
		private TreeView treeView1;
		private DSUtils m_ds = null;
		private Button button7;
		private TrackBar trackBar1;
		private Label label7;
		private Label label8;
		private GroupBox groupBox1;
		private CheckBox checkBox2;
		private CheckBox checkBox1;
		private Process process = null;
		private Label label9;
		private Label label10;
		volatile private bool m_run = true;
		private DateTime m_startTime = DateTime.MinValue;
		private Label label11;
		private readonly int m_demuxCount = 400;
		private readonly int m_audioCount = 600;
		private readonly int m_remuxCount = 1000;
		private int m_lastSecond = 0;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			CheckForIllegalCrossThreadCalls = false;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
			this.label1 = new System.Windows.Forms.Label();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.button1 = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.label2 = new System.Windows.Forms.Label();
			this.panel1 = new System.Windows.Forms.Panel();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.checkBox2 = new System.Windows.Forms.CheckBox();
			this.checkBox1 = new System.Windows.Forms.CheckBox();
			this.label7 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.trackBar1 = new System.Windows.Forms.TrackBar();
			this.button3 = new System.Windows.Forms.Button();
			this.button4 = new System.Windows.Forms.Button();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.label10 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this.button5 = new System.Windows.Forms.Button();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.button6 = new System.Windows.Forms.Button();
			this.label3 = new System.Windows.Forms.Label();
			this.comboBox1 = new System.Windows.Forms.ComboBox();
			this.volumeLabel = new System.Windows.Forms.Label();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
			this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
			this.textBox2 = new System.Windows.Forms.TextBox();
			this.textBox3 = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.textBox4 = new System.Windows.Forms.TextBox();
			this.treeView1 = new System.Windows.Forms.TreeView();
			this.button7 = new System.Windows.Forms.Button();
			this.label11 = new System.Windows.Forms.Label();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
			this.groupBox2.SuspendLayout();
			this.tabControl1.SuspendLayout();
			this.tabPage1.SuspendLayout();
			this.tabPage2.SuspendLayout();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(16, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(40, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "Folder:";
			// 
			// textBox1
			// 
			this.textBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.textBox1.Location = new System.Drawing.Point(56, 16);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(360, 20);
			this.textBox1.TabIndex = 1;
			this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
			// 
			// button1
			// 
			this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.button1.Location = new System.Drawing.Point(424, 16);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(24, 23);
			this.button1.TabIndex = 2;
			this.button1.Text = "...";
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// button2
			// 
			this.button2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.button2.Enabled = false;
			this.button2.Location = new System.Drawing.Point(704, 285);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(48, 23);
			this.button2.TabIndex = 3;
			this.button2.Text = "Import";
			this.button2.Click += new System.EventHandler(this.button2_Click);
			// 
			// progressBar1
			// 
			this.progressBar1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.progressBar1.Location = new System.Drawing.Point(0, 311);
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(760, 7);
			this.progressBar1.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
			this.progressBar1.TabIndex = 4;
			this.progressBar1.Visible = false;
			// 
			// label2
			// 
			this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.Location = new System.Drawing.Point(-1, 292);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(424, 15);
			this.label2.TabIndex = 5;
			// 
			// panel1
			// 
			this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.panel1.Location = new System.Drawing.Point(8, 16);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(248, 222);
			this.panel1.TabIndex = 6;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.checkBox2);
			this.groupBox1.Controls.Add(this.checkBox1);
			this.groupBox1.Controls.Add(this.label7);
			this.groupBox1.Controls.Add(this.label8);
			this.groupBox1.Controls.Add(this.trackBar1);
			this.groupBox1.Location = new System.Drawing.Point(313, 96);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(159, 112);
			this.groupBox1.TabIndex = 25;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Audio Processing";
			// 
			// checkBox2
			// 
			this.checkBox2.AutoSize = true;
			this.checkBox2.Location = new System.Drawing.Point(9, 88);
			this.checkBox2.Name = "checkBox2";
			this.checkBox2.Size = new System.Drawing.Size(130, 17);
			this.checkBox2.TabIndex = 28;
			this.checkBox2.Text = "Dynamic Compression";
			this.checkBox2.UseVisualStyleBackColor = true;
			// 
			// checkBox1
			// 
			this.checkBox1.AutoSize = true;
			this.checkBox1.Location = new System.Drawing.Point(8, 65);
			this.checkBox1.Name = "checkBox1";
			this.checkBox1.Size = new System.Drawing.Size(72, 17);
			this.checkBox1.TabIndex = 27;
			this.checkBox1.Text = "Normalize";
			this.checkBox1.UseVisualStyleBackColor = true;
			this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// label7
			// 
			this.label7.Location = new System.Drawing.Point(6, 16);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(39, 19);
			this.label7.TabIndex = 23;
			this.label7.Text = "Level:";
			this.label7.TextAlign = System.Drawing.ContentAlignment.TopRight;
			// 
			// label8
			// 
			this.label8.Location = new System.Drawing.Point(6, 39);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(39, 19);
			this.label8.TabIndex = 24;
			this.label8.TextAlign = System.Drawing.ContentAlignment.TopRight;
			// 
			// trackBar1
			// 
			this.trackBar1.Location = new System.Drawing.Point(51, 16);
			this.trackBar1.Maximum = 20;
			this.trackBar1.Minimum = -20;
			this.trackBar1.Name = "trackBar1";
			this.trackBar1.Size = new System.Drawing.Size(104, 42);
			this.trackBar1.TabIndex = 22;
			this.trackBar1.TickFrequency = 5;
			this.trackBar1.Scroll += new System.EventHandler(this.trackBar1_Scroll);
			// 
			// button3
			// 
			this.button3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.button3.Image = ((System.Drawing.Image)(resources.GetObject("button3.Image")));
			this.button3.Location = new System.Drawing.Point(160, 247);
			this.button3.Name = "button3";
			this.button3.Size = new System.Drawing.Size(32, 23);
			this.button3.TabIndex = 7;
			this.button3.Click += new System.EventHandler(this.button3_Click);
			// 
			// button4
			// 
			this.button4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.button4.Image = ((System.Drawing.Image)(resources.GetObject("button4.Image")));
			this.button4.Location = new System.Drawing.Point(192, 247);
			this.button4.Name = "button4";
			this.button4.Size = new System.Drawing.Size(32, 23);
			this.button4.TabIndex = 8;
			this.button4.Click += new System.EventHandler(this.button4_Click);
			// 
			// groupBox2
			// 
			this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox2.Controls.Add(this.panel1);
			this.groupBox2.Controls.Add(this.label10);
			this.groupBox2.Controls.Add(this.label9);
			this.groupBox2.Controls.Add(this.button3);
			this.groupBox2.Controls.Add(this.button4);
			this.groupBox2.Controls.Add(this.button5);
			this.groupBox2.Enabled = false;
			this.groupBox2.Location = new System.Drawing.Point(488, 8);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(264, 271);
			this.groupBox2.TabIndex = 11;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Preview";
			// 
			// label10
			// 
			this.label10.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label10.AutoSize = true;
			this.label10.Location = new System.Drawing.Point(7, 252);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(74, 13);
			this.label10.TabIndex = 10;
			this.label10.Text = "affect preview";
			this.label10.Visible = false;
			// 
			// label9
			// 
			this.label9.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label9.AutoSize = true;
			this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label9.Location = new System.Drawing.Point(7, 237);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(133, 13);
			this.label9.TabIndex = 9;
			this.label9.Text = "Audio Processing does not";
			this.label9.Visible = false;
			// 
			// button5
			// 
			this.button5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.button5.Image = ((System.Drawing.Image)(resources.GetObject("button5.Image")));
			this.button5.Location = new System.Drawing.Point(224, 247);
			this.button5.Name = "button5";
			this.button5.Size = new System.Drawing.Size(32, 23);
			this.button5.TabIndex = 0;
			this.button5.Click += new System.EventHandler(this.button5_Click);
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.Add(this.tabPage1);
			this.tabControl1.Controls.Add(this.tabPage2);
			this.tabControl1.Location = new System.Drawing.Point(8, 8);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(464, 80);
			this.tabControl1.TabIndex = 12;
			this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
			// 
			// tabPage1
			// 
			this.tabPage1.Controls.Add(this.button6);
			this.tabPage1.Controls.Add(this.label3);
			this.tabPage1.Controls.Add(this.comboBox1);
			this.tabPage1.Controls.Add(this.volumeLabel);
			this.tabPage1.Location = new System.Drawing.Point(4, 22);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.Size = new System.Drawing.Size(456, 54);
			this.tabPage1.TabIndex = 0;
			this.tabPage1.Text = "DVD";
			// 
			// button6
			// 
			this.button6.Location = new System.Drawing.Point(384, 16);
			this.button6.Name = "button6";
			this.button6.Size = new System.Drawing.Size(64, 23);
			this.button6.TabIndex = 14;
			this.button6.Text = "Refresh";
			this.button6.Visible = false;
			this.button6.Click += new System.EventHandler(this.button6_Click);
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(8, 16);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(40, 16);
			this.label3.TabIndex = 1;
			this.label3.Text = "Drive:";
			// 
			// comboBox1
			// 
			this.comboBox1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox1.Location = new System.Drawing.Point(56, 16);
			this.comboBox1.Name = "comboBox1";
			this.comboBox1.Size = new System.Drawing.Size(64, 21);
			this.comboBox1.TabIndex = 0;
			this.comboBox1.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
			// 
			// volumeLabel
			// 
			this.volumeLabel.Location = new System.Drawing.Point(120, 16);
			this.volumeLabel.Name = "volumeLabel";
			this.volumeLabel.Size = new System.Drawing.Size(328, 16);
			this.volumeLabel.TabIndex = 13;
			// 
			// tabPage2
			// 
			this.tabPage2.Controls.Add(this.label1);
			this.tabPage2.Controls.Add(this.button1);
			this.tabPage2.Controls.Add(this.textBox1);
			this.tabPage2.Location = new System.Drawing.Point(4, 22);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.Size = new System.Drawing.Size(456, 54);
			this.tabPage2.TabIndex = 1;
			this.tabPage2.Text = "VOB File";
			// 
			// saveFileDialog1
			// 
			this.saveFileDialog1.DefaultExt = "mpg";
			this.saveFileDialog1.Filter = "MPEG Files|*.mpg";
			this.saveFileDialog1.OverwritePrompt = false;
			// 
			// textBox2
			// 
			this.textBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.textBox2.Location = new System.Drawing.Point(75, 229);
			this.textBox2.Name = "textBox2";
			this.textBox2.Size = new System.Drawing.Size(64, 20);
			this.textBox2.TabIndex = 14;
			this.textBox2.Text = "1";
			// 
			// textBox3
			// 
			this.textBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.textBox3.Location = new System.Drawing.Point(75, 261);
			this.textBox3.Name = "textBox3";
			this.textBox3.Size = new System.Drawing.Size(392, 20);
			this.textBox3.TabIndex = 15;
			// 
			// label4
			// 
			this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label4.Location = new System.Drawing.Point(19, 229);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(48, 23);
			this.label4.TabIndex = 16;
			this.label4.Text = "ShowID:";
			// 
			// label5
			// 
			this.label5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label5.Location = new System.Drawing.Point(19, 261);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(48, 23);
			this.label5.TabIndex = 17;
			this.label5.Text = "Name:";
			// 
			// label6
			// 
			this.label6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label6.Location = new System.Drawing.Point(155, 229);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(40, 23);
			this.label6.TabIndex = 19;
			this.label6.Text = "Reel #:";
			// 
			// textBox4
			// 
			this.textBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.textBox4.Location = new System.Drawing.Point(203, 229);
			this.textBox4.Name = "textBox4";
			this.textBox4.Size = new System.Drawing.Size(64, 20);
			this.textBox4.TabIndex = 18;
			this.textBox4.Text = "1";
			// 
			// treeView1
			// 
			this.treeView1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)));
			this.treeView1.FullRowSelect = true;
			this.treeView1.HideSelection = false;
			this.treeView1.Location = new System.Drawing.Point(16, 96);
			this.treeView1.Name = "treeView1";
			this.treeView1.ShowPlusMinus = false;
			this.treeView1.ShowRootLines = false;
			this.treeView1.Size = new System.Drawing.Size(456, 127);
			this.treeView1.TabIndex = 20;
			this.treeView1.BeforeCollapse += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeView1_BeforeCollapse);
			this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
			// 
			// button7
			// 
			this.button7.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.button7.Location = new System.Drawing.Point(623, 285);
			this.button7.Name = "button7";
			this.button7.Size = new System.Drawing.Size(75, 23);
			this.button7.TabIndex = 21;
			this.button7.Text = "Options...";
			this.button7.UseVisualStyleBackColor = true;
			this.button7.Click += new System.EventHandler(this.button7_Click);
			// 
			// label11
			// 
			this.label11.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.label11.Location = new System.Drawing.Point(429, 293);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(188, 15);
			this.label11.TabIndex = 26;
			this.label11.TextAlign = System.Drawing.ContentAlignment.TopRight;
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(760, 320);
			this.Controls.Add(this.label11);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.button7);
			this.Controls.Add(this.treeView1);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.textBox4);
			this.Controls.Add(this.textBox3);
			this.Controls.Add(this.textBox2);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.tabControl1);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.progressBar1);
			this.Controls.Add(this.button2);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MinimumSize = new System.Drawing.Size(768, 344);
			this.Name = "Form1";
			this.Text = "DVD Import";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.Form1_Closing);
			this.Load += new System.EventHandler(this.Form1_Load);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.tabControl1.ResumeLayout(false);
			this.tabPage1.ResumeLayout(false);
			this.tabPage2.ResumeLayout(false);
			this.tabPage2.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		protected override void WndProc(ref Message m)
		{
			//from dbt.h and winuser.h
			const int WM_DEVICECHANGE = 0x0219;
			//const int DBT_DEVICEARRIVAL = 0x8000; // system detected a new device
			//const int DBT_DEVTYP_VOLUME = 0x00000002; // logical volume

			//we detect the media arrival event
			if (m.Msg == WM_DEVICECHANGE)
			{
				tabControl1.SelectedIndex = 0;
				RefreshDVD();
			}
			base.WndProc(ref m);
		}

		private void button1_Click(object sender, System.EventArgs e)
		{
			if(folderBrowserDialog1.ShowDialog() == DialogResult.OK)
				textBox1.Text = folderBrowserDialog1.SelectedPath;
		}

		// http://www.mpucoder.com/DVD/vobov.html

		const string REGISTRY_KEY  = @"SOFTWARE\TRMS\DVDImport";
		const int BLOCK_START_CODE = 0x000001ba;
		const int BLOCK_CRYPT_MASK = 0x30000000;
		const int VID_DETECT_BYTES = 0x000001e0;
		const int AUD_DETECT_BYTES = 0x000001c0;
		const int AC3_DETECT_BYTES = 0x000001bd;
		const int NAV_DETECT_BYTES = 0x000001bb;
		const int PCI_DETECT_BYTES = 0x000001bf;
		private ByteSwapper bs = null;

		private UInt32 ReadCode(byte[] p, int offset)
		{
			uint x;
			x  = p[offset]; x <<= 8;
			x |= p[offset + 1]; x <<= 8;
			x |= p[offset + 2]; x <<= 8;
			x |= p[offset + 3];
			return(x);
		}

		private UInt16 ReadWord(byte[] p, int offset)
		{
			UInt16 x;
			x  = p[offset]; x <<= 8;
			x |= p[offset + 1];
			return(x);
		}

		private int ReadPTS(byte[] buf, int offset)
		{
			long a1, a2, a3;
			int pts;

			a1 = (buf[offset] & 0xe) >> 1;
			a2 = ((buf[offset + 1] << 8) | buf[offset + 2]) >> 1;
			a3 = ((buf[offset + 3] << 8) | buf[offset + 4]) >> 1;
			pts = (int)(((a1) << 30) | (a2 << 15) | a3);
			return(pts);
		}

		private void SaveData(FileStream fs, byte[] data, int offset, int len, int id)
		{
			//uint x=0xffffffff;
			int skip = 0;

			if(fs.Name.EndsWith(".wav")) // swap bytes in wav data
			{
				if(bs == null)
					bs = new ByteSwapper(fs);
				for(int i = offset + skip; i < offset + skip + len; i++)
					bs.NextByte(data[i]);
			}
			else
				fs.Write(data, offset + skip, len);
		}

		private string m_tempPath = "";
		private string TempPath
		{
			get
			{
				if (m_tempPath == "")
				{
					string tmp = Settings.Default.TempPath;
					if(tmp == "")
						tmp = Environment.GetEnvironmentVariable("TEMP");
					if (tmp == null || tmp == "")
						tmp = "C:\\";
					m_tempPath = tmp + "\\" + Guid.NewGuid().ToString() + "\\";
					Directory.CreateDirectory(m_tempPath);
				}
				if (!Directory.Exists(m_tempPath))
					Directory.CreateDirectory(m_tempPath);
				return (m_tempPath);
			}
		}

		private void ReadSector(List<IFOParse.VOB> vobs, byte[] buf, long sector, int offset)
		{
			// find which vob has our sector
			foreach (IFOParse.VOB vob in vobs)
			{
				if (sector >= vob.FirstSector && sector < vob.LastSector)
				{
					// got it
					FileStream fs = null;
					if (readers.ContainsKey(vob.FileInfo.Name))
						fs = (FileStream)readers[vob.FileInfo.Name];
					else
					{
						fs = File.OpenRead(vob.Filename);
						readers[vob.FileInfo.Name] = fs;
					}
					long s = (sector - vob.FirstSector) * 2048 + offset;
					if (fs.Seek(s, SeekOrigin.Begin) != s)
						throw new IOException("Error reading VOB file");
					if (fs.Read(buf, 0, buf.Length) != buf.Length)
						throw new IOException("Error reading VOB file");
					return;
				}
			}
			throw new IOException("Invalid sector");
		}

		private void ReadSector(List<IFOParse.VOB> vobs, byte[] buf, long sector)
		{
			// find which vob has our sector
			foreach (IFOParse.VOB vob in vobs)
			{
				if (sector >= vob.FirstSector && sector < vob.LastSector)
				{
					// got it
					FileStream fs = null;
					if (readers.ContainsKey(vob.FileInfo.Name))
						fs = (FileStream)readers[vob.FileInfo.Name];
					else
					{
						fs = File.OpenRead(vob.Filename);
						readers[vob.FileInfo.Name] = fs;
					}
					long s = (sector - vob.FirstSector) * 2048;
					if (fs.Seek(s, SeekOrigin.Begin) != s)
						throw new IOException("Error reading VOB file");
					if (fs.Read(buf, 0, 2048) != 2048)
						throw new IOException("Error reading VOB file");
					return;
				}
			}
			throw new IOException("Invalid sector");
		}

		private int Offset(List<IFOParse.Cell> cells, List<IFOParse.VOB> vobs)
		{
			int videoOffset = 0;
			int audioOffset = 0;
			byte[] buf = new byte[2048];
			bool haveAudioOffset = false;
			bool haveVideoOffset = false;

			foreach (IFOParse.Cell cell in cells)
			{
				for (int sector = cell.FirstSector; sector < cell.LastSector; sector++)
				{
					ReadSector(vobs, buf, sector);
					uint code = ReadCode(buf, 0);
					if (code != BLOCK_START_CODE)
						continue;

					ulong systemCode = ReadCode(buf, 0x0e);

					switch (systemCode)
					{
						case AUD_DETECT_BYTES:
						case AC3_DETECT_BYTES:
							if ((buf[0x15] & 0x80) != 0 && haveAudioOffset == false)
							{
								audioOffset = ReadPTS(buf, 0x17);
								haveAudioOffset = true;
							}
							break;
						case NAV_DETECT_BYTES:
							if (haveVideoOffset == false)
							{
								videoOffset = (int)ReadCode(buf, 0x39);
								haveVideoOffset = true;
							}
							break;
						default:
							break;
					}
					if (haveAudioOffset && haveVideoOffset)
						break;
				}
				if (haveAudioOffset && haveVideoOffset)
					break;
			}
			foreach (FileStream fs in readers.Values)
			{
				fs.Close();
			}
			readers.Clear();

			int msOffset = audioOffset - videoOffset;
			if (msOffset < 0)
				msOffset -= 44;
			else
				msOffset += 44;
			msOffset /= 90;
			return (msOffset);
		}

		#region Thread Safe UI calls
		delegate void SetStatusTextCallback(string text);
		private void SetStatusText(string text)
		{
			if (label2.InvokeRequired)
			{
				SetStatusTextCallback d = new SetStatusTextCallback(SetStatusText);
				this.Invoke(d, new object[] { text });
			}
			else
				label2.Text = text;
		}

		private void SetETAText(string text)
		{
			if (label11.InvokeRequired)
			{
				SetStatusTextCallback d = new SetStatusTextCallback(SetETAText);
				this.Invoke(d, new object[] { text });
			}
			else
				label11.Text = text;
		}

		delegate void SetProgressCallback(int progress);
		private void SetProgress(int progress)
		{
			if (progressBar1.InvokeRequired)
			{
				SetProgressCallback d = new SetProgressCallback(SetProgress);
				this.Invoke(d, new object[] { progress });
			}
			else
			{
				progressBar1.Minimum = 0;
				progressBar1.Maximum = m_remuxCount;
				if (progress > 0 && progress < m_remuxCount)
				{
					progressBar1.Value = progress;

					try
					{
						TimeSpan elapsed = DateTime.Now - m_startTime;
						int curSecond = Convert.ToInt32(elapsed.TotalSeconds);
						if (curSecond > 5 && curSecond != m_lastSecond) // don't update more frequently than each second
						{
							m_lastSecond = curSecond;
							double secPerTick = elapsed.TotalSeconds / progress;
							double remainingSec = secPerTick * (m_remuxCount - progress);
							int inputSecs = Convert.ToInt32(remainingSec);
							int seconds = inputSecs % 60;
							int minutes = 0;
							inputSecs = inputSecs - seconds;
							if (inputSecs > 0)
							{
								inputSecs = inputSecs / 60;
								minutes = inputSecs % 60;
							}
							string remaining = "";
							if (minutes > 1)
								remaining = "About " + minutes.ToString() + " minutes";
							else if (minutes == 1)
								remaining = "About a minute";
							else
								remaining = "Less than a minute";
							remaining += " left";
							label11.Text = remaining;
						}
					}
					catch { }
				}
			}
		}

		delegate void NullCallback();
		private void DisableForm()
		{
			if (this.InvokeRequired)
			{
				NullCallback d = new NullCallback(DisableForm);
				this.Invoke(d);
			}
			else
			{
				Cursor = Cursors.WaitCursor;
				progressBar1.Visible = true;
				textBox1.Enabled = false;
				button1.Enabled = false;
				button2.Text = "Cancel";
				button3.Enabled = false;
				groupBox1.Enabled = false;
				groupBox2.Enabled = false;
				treeView1.Enabled = false;
				tabControl1.Enabled = false;
				button7.Enabled = false;
				textBox2.Enabled = false;
				textBox3.Enabled = false;
				textBox4.Enabled = false;
			}
		}

		private void DisableCancel()
		{
			if (this.InvokeRequired)
			{
				NullCallback d = new NullCallback(DisableCancel);
				this.Invoke(d);
			}
			else
			{
				button2.Enabled = false;
			}
		}

		private void EnableForm()
		{
			if (this.InvokeRequired)
			{
				NullCallback d = new NullCallback(EnableForm);
				this.Invoke(d);
			}
			else
			{
				Cursor = Cursors.Default;
				progressBar1.Visible = false;
				textBox1.Enabled = true;
				button1.Enabled = true;
				button2.Text = "Import";
				button2.Enabled = true;
				button3.Enabled = true;
				groupBox1.Enabled = true;
				groupBox2.Enabled = true;
				treeView1.Enabled = true;
				tabControl1.Enabled = true;
				button7.Enabled = true;
				textBox2.Enabled = true;
				textBox3.Enabled = true;
				textBox4.Enabled = true;
			}
		}
		#endregion

		private void ProcessVOB()
		{
			try
			{
				DisableForm();
				List<IFOParse.VOB> vobs = null;
				IFOParse.ProgramChain pgc = null;
				List<IFOParse.Cell> cells = null;
				if (treeView1.SelectedNode.Tag.GetType() == typeof(FileInfo))
				{
					// treat a single vob file with no ifo file as all data
					// got single vob file
					FileInfo fi = (FileInfo)treeView1.SelectedNode.Tag;
					IFOParse.VOB v = new IFOParse.VOB(fi.FullName);
					vobs = new List<IFOParse.VOB>();
					vobs.Add(v);

					// create dummy cell list
					cells = new List<IFOParse.Cell>();
					IFOParse.Cell c = new IFOParse.Cell();
					c.FirstSector = 0;
					c.LastSector = (int)v.LastSector;
					cells.Add(c);
				}
				else if (treeView1.SelectedNode.Tag.GetType() == typeof(IFOParse.ProgramChain))
				{
					pgc = (IFOParse.ProgramChain)treeView1.SelectedNode.Tag;
					vobs = pgc.Title.VOBs;
					cells = pgc.Cells;
				}

				//string outputDir = "";
				#region Choose a file, default to last directory used
				string saveFile = "";
				saveFileDialog1.FileName = textBox2.Text + "-" + textBox4.Text + "-" + textBox3.Text + ".mpg";
				saveFileDialog1.FileName = new Utilities().MakeSafeFilename(saveFileDialog1.FileName);

				RegistryKey rk = Registry.LocalMachine.OpenSubKey(REGISTRY_KEY);
				if (rk != null)
				{
					string initialDir = rk.GetValue("DefaultDirectory").ToString();
					if (initialDir != null && Directory.Exists(initialDir))
						saveFileDialog1.InitialDirectory = initialDir;
				}
				DialogResult dr = saveFileDialog1.ShowDialog();
				if (dr == DialogResult.OK && File.Exists(saveFileDialog1.FileName))
				{
					dr = DialogResult.Cancel;
					// ask about overwriting
					if (MessageBox.Show(saveFileDialog1.FileName + " already exists.\nDo you want to replace it?", "Save As", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.Yes)
					{
						dr = DialogResult.OK;
						File.Delete(saveFileDialog1.FileName);
					}
				}
				if (dr == DialogResult.OK)
				{
					saveFile = saveFileDialog1.FileName;
					FileInfo fi = new FileInfo(saveFile);
					try
					{
						RegistryKey saveKey = Registry.LocalMachine.CreateSubKey(REGISTRY_KEY);
						saveKey.SetValue("DefaultDirectory", fi.Directory.FullName);
						//outputDir = fi.Directory.FullName;
					}
					catch { }
				}
				else
				{
					SetProgress(0);
					SetStatusText("");
					EnableForm();
					thread = null;
					return;
				}
				#endregion

				int audioOffset = 0;
				int videoOffset = 0;
				string audio = null;
				string video = null;

				m_startTime = DateTime.Now;
				m_lastSecond = 0;

				try
				{
					SetProgress(0);
					byte[] buf = new byte[2048];
					long totalSectors = 0;
					long currentSector = 0;
					// find total number of sectors for all cells
					for (int cellNum = 0; cellNum < cells.Count; cellNum++)
					{
						IFOParse.Cell cell = cells[cellNum];
						totalSectors += cell.LastSector - cell.FirstSector;
					}

					SetStatusText("Reading from DVD...");
					for (int cellNum = 0; cellNum < cells.Count; cellNum++)
					{
						IFOParse.Cell cell = cells[cellNum];
						ulong updateInterval = 0;
						for (int sector = cell.FirstSector; sector <= cell.LastSector; sector++)
						{
							ReadSector(vobs, buf, sector);
							if (updateInterval++ % 10 == 0)
							{
								SetStatusText("Reading from DVD..." + Convert.ToInt32(100.0 * (Convert.ToDouble(currentSector) / Convert.ToDouble(totalSectors))) + "%");
								SetProgress(Convert.ToInt32(m_demuxCount * (Convert.ToDouble(currentSector) / Convert.ToDouble(totalSectors))));
							}
							currentSector++;
							uint code = ReadCode(buf, 0);
							if (code != BLOCK_START_CODE)
								continue;

							int i = 0x0e;
							ulong systemCode = ReadCode(buf, i);
							i += 4;
							UInt16 headerLength = ReadWord(buf, i);
							i += 2;

							switch (systemCode)
							{
								case AC3_DETECT_BYTES:
									//if (!inCell)
									//	break;
									//audPacks++;
									#region write non-mpeg audio data to temp file
									UInt16 flags = ReadWord(buf, i);
									i += 2;
									byte b = buf[i++];
									#region find ms offset
									if ((flags & 0xc000) == 0x8000 && (flags & 0xff) >= 0x80 && audioOffset == 0)
									{
										byte c = buf[i++];
										int offset = (c & 0x0e) << 29;
										offset += (ReadWord(buf, i) & 0xfffe) << 14;
										i += 2;
										offset += (ReadWord(buf, i) >> 1) & 0x7fff;
										i += 2;
										offset /= 90;
										i += b - 5;
										audioOffset = offset;
									}
									else
										i += b;
									#endregion
									byte substream = buf[i++];
									i++; // # frame headers
									UInt16 pointer = ReadWord(buf, i);
									i += 2;
									int t = substream;
									string name = t.ToString("000");
									if (t >= 0xA8) // nothing
										continue;
									else if (t >= 0xA0) // PCM
									{
										// http://dvd.sourceforge.net/dvdinfo/index.html
										name += ".wav";
										i++; // emph, mute, reserved, frame #
										byte details = buf[i++];
										i++; // dynamic range;

										// these seem to be zeroed out in my tests, ignore them
										int bitsPerSample = (details & 0xC0) >> 6;
										int sampleRate = (details & 0x30) >> 4;
										int numChannels = details & 0x07;
										b += 3;
									}
									else if (t >= 0x88) // DTS
										name += ".dts"; // dts audio will be ignored (most dvds are ac3, or at least have an ac3 track)
									else if (t >= 0x80) // AC3
										name += ".ac3";
									else
										continue;
									FileStream w = null;
									if (writers.ContainsKey(name))
										w = (FileStream)writers[name];
									else
									{
										w = File.Create(TempPath + name);
										writers[name] = w;
										if (name.EndsWith(".wav")) // leave room for wav header
											w.Seek(44, SeekOrigin.Begin);
									}
									//SaveData(w, buf, i, 2048 - i, t);
									SaveData(w, buf, i, (headerLength - 7 - b), t);
									#endregion
									break;
								case VID_DETECT_BYTES:
									//if (!inCell)
									//	break;
									//vidPacks++;
									#region write mpeg video to temp file
									flags = ReadWord(buf, i);
									i += 2;
									b = buf[i++];
									#region find ms offset
									if ((flags & 0xc000) == 0x8000 && (flags & 0xff) >= 0x80 && videoOffset == 0)
									{
										byte c = buf[i++];
										int offset = (c & 0x0e) << 29;
										offset += (ReadWord(buf, i) & 0xfffe) << 14;
										i += 2;
										offset += (ReadWord(buf, i) >> 1) & 0x7fff;
										i += 2;
										offset /= 90;
										i += b - 5;
										videoOffset = offset;
									}
									else
										i += b;
									#endregion
									string vname = "video.m2v";
									video = vname;
									FileStream vw = null;
									if (writers.ContainsKey(vname))
										vw = (FileStream)writers[vname];
									else
									{
										vw = File.Create(TempPath + vname);
										writers[vname] = vw;
									}
									//SaveData(vw, buf, i, 2048 - i, 1);
									SaveData(vw, buf, i, (headerLength - 3 - b), 1);
									#endregion
									break;
								case AUD_DETECT_BYTES:
									//if (!inCell)
									//	break;
									//audPacks++;
									#region write mpeg audio to temp file
									flags = ReadWord(buf, i);
									i += 2;
									b = buf[i++];
									#region find ms offset
									if ((flags & 0xc000) == 0x8000 && (flags & 0xff) >= 0x80 && audioOffset == 0)
									{
										byte c = buf[i++];
										int offset = (c & 0x0e) << 29;
										offset += (ReadWord(buf, i) & 0xfffe) << 14;
										i += 2;
										offset += (ReadWord(buf, i) >> 1) & 0x7fff;
										i += 2;
										offset /= 90;
										i += b - 5;
										audioOffset = offset;
									}
									else
										i += b;
									#endregion
									string aname = "vob.mp2";
									if (audio == null)
										audio = aname;
									FileStream aw = null;
									if (writers.ContainsKey(aname))
										aw = (FileStream)writers[aname];
									else
									{
										aw = File.Create(TempPath + aname);
										writers[aname] = aw;
									}
									//SaveData(aw, buf, i, 2048 - i, 0);
									SaveData(aw, buf, i, (headerLength - 3 - b), 1);
									#endregion
									break;
								case NAV_DETECT_BYTES:
									//navPacks++;
									#region find vobID and cellID
									int cellID = buf[0x422];
									int vobID = (buf[0x41f] << 8) + buf[0x420];
									//if (cellID == cell.CellID && vobID == cell.VobID)
									//	inCell = true;
									//else
									//	inCell = false;
									#endregion
									break;
								default:
									break;
							}

							if (m_run == false)
								return;
						}
					}
					SetProgress(m_demuxCount);
				}
				finally
				{
					if (bs != null)
						bs.TRMSFinalize();
					bs = null;
					foreach (string name in writers.Keys)
					{
						FileStream fs = (FileStream)writers[name];
						fs.Close();

						if (name.EndsWith(".m2v"))
							video = name;
						else if (name.EndsWith(".ac3") || name.EndsWith(".dts") || name.EndsWith(".wav") || name.EndsWith(".mp2"))
							audio = name;

						// delete anything after the first audio and video streams in the file
						if (audio != null && name != video && name != audio)
							File.Delete(name);
					}
					writers.Clear();
					foreach (FileStream fs in readers.Values)
					{
						fs.Close();
					}
					readers.Clear();
				}

				int duration = 0;
				if (pgc != null)
					duration = pgc.Duration;
				audio = ConvertAudio(TempPath + audio, duration);
				if (audio == null)
					return;
				SetProgress(m_audioCount);

				DisableCancel();
				SetStatusText("Remuxing elementary mpeg streams...");
				MultiplexGlue mg = new MultiplexGlue();
				mg.Video = TempPath + video;
				mg.Audio = audio;
				mg.Offset = Offset(cells, vobs);
				mg.Output = saveFile;
				string mplexLog = TempPath + "mplex.log";
				mg.Log = mplexLog;
				Thread mt = new Thread(new ThreadStart(mg.Multiplex));
				mt.Start();
				while (mt.IsAlive)
				{
					if (pgc != null)
					{
						try
						{
							SetStatusText("Remuxing elementary mpeg streams..." + Convert.ToInt32(100.0 * (Convert.ToDouble(mg.CurrentSCR()) / Convert.ToDouble(pgc.Duration))) + "%");
							SetProgress(m_audioCount + Convert.ToInt32((m_remuxCount - m_audioCount) * (Convert.ToDouble(mg.CurrentSCR()) / Convert.ToDouble(pgc.Duration))));
						}
						catch { }
					}
					else
						SetStatusText("Remuxing elementary mpeg streams...");
					Thread.Sleep(500);
				}
				//mg.Multiplex(TempPath + video, audio, Offset(cells, vobs), saveFile);
				SetProgress(m_remuxCount);
				if (File.Exists(mplexLog))
				{
					StreamReader sr = File.OpenText(mplexLog);
					string line = "";
					while((line = sr.ReadLine()) != null)
					{
						if (line.StartsWith("MUX STATUS: ") || line.StartsWith("Average bit-rate"))
						{
							line = line.Replace("MUX STATUS: ", "");
							SetStatusText("Remuxing elementary mpeg streams... " + line);
							Thread.Sleep(500);
						}
					}
					sr.Close();

					File.Delete(mplexLog);
				}
			}
			catch (ThreadAbortException)
			{
				try
				{
					if (process != null && process.HasExited == false)
						process.Kill();
				}
				catch { }
			}
			catch (Exception ex)
			{
				MessageBox.Show("Error: " + ex.Message);
			}
			finally
			{
				if (process != null && process.HasExited == false)
					process.Kill();
				EnableForm();
				thread = null;
				SetProgress(0);
				//SetStatusText("");
				SetETAText("");

				try
				{
					Directory.Delete(TempPath, true);
				}
				catch { }
			}
		}

		private string ConvertAudio(string filename, int duration)
		{
			if (filename.ToLower().EndsWith(".wav"))
			{
				SetStatusText("Writing LPCM wave header...");
				#region Write out .wav file header
				FileInfo fi = new FileInfo(filename);
				FileStream w = File.OpenWrite(filename);
				byte[] hdr = new byte[44];
				// ChunkID
				hdr[0] = (byte)'R';
				hdr[1] = (byte)'I';
				hdr[2] = (byte)'F';
				hdr[3] = (byte)'F';

				// ChunkSize
				long size = fi.Length - 8;
				hdr[7] = (byte)(size & 0xFF000000 >> 24);
				hdr[6] = (byte)(size & 0xFF0000 >> 16);
				hdr[5] = (byte)(size & 0xFF00 >> 8);
				hdr[4] = (byte)(size & 0xFF);

				// Format
				hdr[8] = (byte)'W';
				hdr[9] = (byte)'A';
				hdr[10] = (byte)'V';
				hdr[11] = (byte)'E';

				// Subchunk1ID
				hdr[12] = (byte)'f';
				hdr[13] = (byte)'m';
				hdr[14] = (byte)'t';
				hdr[15] = (byte)' ';

				// Subchunk1Size
				hdr[19] = 0;
				hdr[18] = 0;
				hdr[17] = 0;
				hdr[16] = 16; // size of format subchunk, 16 = PCM

				// AudioFormat
				hdr[21] = 0;
				hdr[20] = 1; // 1 = PCM

				// NumChannels
				hdr[23] = 0;
				hdr[22] = 2; // Stereo

				// SampleRate
				hdr[27] = 0;
				hdr[26] = 0;
				hdr[25] = 0xBB;
				hdr[24] = 0x80; // 48kHz

				// ByteRate
				hdr[31] = 0;
				hdr[30] = 0x02;
				hdr[29] = 0xEE;
				hdr[28] = 0; // == SampleRate * NumChannels * BitsPerSample/8

				// BlockAlign
				hdr[33] = 0;
				hdr[32] = 4; // == NumChannels * BitsPerSample/8

				// BitsPerSample
				hdr[35] = 0;
				hdr[34] = 16;

				// Subchunk2ID
				hdr[36] = (byte)'d';
				hdr[37] = (byte)'a';
				hdr[38] = (byte)'t';
				hdr[39] = (byte)'a';

				// Subchunk2Size
				size = fi.Length - hdr.Length;
				hdr[43] = (byte)(size & 0xFF000000 >> 24);
				hdr[42] = (byte)(size & 0xFF0000 >> 16);
				hdr[41] = (byte)(size & 0xFF00 >> 8);
				hdr[40] = (byte)(size & 0xFF);

				w.Seek(0, SeekOrigin.Begin); // write header at beginning of file
				w.Write(hdr, 0, hdr.Length);
				w.Close();
				#endregion
			}
			
			if (Settings.Default.AudioMode)
				return (ConvertAudioNew(filename, duration));
			else
				return (ConvertAudioOld(filename));
		}

		private string ConvertAudioNew(string filename, int duration)
		{
			string output = filename;
			FileInfo appPath = new FileInfo(Application.ExecutablePath);
			int idx = output.LastIndexOf('.');
			if (idx != -1)
				output = output.Substring(0, idx);
			output += ".mp2";
			
			#region encode audio
			SetStatusText("Encoding mpeg audio...");
			string newAudio = output.Replace(".wav", ".mp2");
			string besweetCommand = appPath.Directory + "\\BeSweet\\BeSweet.exe";
			if (File.Exists(besweetCommand) == false)
				return (null);
			process = new Process();
			process.StartInfo.UseShellExecute = false;
			process.StartInfo.WorkingDirectory = Environment.CurrentDirectory;
			//process.StartInfo.RedirectStandardOutput = true;
			process.StartInfo.RedirectStandardError = true;
			process.StartInfo.FileName = besweetCommand;
			process.StartInfo.Arguments = "-core( -input \"" + filename + "\" -output \"" + output + "\" ) -toolame( -m j -b 192 )";
			string dynamicCompression = "";
			if (checkBox2.Checked)
				dynamicCompression += " -c normal";
			if (checkBox1.Checked)
				process.StartInfo.Arguments += " -azid( -s stereo " + dynamicCompression + "-L -3db --maximize )";
			else
				process.StartInfo.Arguments += " -azid( -s stereo " + dynamicCompression + "-L -3db -g " + trackBar1.Value + "db )";
			process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
			process.StartInfo.CreateNoWindow = true;
			process.Start();
			string line;
			bool success = false;
			StreamReader sr = process.StandardError;
			while ((line = sr.ReadLine()) != null)
			{
				if (m_run == false)
					return (null);
				if (line == "Conversion Completed !")
					success = true;
				else if (line.EndsWith("transcoding ..."))
				{
					line = line.TrimStart('[');
					idx = line.IndexOf(']');
					if (idx != -1)
						line = line.Substring(0, idx - 1);
					idx = line.LastIndexOf(':');
					if (idx != -1)
						line = line.Substring(0, idx);
					// convert time into seconds to get a percentage
					int len = 0;
					string[] cols = line.Split(':');
					if (cols.Length == 3)
						len = (Convert.ToInt32(cols[0]) * 3600) + (Convert.ToInt32(cols[1]) * 60) + Convert.ToInt32(cols[2]);
					if (len != 0 && duration != 0)
					{
						SetStatusText("Encoding mpeg audio..." + Convert.ToInt32(100.0 * (Convert.ToDouble(len) / Convert.ToDouble(duration))) + "%");
						SetProgress(m_demuxCount + Convert.ToInt32((m_audioCount - m_demuxCount) * (Convert.ToDouble(len) / Convert.ToDouble(duration))));
					}
				}
			}
			process.WaitForExit();
			process = null;
			try
			{
				File.Delete(filename);
			}
			catch { }
			#endregion

			return (output);
		}

		private string ConvertAudioOld(string filename)
		{
			string output = filename;
			FileInfo appPath = new FileInfo(Application.ExecutablePath);

			if (filename.ToLower().EndsWith(".ac3"))
			{
				SetStatusText("Decoding AC3 audio...");
				output = output.Replace(".ac3", ".wav");
				#region decode ac3
				try
				{
					File.Delete(output);
				}
				catch { }
				string command = appPath.Directory + "\\ac3dec.exe";
				if (File.Exists(command) == false)
					return (null);
				process = new Process();
				process.StartInfo.UseShellExecute = false;
				process.StartInfo.WorkingDirectory = Environment.CurrentDirectory;
				process.StartInfo.RedirectStandardOutput = true;
				//process.StartInfo.RedirectStandardError = true;
				process.StartInfo.FileName = command;
				process.StartInfo.Arguments = "\"" + filename + "\" -pcmwav \"" + output + "\"";
				process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
				process.StartInfo.CreateNoWindow = true;
				process.Start();
				string line;
				bool success = false;
				StreamReader sr = process.StandardOutput;
				while ((line = sr.ReadLine()) != null)
				{
					if (m_run == false)
						return (null);
					if (line == "Decoding complete.")
						success = true;
				}
				process.WaitForExit();
				process = null;
				#endregion
				File.Delete(filename);
				if(success == false)
				{
					try
					{
						File.Delete(output);
					}
					catch { }
					return (null);
				}
			}
			#region encode audio to mpeg if it isn't
			if (output.EndsWith(".wav"))
			{
				SetStatusText("Encoding mpeg audio...");
				string newAudio = output.Replace(".wav", ".mp2");
				string lameCommand = appPath.Directory + "\\tooLame.exe";
				if (File.Exists(lameCommand) == false)
					return (null);
				process = new Process();
				process.StartInfo.UseShellExecute = false;
				process.StartInfo.WorkingDirectory = Environment.CurrentDirectory;
				//process.StartInfo.RedirectStandardOutput = true;
				process.StartInfo.RedirectStandardError = true;
				process.StartInfo.FileName = lameCommand;
				string extraOptions = " -g";
				if (System.Configuration.ConfigurationManager.AppSettings.GetValues("MPEGEncodeOptions") != null)
					extraOptions = " " + System.Configuration.ConfigurationManager.AppSettings.GetValues("MPEGEncodeOptions")[0];
				process.StartInfo.Arguments = "-s 48" + extraOptions + " \"" + output + "\" \"" + newAudio + "\"";
				process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
				process.StartInfo.CreateNoWindow = true;
				process.Start();
				string line;
				bool success = false;
				StreamReader sr = process.StandardError;
				while ((line = sr.ReadLine()) != null)
				{
					if (m_run == false)
						return (null);
					if (line == "Done")
						success = true;
				}
				process.WaitForExit();
				process = null;
				if (success)
				{
					File.Delete(output);
					output = newAudio;
				}
				else
				{
					try
					{
						File.Delete(output);
					}
					catch { }
					try
					{
						File.Delete(newAudio);
					}
					catch { }
					return (null);
				}
			}
			#endregion
			return (output);
		}

		private void button2_Click(object sender, System.EventArgs e)
		{
			if (thread == null)
			{
				if (treeView1.SelectedNode == null)
					return;
				m_run = true;
				thread = new Thread(new ThreadStart(ProcessVOB));
				thread.Start();
			}
			else
				m_run = false;
		}

		private void Form1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			Settings.Default.AudioLevel = trackBar1.Value;
			Settings.Default.Normalize = checkBox1.Checked;
			Settings.Default.DynamicCompress = checkBox2.Checked;
			Settings.Default.Save();

			if (m_ds != null)
				m_ds.Stop();
			
			if(thread != null)
			{
				thread.Abort();
				thread = null;
			}
		}

		[DllImport("kernel32.dll")]
		private static extern int GetDriveType(string driveLetter);
		[DllImport("kernel32.dll")]
		private static extern int GetVolumeInformation(string PathName, StringBuilder VolumeNameBuffer, UInt32 VolumeNameSize, ref UInt32 VolumeSerialNumber, ref UInt32 MaximumComponentLength, ref UInt32 FileSystemFlags, StringBuilder FileSystemNameBuffer, UInt32 FileSystemNameSize);
		[DllImport("coredll.dll", EntryPoint="GetLastError")]
		public static extern int GetLastError();

		private void Form1_Load(object sender, System.EventArgs e)
		{
			FileInfo appPath = new FileInfo(Application.ExecutablePath);
			string ac3decCommand = appPath.Directory + "\\ac3dec.exe";
			string lameCommand = appPath.Directory + "\\tooLame.exe";
			string besweetCommand = appPath.Directory + "\\BeSweet\\BeSweet.exe";
			if (File.Exists(ac3decCommand) && File.Exists(lameCommand) && File.Exists(besweetCommand))
			{
				// okay
			}
			else
			{
				MessageBox.Show("Check installation, helper utilities are not installed properly", "Installation Error", MessageBoxButtons.OK);
				Application.Exit();
			}

			System.Version v = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
			string versionText = v.ToString(3) + " Build " + v.Revision;
			this.Text += " " + versionText;
			if (Settings.Default.IsFirstRun == true)
			{
				Settings.Default.Upgrade();
				Settings.Default.IsFirstRun = false;
				Settings.Default.Save();
			}

			trackBar1.Value = Settings.Default.AudioLevel;
			checkBox1.Checked = Settings.Default.Normalize;
			checkBox2.Checked = Settings.Default.DynamicCompress;
			SetupControls();

			button3.Enabled = false;
			button4.Enabled = false;
			button5.Enabled = false;

			comboBox1.Items.Clear();
			string[] drives = Environment.GetLogicalDrives();
			foreach(string drive in drives)
			{
				if(GetDriveType(drive) == 5)
					comboBox1.Items.Add(drive);
			}
			comboBox1.SelectedIndex = 0;
		}

		private void RefreshDVD()
		{
			if(comboBox1.Items.Count == 0)
				return;
			string drive = comboBox1.Items[comboBox1.SelectedIndex].ToString();
			//DSUtils ds = new DSUtils();
			//ulong id = ds.GetDVDDiscID(drive);
			uint serNum = 0;
			uint maxCompLen = 0;
			StringBuilder volLabel = new StringBuilder(256); // Label
			UInt32 volFlags = new UInt32();
			StringBuilder fsName = new StringBuilder(256);
			volumeLabel.Text = "";
			int result = GetVolumeInformation(drive, volLabel, (UInt32)volLabel.Capacity, ref serNum, ref maxCompLen, ref volFlags, fsName, (UInt32)fsName.Capacity);
			if(result != 0)
			{
				volumeLabel.Text = volLabel.ToString();
				LoadTitles();
			}
			else
				treeView1.Nodes.Clear();
		}

		private void comboBox1_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			RefreshDVD();
		}

		private void textBox1_TextChanged(object sender, System.EventArgs e)
		{
			LoadTitles();
		}

		private void LoadTitles()
		{
			try
			{
				treeView1.Nodes.Clear();
				button3.Enabled = false;
				button4.Enabled = false;
				button5.Enabled = false;
				string path = "";
				if (tabControl1.SelectedIndex == 0)
				{
					string drive = comboBox1.Items[comboBox1.SelectedIndex].ToString();
					path = drive + "VIDEO_TS";
				}
				else
					path = textBox1.Text;
				if (!Directory.Exists(path))
					return;

				int index = 0;
				int maxLength = 0;
				TreeNode longestNode = null;
				foreach (string file in Directory.GetFiles(path, "*.IFO"))
				{
					FileInfo fi = new FileInfo(file);
					string name = fi.Name.ToUpper();
					if (name == "VIDEO_TS.IFO")
						continue;
					try
					{
						int title = Convert.ToInt32(name.Replace("VTS_", "").Replace("_0.IFO", ""));
						IFOParse ifo = new IFOParse(file);

						// add to tree view
						TreeNode titleNode = new TreeNode();
						titleNode.Text = "Title " + title + ": " + ifo.VideoMode + " video, " + ifo.AudioFormat + " audio";
						titleNode.Tag = ifo;
						titleNode.Nodes.Clear();
						titleNode.ToolTipText = ifo.ProgramChainCount + " programs";
						for (int pgcNum = 0; pgcNum < ifo.ProgramChainCount; pgcNum++)
						{
							IFOParse.ProgramChain pgc = ifo.ProgramChains[pgcNum];
							TreeNode n = new TreeNode();
							n.Text = "PGC " + (pgcNum + 1) + ": " + Utilities.SecondsToLength(pgc.Duration);
							n.ToolTipText = pgc.Cells.Count + " cells";
							n.Tag = pgc;
							titleNode.Nodes.Add(n);
							if (pgc.Duration > maxLength)
							{
								longestNode = n;
								maxLength = pgc.Duration;
							}
						}
						treeView1.Nodes.Add(titleNode);
					}
					catch (Exception ex)
					{
						MessageBox.Show(ex.Message);
					}
				}
				if (treeView1.Nodes.Count == 0)
				{
					// found no IFO files, load raw VOB list
					foreach (string file in Directory.GetFiles(path, "*.VOB"))
					{
						FileInfo fi = new FileInfo(file);
						string name = fi.Name.ToUpper();

						TreeNode titleNode = new TreeNode();

						List<IFOParse.VOB> vobs = new List<IFOParse.VOB>();
						IFOParse.VOB v = new IFOParse.VOB(file);
						vobs = new List<IFOParse.VOB>();
						vobs.Add(v);

						// create dummy cell list
						List<IFOParse.Cell> cells = new List<IFOParse.Cell>();
						IFOParse.Cell c = new IFOParse.Cell();
						c.FirstSector = 0;
						c.LastSector = (int)v.LastSector;
						cells.Add(c);

						titleNode.Text = name + ": " + Utilities.BytesToSize(fi.Length);
						titleNode.Tag = fi;
						treeView1.Nodes.Add(titleNode);
					}
				}
				if (longestNode != null)
					treeView1.SelectedNode = longestNode;
				treeView1.ExpandAll();
			}
			catch (Exception ex)
			{
				MessageBox.Show("An error occurred reading DVD, it may be corrupt or a failed burn.\n\n" + ex.Message);
			}
		}

		#region Video Playback
		private Thread m_videoThread = null;
		private void PlaybackProgress()
		{
			lock (this)
			{
				if (m_ds == null)
					return;
			}

			while(true)
			{
				lock (this)
				{
					if (m_ds.IsPlaying() == false)
						break;
				}
				Thread.Sleep(250);
			}

			lock (this)
			{
				button3.Enabled = true;
				button4.Enabled = false;
				button5.Enabled = false;
				m_ds.Stop();
				label9.Visible = false;
				label10.Visible = false;
				m_ds = null;
				m_videoThread = null;
			}
		}

		private void button3_Click(object sender, System.EventArgs e)
		{
			lock (this)
			{
				List<IFOParse.VOB> vobs = null;
				IFOParse.ProgramChain pgc = null;
				List<IFOParse.Cell> cells = null;
				if (treeView1.SelectedNode == null)
					return;
				if (treeView1.SelectedNode.Tag.GetType() == typeof(FileInfo))
				{
					// got single vob file
					FileInfo fi = (FileInfo)treeView1.SelectedNode.Tag;
					IFOParse.VOB v = new IFOParse.VOB(fi.FullName);
					vobs = new List<IFOParse.VOB>();
					vobs.Add(v);

					// create dummy cell list
					cells = new List<IFOParse.Cell>();
					IFOParse.Cell c = new IFOParse.Cell();
					c.FirstSector = 0;
					c.LastSector = (int)v.LastSector;
					cells.Add(c);
				}
				else if (treeView1.SelectedNode.Tag.GetType() == typeof(IFOParse.ProgramChain))
				{
					pgc = (IFOParse.ProgramChain)treeView1.SelectedNode.Tag;
					vobs = pgc.Title.VOBs;
					cells = pgc.Cells;
				}


				m_ds = new DSUtils();
				ArrayList ranges = new ArrayList();
				ArrayList vobNames = new ArrayList();
				for (int c = 0; c < cells.Count; c++)
				{
					ranges.Add(cells[c].FirstSector);
					ranges.Add(cells[c].LastSector);
				}
				for (int v = 0; v < vobs.Count; v++)
				{
					vobNames.Add(vobs[v].FileInfo.FullName);
					vobNames.Add(vobs[v].Sectors);
				}
				m_ds.Preview(ranges, vobNames, panel1.Handle, trackBar1.Value);
				button3.Enabled = false;
				button4.Enabled = true;
				button5.Enabled = true;
				if (Settings.Default.AudioMode)
				{
					label9.Visible = true;
					label10.Visible = true;
				}

				m_videoThread = new Thread(new ThreadStart(PlaybackProgress));
				m_videoThread.Start();
			}
		}

		private void button4_Click(object sender, System.EventArgs e)
		{
			lock (this)
			{
				if (m_ds == null)
					return;
				m_ds.Stop();
				label9.Visible = false;
				label10.Visible = false;
				button3.Enabled = true;
				button4.Enabled = false;
				button5.Enabled = false;
			}
		}

		private void button5_Click(object sender, System.EventArgs e)
		{
			lock (this)
			{
				if (m_ds == null)
					return;
				m_ds.Pause();
			}
		}
		#endregion

		private void button6_Click(object sender, System.EventArgs e)
		{
			RefreshDVD();
		}

		private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
		{
			if (e.Node.Nodes.Count > 0)
			{
				button2.Enabled = false;
				groupBox2.Enabled = false;
				button3.Enabled = false;
				button4.Enabled = false;
				button5.Enabled = false;
				textBox3.Text = "";
			}
			else
			{
				if(m_ds == null)
					button3.Enabled = true;
				button2.Enabled = true;
				groupBox2.Enabled = true;

				if (e.Node.Parent == null)
				{
					// got a single vob
					string val = e.Node.Text;
					int idx = val.IndexOfAny(":.".ToCharArray());
					if (idx != -1)
						val = val.Substring(0, idx);
					textBox3.Text = "";
					if (tabControl1.SelectedIndex == 0)
						textBox3.Text = volumeLabel.Text + ", ";
					textBox3.Text += val;
				}
				else
				{
					string val = e.Node.Parent.Text;
					int idx = val.IndexOfAny(":.".ToCharArray());
					if (idx != -1)
						val = val.Substring(0, idx);
					val += " - " + e.Node.Text;
					idx = val.IndexOfAny(":.".ToCharArray());
					if (idx != -1)
						val = val.Substring(0, idx);
					val = val.Replace("PGC", "Program");
					textBox3.Text = "";
					if (tabControl1.SelectedIndex == 0)
						textBox3.Text = volumeLabel.Text + ", ";
					textBox3.Text += val;
				}
			}
				
		}

		private void treeView1_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
		{
			// don't let the user double click to hide items
			e.Cancel = true;
		}

		private void SetupControls()
		{
			groupBox1.Visible = Settings.Default.AudioMode;
			label8.Text = trackBar1.Value + "dB";
			if (Settings.Default.AudioMode)
				treeView1.Width = tabControl1.Width - groupBox1.Width - 10;
			else
				treeView1.Width = tabControl1.Width;
			trackBar1.Enabled = !checkBox1.Checked;
			label7.Enabled = !checkBox1.Checked;
			label8.Enabled = !checkBox1.Checked;

		}

		private void button7_Click(object sender, EventArgs e)
		{
			this.Enabled = false;
			Options o = new Options();
			o.ShowDialog();
			this.Enabled = true;
			SetupControls();
		}

		private void trackBar1_Scroll(object sender, EventArgs e)
		{
			label8.Text = trackBar1.Value + "dB";
		}

		private void checkBox1_CheckedChanged(object sender, EventArgs e)
		{
			trackBar1.Value = 0;
			label8.Text = trackBar1.Value + "dB";
			trackBar1.Enabled = !checkBox1.Checked;
			label7.Enabled = !checkBox1.Checked;
			label8.Enabled = !checkBox1.Checked;
		}

		private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (tabControl1.SelectedIndex == 1)
				LoadTitles();
			else
				RefreshDVD();
		}
	}
}
