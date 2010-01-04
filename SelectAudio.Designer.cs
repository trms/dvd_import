namespace Utilities.DVDImport
{
	partial class SelectAudio
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

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.ux_List = new System.Windows.Forms.ListBox();
			this.ux_OK = new System.Windows.Forms.Button();
			this.ux_Cancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// ux_List
			// 
			this.ux_List.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.ux_List.FormattingEnabled = true;
			this.ux_List.Location = new System.Drawing.Point(12, 12);
			this.ux_List.Name = "ux_List";
			this.ux_List.Size = new System.Drawing.Size(200, 108);
			this.ux_List.TabIndex = 0;
			this.ux_List.SelectedIndexChanged += new System.EventHandler(this.ux_List_SelectedIndexChanged);
			// 
			// ux_OK
			// 
			this.ux_OK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.ux_OK.Location = new System.Drawing.Point(137, 136);
			this.ux_OK.Name = "ux_OK";
			this.ux_OK.Size = new System.Drawing.Size(75, 23);
			this.ux_OK.TabIndex = 1;
			this.ux_OK.Text = "OK";
			this.ux_OK.UseVisualStyleBackColor = true;
			this.ux_OK.Click += new System.EventHandler(this.ux_OK_Click);
			// 
			// ux_Cancel
			// 
			this.ux_Cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.ux_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.ux_Cancel.Location = new System.Drawing.Point(56, 136);
			this.ux_Cancel.Name = "ux_Cancel";
			this.ux_Cancel.Size = new System.Drawing.Size(75, 23);
			this.ux_Cancel.TabIndex = 2;
			this.ux_Cancel.Text = "Cancel";
			this.ux_Cancel.UseVisualStyleBackColor = true;
			this.ux_Cancel.Click += new System.EventHandler(this.ux_Cancel_Click);
			// 
			// SelectAudio
			// 
			this.AcceptButton = this.ux_OK;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.ux_Cancel;
			this.ClientSize = new System.Drawing.Size(224, 171);
			this.ControlBox = false;
			this.Controls.Add(this.ux_Cancel);
			this.Controls.Add(this.ux_OK);
			this.Controls.Add(this.ux_List);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "SelectAudio";
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Select Audio Track...";
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ListBox ux_List;
		private System.Windows.Forms.Button ux_OK;
		private System.Windows.Forms.Button ux_Cancel;
	}
}