using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Utilities.DVDImport
{
	public partial class SelectAudio : Form
	{
		private List<IFOParse.AudioFormatDetails> m_audioFormats = null;
		private IFOParse.AudioFormatDetails m_selectedFormat = null;

		public SelectAudio(List<IFOParse.AudioFormatDetails> formats)
		{
			InitializeComponent();
			m_audioFormats = formats;

			ux_List.Items.Clear();
			foreach (IFOParse.AudioFormatDetails format in m_audioFormats)
				ux_List.Items.Add(format);
			ux_List.SelectedIndex = 0;
		}

		private void ux_Cancel_Click(object sender, EventArgs e)
		{
			this.DialogResult = DialogResult.Cancel;
		}

		private void ux_OK_Click(object sender, EventArgs e)
		{
			this.DialogResult = DialogResult.OK;
		}

		private void ux_List_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (ux_List.SelectedIndex != -1)
			{
				ux_OK.Enabled = true;
				m_selectedFormat = (IFOParse.AudioFormatDetails)ux_List.Items[ux_List.SelectedIndex];
			}
			else
				ux_OK.Enabled = false;
		}

		public int SelectedSubstreamID
		{
			get
			{
				if (m_selectedFormat == null)
					return (0);
				else
					return (m_selectedFormat.StreamID);

			}
		}
	}
}