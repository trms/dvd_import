namespace Utilities.DVDImport.CPUID
{
	using System;
	using System.Runtime.InteropServices;
	using System.Text;

    abstract class cpuid
	{
		// CPUID, EAX=1, Returned in ECX
		public const int CPU_FEATURE_SSE3_FLAG = 0x1;
		public const int CPU_FEATURE_MON_FLAG = 0x8;
		public const int CPU_FEATURE_DSCPL_FLAG = 0x10;
		public const int CPU_FEATURE_VMX_FLAG = 0x20;
		public const int CPU_FEATURE_EST_FLAG = 0x80;
		public const int CPU_FEATURE_TM2_FLAG = 0x100;
		public const int CPU_FEATURE_CID_FLAG = 0x400;
		public const int CPU_FEATURE_CX16_FLAG = 0x1000;
		public const int CPU_FEATURE_ETPRD_FLAG = 0x2000;

		// CPUID, EAX=1, Returned in EDX
		public const int CPU_FEATURE_FPU_FLAG = 0x1;
		public const int CPU_FEATURE_VME_FLAG = 0x2;
		public const int CPU_FEATURE_DE_FLAG = 0x4;
		public const int CPU_FEATURE_PSE_FLAG = 0x8;
		public const int CPU_FEATURE_TSE_FLAG = 0x10;
		public const int CPU_FEATURE_MSR_FLAG = 0x20;
		public const int CPU_FEATURE_PAE_FLAG = 0x40;
		public const int CPU_FEATURE_MCE_FLAG = 0x80;
		public const int CPU_FEATURE_CMPXCHG8B_FLAG = 0x100;
		public const int CPU_FEATURE_APIC_FLAG = 0x200;
		public const int CPU_FEATURE_SYSENTER_FLAG = 0x800;
		public const int CPU_FEATURE_MTRR_FLAG = 0x1000;
		public const int CPU_FEATURE_PGE_FLAG = 0x2000;
		public const int CPU_FEATURE_MCA_FLAG = 0x4000;
		public const int CPU_FEATURE_CMOV_FLAG = 0x8000;
		public const int CPU_FEATURE_PAT_FLAG = 0x10000;
		public const int CPU_FEATURE_PSE36_FLAG = 0x20000;
		public const int CPU_FEATURE_PSN_FLAG = 0x40000;
		public const int CPU_FEATURE_CLFL_FLAG = 0x80000;
		public const int CPU_FEATURE_DTES_FLAG = 0x200000;
		public const int CPU_FEATURE_ACPI_FLAG = 0x400000;
		public const int CPU_FEATURE_MMX_FLAG = 0x800000;
		public const int CPU_FEATURE_FXSR_FLAG = 0x1000000;
		public const int CPU_FEATURE_SSE_FLAG = 0x2000000;
		public const int CPU_FEATURE_SSE2_FLAG = 0x4000000;
		public const int CPU_FEATURE_SS_FLAG = 0x8000000;
		public const int CPU_FEATURE_HTT_FLAG = 0x10000000;
		public const int CPU_FEATURE_TM1_FLAG = 0x20000000;
		public const int CPU_FEATURE_IA64_FLAG = 0x40000000;
		public const uint CPU_FEATURE_PBE_FLAG = 0x80000000;

		public const int CPU_STEPPING_FLAG = 0xF;
		public const int CPU_TYPE_FLAG = 0xF00;
		public const int CPU_FAMILY_FLAG = 0x780;
		public const int CPU_EXTENDED_FAMILY_FLAG = 0xFF00000;
		public const int CPU_MODEL_FLAG = 0xF0;
		public const int CPU_EXTENDED_MODEL_FLAG = 0xF0000;
		public const int CPU_BRAND_ID_FLAG = 0xFF;
		public const int CPU_CLFLUSH_FLAG = 0xFF00;
		public const int CPU_COUNT_FLAG = 0xFF0000;
		public const uint CPU_APIC_FLAG = 0xFF008000;

		public const string VendorIntel = "GenuineIntel";
		public const string VendorUMC = "UMC UMC UMC ";
		public const string VendorAMD = "AuthenticAMD";
		public const string VendorCyrix = "CyrixInstead";
		public const string VendorNextGen = "NexGenDriven";
		public const string VendorCentaur = "CentaurHauls";
		public const string VendorRise = "RiseRiseRise";
		public const string VendorSiS = "SiS SiS SiS ";
		public const string VendorTransmeta = "GenuineTMx86";
		public const string VendorNSC = "Geode by NSC";

		[DllImport("cpuid.dll")] public static extern bool CPUIDIsSupported();
		[DllImport("cpuid.dll")] private unsafe static extern bool __cpuid(uint level, int* eax, int* ebx, int* ecx, int* edx);

		public unsafe static bool Invoke(uint level, out int eax, out int ebx, out int ecx, out int edx)
		{
			int __eax = 0;
			int __ebx = 0;
			int __ecx = 0;
			int __edx = 0;

			if (__cpuid(level, &__eax, &__ebx, &__ecx, &__edx))
			{
				eax = __eax;
				ebx = __ebx;
				ecx = __ecx;
				edx = __edx;

				return true;
			}
			else
			{
				eax = 0;
				ebx = 0;
				ecx = 0;
				edx = 0;

				return false;
			}
		}

		public static string dword2str(int dword) 
		{
			int dxl = (dword & 0xff);
			int dxh = (dword & 0xff00) >> 8;
			int dlx = (dword & 0xff0000) >> 16;
			int dhx = (int)((uint)(dword & 0xff000000) >> 24);

			return 
				((char)dxl).ToString() + 
				((char)dxh).ToString() +
				((char)dlx).ToString() +
				((char)dhx).ToString();
		}
	}
}