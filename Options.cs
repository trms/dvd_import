using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Configuration;
using System.Management;

namespace Utilities.DVDImport
{
	public partial class Options : Form
	{
		public Options()
		{
			InitializeComponent();
		}

		private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
		{
			if(Directory.Exists(linkLabel1.Text))
				folderBrowserDialog1.SelectedPath = linkLabel1.Text;
			if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
			{
				linkLabel1.Text = folderBrowserDialog1.SelectedPath;
				UpdateFreeSpace();
			}
		}

		private void button1_Click(object sender, EventArgs e)
		{
			// save
			Settings.Default.AudioMode = radioButton2.Checked;
			Settings.Default.TempPath = linkLabel1.Text;
			Settings.Default.Save();

			this.Close();
		}

		private void button2_Click(object sender, EventArgs e)
		{
			this.Close();
		}

		private void Options_Load(object sender, EventArgs e)
		{
			FileInfo appPath = new FileInfo(Application.ExecutablePath);
			string ac3decCommand = appPath.Directory + "\\ac3dec.exe";
			string lameCommand = appPath.Directory + "\\tooLame.exe";
			string besweetCommand = appPath.Directory + "\\BeSweet\\BeSweet.exe";
			if (File.Exists(ac3decCommand) && File.Exists(lameCommand) && File.Exists(besweetCommand))
			{
				radioButton1.Enabled = true;
				radioButton2.Enabled = true;
				if (Settings.Default.AudioMode == true)
				{
					radioButton1.Checked = false;
					radioButton2.Checked = true;
				}
				else
				{
					radioButton1.Checked = true;
					radioButton2.Checked = false;
				}
			}
			else
			{
				MessageBox.Show("Check installation, helper utilities are not installed properly", "Installation Error", MessageBoxButtons.OK);
				radioButton1.Enabled = false;
				radioButton2.Enabled = false;
				radioButton1.Checked = false;
				radioButton2.Checked = false;
			}
			string tempPath = Settings.Default.TempPath;
			if (tempPath == null || tempPath == "")
			{
				string tmp = Environment.GetEnvironmentVariable("TEMP");
				if (tmp == null || tmp == "")
					tmp = "C:\\";
				DirectoryInfo di = new DirectoryInfo(tmp);
				tempPath = di.FullName;
			}
			linkLabel1.Text = tempPath;
			UpdateFreeSpace();
		}

		private void UpdateFreeSpace()
		{
			string path = linkLabel1.Text;
			try
			{
				#region QueryObjects
				ManagementScope ms1 = new ManagementScope("root\\cimv2");
				ObjectQuery oq1 = new ObjectQuery("SELECT * FROM Win32_LogicalDisk");
				ManagementObjectSearcher query1 = new ManagementObjectSearcher(ms1, oq1);
				ManagementObjectCollection queryCollection1 = query1.Get();
				#endregion
				foreach (ManagementObject mo1 in queryCollection1)
				{
					string letter = mo1["Name"].ToString();
					if (letter != null && path.ToUpper().StartsWith(letter.ToUpper()))
					{
						//Convert.ToUInt64(mo1["Size"]);
						long freeSpace = Convert.ToInt64(mo1["FreeSpace"]);
						label3.Text = Utilities.BytesToSize(freeSpace) + " available";
						break;
					}
				}
			}
			catch
			{
				label3.Text = "";
			}
		}
	}
}