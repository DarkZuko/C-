#include "GUI.h"
#include "Injector.h"
#include "AllSetting.h"
#include "FilterForm.h"
#include "Functions.h"
#include "Global.h"

using namespace ZSTrainer;
using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace std;

[STAThread]
int Main() 
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew GUI);
	return 0;
}

static LPCTSTR StringToLPCTSTR ( System::String ^X)
{
	marshal_context context;
	return context.marshal_as<const TCHAR*>(X);
}

BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == lParam)
	{
		MapleHWND = hwnd;
		return FALSE;
	}
	return TRUE;
}

void zLog(System::String^ msg) 
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	int Hour = Convert::ToInt32(systime.wHour);
	if (Hour > 12) {
		Hour -= 12;
		GUI::Zuko->Log->Text += "[" + Hour + ":" + systime.wMinute + ":" + systime.wSecond + " PM] ";
	}
	else
		GUI::Zuko->Log->Text += "[" + Hour + ":" + systime.wMinute + ":" + systime.wSecond + " AM] ";
	GUI::Zuko->Log->Text += msg + "\r\n";
	GUI::Zuko->Log->SelectionStart = GUI::Zuko->Log->Text->Length;
	GUI::Zuko->Log->ScrollToCaret();
}

bool StartupHacks()
{
	if (typePIC && fPic)
		Msg(_PIC);

	return true;
}

bool CreateDefault(LPCSTR Path)
{
	WritePrivateProfileString("Main", "Dir", "C:\\Nexon\\MapleStory\\MapleStory.exe", Path);
	WritePrivateProfileString("Main", "Name", "Default", Path);
	WritePrivateProfileString("Main", "Delay", "30", Path);
	WritePrivateProfileString("Main", "Embeded", "TRUE", Path);
	WritePrivateProfileString("Main", "Hidden", "FALSE", Path);
	WritePrivateProfileString("Info", "ID", "", Path);
	WritePrivateProfileString("Info", "PW", "", Path);
	WritePrivateProfileString("Info", "PIC", "", Path);
	WritePrivateProfileString("Info", "Char", "1", Path);  
	WritePrivateProfileString("Info", "Server", "", Path);
	WritePrivateProfileString("Info", "Channel", "3", Path); 
	WritePrivateProfileString("Startups", "AutoLogin", "FALSE", Path);
	WritePrivateProfileString("Startups", "Tubi", "FALSE", Path);
	WritePrivateProfileString("Startups", "Pic", "FALSE", Path);
	WritePrivateProfileString("Startups", "LogoSkip", "TRUE", Path);
	return true;
}

bool CreateInfo(LPCSTR Path)
{
	WritePrivateProfileString("Main", "Profile", "", Path);

	WritePrivateProfileString("Char", "Hp", "", Path);
	WritePrivateProfileString("Char", "MaxHp", "", Path);
	WritePrivateProfileString("Char", "Mp", "", Path);
	WritePrivateProfileString("Char", "MaxMp", "", Path);
	WritePrivateProfileString("Char", "Exp", "", Path);
	WritePrivateProfileString("Char", "MaxExp", "", Path);
	WritePrivateProfileString("Char", "X", "", Path);
	WritePrivateProfileString("Char", "Y", "", Path);
	WritePrivateProfileString("Char", "Breath", "", Path);
	WritePrivateProfileString("Char", "Attack", "", Path);

	WritePrivateProfileString("Mouse", "Animation", "", Path);
	WritePrivateProfileString("Mouse", "X", "", Path);
	WritePrivateProfileString("Mouse", "Y", "", Path);

	WritePrivateProfileString("Server", "World", "", Path);
	WritePrivateProfileString("Server", "Channel", "", Path);

	WritePrivateProfileString("Mob", "Count", "", Path);

	WritePrivateProfileString("People", "Count", "", Path);

	WritePrivateProfileString("Map", "ID", "", Path);

	WritePrivateProfileString("Item", "Count", "", Path);
	WritePrivateProfileString("Item", "X", "", Path);
	WritePrivateProfileString("Item", "Y", "", Path);

	/*WritePrivateProfileString("Wall", "Left", "", Path);
	WritePrivateProfileString("Wall", "Right", "", Path);
	WritePrivateProfileString("Wall", "Top", "", Path);
	WritePrivateProfileString("Wall", "Bottom", "", Path);*/

	WritePrivateProfileString("Mini", "X", "", Path);
	WritePrivateProfileString("Mini", "Y", "", Path);

	WritePrivateProfileString("Rope", "Count", "", Path);
	return true;
}

bool LoadProfile(LPCTSTR Path) //TODO: Optional Profile name? (doesnt have to be Info.ZS)
{
	System::String ^Dir = Directory::GetCurrentDirectory();
	Dir = Dir + "\\Info.ZS";

	SetWindowPos(Gui, HWND_TOP,10,10,475,751, SWP_FRAMECHANGED);
	// Get Maple Directory
	GetPrivateProfileString("Main", "Dir", "NULL", MapleDirBuf, sizeof(MapleDirBuf), Path);
	MapleDir = MapleDirBuf;

	System::String ^DirDir = LPCSTRToString(MapleDir.substr(0, MapleDir.size() - 17).c_str());
	DirDir = DirDir + "\\Path.ZS";

	if (!File::Exists(DirDir))
	WritePrivateProfileString("Main", "Path", StringToLPCSTR(Directory::GetCurrentDirectory()), StringToLPCSTR(DirDir));
	
	GetPrivateProfileString("Main", "Name", "No Name", Name, sizeof(Name), Path);
	ProfName = Name;
	WritePrivateProfileString("Main", "Profile", ProfName, StringToLPCSTR(Dir));

	GetPrivateProfileString("Main", "Delay", "60", WaitTimeBuf, sizeof(WaitTimeBuf), Path);
	WaitTime = (int)atof(WaitTimeBuf);

	GetPrivateProfileString("Info", "ID", "", IDBuf, sizeof(IDBuf), Path);
	ID = IDBuf;
	if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Wizet\\MapleStory", &key) == ERROR_SUCCESS || RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Wow6432Node\\MapleStory", &key) == ERROR_SUCCESS)
	{
		DWORD iVal = 0x1;
		RegSetValueEx(key, "RMA", 0, REG_DWORD, (CONST BYTE*)&iVal, sizeof(iVal)); //Sets ID to be Saved
		RegSetValueEx(key, "LMA", 0, REG_SZ,(unsigned char*)IDBuf, sizeof(IDBuf)); //Sets Current Login ID
		RegCloseKey(key);
	}

	GetPrivateProfileString("Info", "PW", "", PWBuf, sizeof(PWBuf), Path);
	PW = PWBuf;

	GetPrivateProfileString("Info", "PIC", "", PICBuf, sizeof(PICBuf), Path);
	PIC = PICBuf;

	GetPrivateProfileString("Info", "Server", "", ServBuf, sizeof(ServBuf), Path);
	cServer = ServBuf;
	

	if (strncmp(cServer, "Scania", 6) == 0)
		wNeeded = 0;
	if (strncmp(cServer, "Bera", 4) == 0)
		wNeeded = 1;
	if (strncmp(cServer, "Broa", 4) == 0)
		wNeeded = 2;
	if (strncmp(cServer, "Windia", 6) == 0)
		wNeeded = 3;
	if (strncmp(cServer, "Khaini", 6) == 0)
		wNeeded = 4;
	if (strncmp(cServer, "Bellocan", 8) == 0)
		wNeeded = 5;
	if (strncmp(cServer, "Mardia", 6) == 0)
		wNeeded = 6;
	if (strncmp(cServer, "Kradia", 6) == 0)
		wNeeded = 7;
	if (strncmp(cServer, "Yellonde", 8) == 0)
		wNeeded = 8;
	if (strncmp(cServer, "Demethos", 8) == 0)
		wNeeded = 9;
	if (strncmp(cServer, "Galicia", 7) == 0)
		wNeeded = 10;
	if (strncmp(cServer, "ElNido", 7) == 0)
		wNeeded = 11;
	if (strncmp(cServer, "Zenith", 6) == 0)
		wNeeded = 12;
	if (strncmp(cServer, "Arcania", 7) == 0)
		wNeeded = 13;
	if (strncmp(cServer, "Chaos", 5) == 0)
		wNeeded = 14;
	if (strncmp(cServer, "Nova", 4) == 0)
		wNeeded = 15;
	if (strncmp(cServer, "Renegades", 9) == 0)
		wNeeded = 16;

	GetPrivateProfileString("Info", "Channel", "", sChannelBuf, sizeof(sChannelBuf), Path);
	sChannel = atoi(sChannelBuf);

	GetPrivateProfileString("Info", "Char", "", sCharBuf, sizeof(sCharBuf), Path);
	sChar = atoi(sCharBuf);

	GetPrivateProfileString("Main", "Embeded", "FALSE", EmbededBuffer, sizeof(EmbededBuffer), Path);
	sEmbeded = EmbededBuffer;
	if (sEmbeded == "TRUE")
	{
		Embeded = true;
		GUI::Zuko->EmbedCB->Checked = true;
	}
	else
	{
		Embeded = false;
		GUI::Zuko->EmbedCB->Checked = false;
	}

	GetPrivateProfileString("Main", "Hidden", "FALSE", HiddenBuffer, sizeof(HiddenBuffer), Path);
	sHidden = HiddenBuffer;

	if (sHidden == "TRUE")
		MapleHidden = true;
	else
		MapleHidden = false;


	GetPrivateProfileString("Startups", "AutoLogin", "FALSE", AutoLoginBuffer, sizeof(AutoLoginBuffer), Path);
	sAutoLogin = AutoLoginBuffer;
	if (sAutoLogin == "TRUE")
		GUI::Zuko->LoginCB->Checked = true;
	else
		GUI::Zuko->LoginCB->Checked = false;

	/////////////////////////////////////Startup Hacks/////////////////////////////////////////////////
	//Tubi
	GetPrivateProfileString("Startups", "Tubi", "FALSE", TubiBuffer, sizeof(TubiBuffer), Path);
	sTubi = TubiBuffer;
	if (sTubi == "TRUE")
		TubiOn = true;
	else
		TubiOn = false;

	//PIC
	GetPrivateProfileString("Startups", "Pic", "FALSE", PicBuffer, sizeof(PicBuffer), Path);
	sPic = PicBuffer;
	if (sPic == "TRUE")
		typePIC = true;
	else
		typePIC = false;

	//Logo Skip
	GetPrivateProfileString("Startups", "LogoSkip", "FALSE", LogoSkipBuffer, sizeof(LogoSkipBuffer), Path);
	sLogoSkip = LogoSkipBuffer;
	if (sLogoSkip == "TRUE")
		LogoSkipOn = true;
	else
		LogoSkipOn = false;

	/////////////////////////////////////Hacks/////////////////////////////////////////////////
	return true;
}

bool ServerCheck()
{
	#ifdef SERVER_CHECK
		MapleServer::hOpen = InternetOpen("UN/1.0",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
		MapleServer::hFile = InternetOpenUrl(MapleServer::hOpen,"http://www.nexon.net/json/game_status.js",NULL,0,0,0);
		InternetReadFile(MapleServer::hFile,MapleServer::buffer,1024,&MapleServer::dwRead);
		InternetCloseHandle(MapleServer::hOpen);
		InternetCloseHandle(MapleServer::hFile);
		
		if (strstr(MapleServer::buffer,"SVG007\":false") && strstr(MapleServer::buffer,"SVG011\":true"))
			return false;
		else
			return true;
	#endif
	return true;
}

void Reset()
{
	TerminateProcess(MapleHandle, 0);
	MapleOpen = false;
	Updated = false;
	LoggedIn = false;
	Starting = false;
	//CipherBypassed = NULL;
	//HSBypassed = NULL;
	
	SetWindowPos(Gui, HWND_TOP,10,10,475,751, SWP_FRAMECHANGED);
	GUI::Zuko->Status->Text = "MapleStory disconnected!";
	GUI::Zuko->ProgressBar->Value = 0;

	//Clear All Labels
	GUI::Zuko->topnamelabel->Text = "";
	GUI::Zuko->islandlabel->Text = "";
	GUI::Zuko->maplabel->Text = "";
	GUI::Zuko->itemlabel->Text = "Items:";
	GUI::Zuko->moblabel->Text = "Mobs:";
	GUI::Zuko->peoplelabel->Text = "People:";
	GUI::Zuko->hplabel->Text = "HP:";
	GUI::Zuko->mplabel->Text = "MP:";
	GUI::Zuko->explabel->Text = "EXP:";
	GUI::Zuko->attacklabel->Text = "Attacks:";
	GUI::Zuko->breathlabel->Text = "Breath:";
	GUI::Zuko->locationlabel->Text = "Location:";
	GUI::Zuko->mouselabel->Text = "Mouse:";
	GUI::Zuko->worldlabel->Text = "Server:";
	GUI::Zuko->npclabel->Text = "NPC's:";
	GUI::Zuko->channellabel->Text = "Channel:";
}

void MSCheck()
{
	for(;;)
	{
		if (MapleOpen)
		{
			//What if we have multiple instances running?
			//TODO: Below conditions could use improving.
				if (isProcessRunning(MapleHandle) == 0)
				{
					Reset();
					Sleep(5000);

					if (GetProcessHandle("eTracer.aes") != NULL)
					{
						TerminateProcess(GetProcessHandle("eTracer.aes"), 0);
					}
				}
			}
		Sleep(500);
	}
}

void GUI::GUI_Load(System::Object^  sender, System::EventArgs^  e)
{
	GUI::Zuko->Text = LPCSTRToString(GuiTitle);
	Gui = (HWND)Zuko->Handle.ToPointer();//= FindWindow(NULL, "::ZSTrainer::");
	SetActiveWindow(Gui);
	GUI::Zuko->Log->SelectionStart = GUI::Zuko->Log->Text->Length;
	GUI::Zuko->Log->ScrollToCaret();
	//Set CheckBoxes
	if (Embeded)
		EmbedCB->Checked = true;
	else
		EmbedCB->Checked = false;

	if (MapleHidden)
		HideCB->Checked = true;
	else
		HideCB->Checked = false;

	// Server Check //
	if (ServerCheck())
	{
		::MessageBox(Gui, "MapleStory servers detected offline!\nCheck the MapleStory website for more information...\nhttp://maplestory.nexon.net/", Title, MB_OK);
		GUI::Zuko->Status->Text = "MapleStory Servers Detected Offline";
		MapleServer::Online = false;
	}
	else
		MapleServer::Online = true;

	// Get Trainer Directory //
	System::String ^FoundDir = Directory::GetCurrentDirectory();
	FoundDir = FoundDir + "\\Default.ZS";

	System::String ^InfoDir = Directory::GetCurrentDirectory();
	InfoDir = InfoDir + "\\Info.ZS";

	if (!File::Exists(FoundDir))
	CreateDefault(StringToLPCSTR(FoundDir));

	if (!File::Exists(InfoDir))
	CreateInfo(StringToLPCSTR(InfoDir));

	// Set Directory After File's Existance is Positive
	CurrentDir = StringToLPCSTR(FoundDir);

	/*LoadProfile Default*/
	if (LoadProfile(CurrentDir))
	// Check Dll //
	ProfileLoaded = true; // After All is Done Confirm Profile is Loaded //

	#ifdef DLL_CHECK
		System::String ^ZSDir = Directory::GetCurrentDirectory();
		ZSDir = ZSDir + "\\ZS.dll";

		System::String ^MSDir = Directory::GetCurrentDirectory();
		MSDir = MSDir + "\\MS.dll";

		System::String ^MapDir = Directory::GetCurrentDirectory();
		MapDir = MapDir + "\\gms.map";


		if (!File::Exists(ZSDir))
		{
			::MessageBox(Gui, "Could not find ZS.dll in your folder, Please reinstall ZSTrainer.", Title, MB_OK);
			ExitProcess(0);
		}

		if (!File::Exists(MSDir))
		{
			::MessageBox(Gui, "Could not find MS.dll in your folder, Please reinstall ZSTrainer.", Title, MB_OK);
			ExitProcess(0);
		}

		if (!File::Exists(MapDir))
		{
			::MessageBox(Gui, "Could not find GMS.map in your folder, Please reinstall ZSTrainer.", Title, MB_OK);
			ExitProcess(0);
		}


	if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Wizet\\MapleStory", &key) == ERROR_SUCCESS || RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Wow6432Node\\MapleStory", &key) == ERROR_SUCCESS)
	{
		DWORD iVal = 0x13;
		RegSetValueEx(key, "soHPFlash", 0, REG_DWORD, (CONST BYTE*)&iVal, sizeof(iVal)); 
		RegSetValueEx(key, "soMPFlash", 0, REG_DWORD, (CONST BYTE*)&iVal, sizeof(iVal));
		RegCloseKey(key);
	}

	#endif
}

void GUI::GUI_Closing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e )
{
	if (MapleOpen || Starting)
	{
		//TODO: Maybe make MS not included? (like return to normal play, detach dll, etc..)
		if (::MessageBox(Gui, "Are you sure you want to close ZSTrainer? \nThis will also close MapleStory!", Title, MB_YESNO) == IDYES)
		{
			TerminateProcess(MapleHandle, 0);
			ExitProcess(0);
		}
		else
		{
			e->Cancel = true;
		}
	}
}

void GUI::InjectButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	Msg(_TEST);
	//Injector().ShowDialog();
}

void GUI::LoadButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (BrowseProfile->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		CurrentDir = StringToLPCSTR(BrowseProfile->FileName);

		if (LoadProfile(CurrentDir))
		{
			//GUI::Text = LPCSTRToString(GuiTitle) + LPCSTRToString(ProfName); 

			/*Set Check Boxes*/
			if (Embeded)
				GUI::EmbedCB->Checked = true;
			else
				GUI::EmbedCB->Checked = false;

			if (MapleHidden)
				GUI::HideCB->Checked = true;
			else
				GUI::HideCB->Checked = false;
		}
	}
}

void GUI::FilterButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	FilterForm().ShowDialog();
}

void GUI::SaveButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (SaveProfile->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		// File Settings //
		sPath = StringToLPCSTR(SaveProfile->FileName);
		WritePrivateProfileString("Main", "Dir", MapleDir.c_str(), sPath);
		WritePrivateProfileString("Main", "Name", ProfName, sPath);
		WritePrivateProfileString("Main", "Delay", StringToLPCSTR(WaitTime.ToString()), sPath);

		if (Embeded)
			WritePrivateProfileString("Main", "Embeded", "TRUE", sPath);
		else
			WritePrivateProfileString("Main", "Embeded", "FALSE", sPath);

		if (MapleHidden)
			WritePrivateProfileString("Main", "Hidden", "TRUE", sPath);
		else
			WritePrivateProfileString("Main", "Hidden", "FALSE", sPath);

		WritePrivateProfileString("Info", "ID", ID, sPath);
		WritePrivateProfileString("Info", "PW", PW, sPath);
		WritePrivateProfileString("Info", "PIC", PIC, sPath);
		WritePrivateProfileString("Info", "Char", StringToLPCSTR(sChar.ToString()), sPath);
		WritePrivateProfileString("Info", "Server", cServer, sPath);
		WritePrivateProfileString("Info", "Channel", StringToLPCSTR(sChannel.ToString()), sPath);

		if (GUI::Zuko->LoginCB->Checked)
			WritePrivateProfileString("Startups", "AutoLogin", "TRUE",sPath);  
		else
			WritePrivateProfileString("Startups", "AutoLogin", "FALSE",sPath);  

		if (typePIC)
			WritePrivateProfileString("Startups", "Pic", "TRUE", sPath);  
		else
			WritePrivateProfileString("Startups", "Pic", "FALSE", sPath);  
		////////////////////////////////////Hack Settings////////////////////////////////////////////
		/*Tubi*/
		if (TubiOn)
			WritePrivateProfileString("Startups", "Tubi", "TRUE",sPath);  
		else
			WritePrivateProfileString("Startups", "Tubi", "FALSE",sPath);

		if (LogoSkipOn)
			WritePrivateProfileString("Startups", "Logo", "TRUE", sPath); 
		else
			WritePrivateProfileString("Startups", "Logo", "FALSE", sPath);

		LoadProfile(sPath); // Not Neccecary But Can Be Useful later

		::MessageBox(Gui, "Profile Saved!", Title, MB_OK);
	}
}

void ReadPointers()
{
	System::String ^Dir = Directory::GetCurrentDirectory();
	Dir = Dir + "\\Info.ZS";

	System::String ^Map = Directory::GetCurrentDirectory();
	Map = Map + "\\gms.map";

	//Since If MS DC's, this closes and would have to restart when MS opens. Upon DC we should chagne MapleUpon correctly.
	while (MapleOpen)
	{
		if (LoggedIn)
		{
			if (fCharBase)
			{
				if (fCharX && fCharY || fMiniBase && fMiniX) //MiniBase / MiniX arent used here.
				{
					GetPrivateProfileString("Char", "X", "", XBuf, sizeof(XBuf), StringToLPCSTR(Dir));
					CharX = atoi(XBuf);

					GetPrivateProfileString("Char", "Y", "", YBuf, sizeof(YBuf), StringToLPCSTR(Dir));
					CharY = atoi(YBuf);

					if (CharX.ToString()->Length < 5 && CharY.ToString()->Length < 5)
						GUI::Zuko->locationlabel->Text = "Location: " + CharX.ToString() + "," + CharY.ToString();
				}
	
				if (fBreath)
				{
					GetPrivateProfileString("Char", "Breath", "", BreathBuf, sizeof(BreathBuf), StringToLPCSTR(Dir));
					Breath = atoi(BreathBuf);

					GUI::Zuko->breathlabel->Text = "Breath: " + Breath.ToString();
				}
				if (fAttack)
				{
					GetPrivateProfileString("Char", "Attack", "", AttackBuf, sizeof(AttackBuf), StringToLPCSTR(Dir));
					Attacks = atoi(AttackBuf);

					GUI::Zuko->attacklabel->Text = "Attacks: " + Attacks.ToString();
				}
			}

			if (fMobBase)
			{
				if (fMobOffset)
				{
					GetPrivateProfileString("Mob", "Count", "", MobBuf, sizeof(MobBuf), StringToLPCSTR(Dir));
					Mobs = atoi(MobBuf);
					GUI::Zuko->moblabel->Text = "Mobs: " + Mobs.ToString();
				}
			}

			if (fStatBase)
			{
				GetPrivateProfileString("Char", "Hp", "", HPBuf, sizeof(HPBuf), StringToLPCSTR(Dir));
				HP = atoi(HPBuf);
				GetPrivateProfileString("Char", "MaxHp", "", MaxHPBuf, sizeof(MaxHPBuf), StringToLPCSTR(Dir));
				MaxHP = atoi(MaxHPBuf);

				if (MaxHP > 0 && MaxHP < 999999)
				{
					if (!GUI::Zuko->HpBar->Visible)
						GUI::Zuko->HpBar->Visible = true;

					GUI::Zuko->hplabel->Text = "HP: " + HP.ToString() + "/" + MaxHP.ToString();
					GUI::Zuko->HpBar->Width = ((double)HP / MaxHP) * 133;
				}
				else 
				{
					GUI::Zuko->hplabel->Text = "HP: " + HP.ToString();
					GUI::Zuko->HpBar->Visible = false;
				}

				GetPrivateProfileString("Char", "Mp", "", MPBuf, sizeof(MPBuf), StringToLPCSTR(Dir));
				MP = atoi(MPBuf);
				GetPrivateProfileString("Char", "MaxMp", "", MaxMPBuf, sizeof(MaxMPBuf), StringToLPCSTR(Dir));
				MaxMP = atoi(MaxMPBuf);

				if (MaxMP > 0 && MaxMP < 999999)
				{
					if (!GUI::Zuko->MpBar->Visible)
						GUI::Zuko->MpBar->Visible = true;

					GUI::Zuko->mplabel->Text = "MP: " + MP.ToString() + "/" + MaxMP.ToString();
					GUI::Zuko->MpBar->Width = ((double)MP / MaxMP) * 133;
				}
				else
				{
					GUI::Zuko->mplabel->Text = "MP: " + MP.ToString();
					GUI::Zuko->MpBar->Visible = false;
				}

				GetPrivateProfileString("Char", "Exp", "", EXPBuf, sizeof(EXPBuf), StringToLPCSTR(Dir));
				EXP = atof(EXPBuf);

				if (EXP >= 0)
				{
					GUI::Zuko->explabel->Text = "EXP: " + EXP.ToString("f2") + "%";
					GUI::Zuko->ExpBar->Width = (EXP / 100)* 133;
				}
			}

			if (fPeopleBase && fPeopleOffset)
			{
				GetPrivateProfileString("People", "Count", "", PeopleBuf, sizeof(PeopleBuf), StringToLPCSTR(Dir));
				PeopleCount = atoi(PeopleBuf);
				GUI::Zuko->peoplelabel->Text = "People: " + PeopleCount.ToString();
			}

			if (fItemBase && fItemOffset)
			{
				GetPrivateProfileString("Item", "Count", "", ItemBuf, sizeof(ItemBuf), StringToLPCSTR(Dir));
				Items = atoi(ItemBuf);
				GUI::Zuko->itemlabel->Text = "Items: " + Items.ToString();
			}
			
			if (fItemHook)
			{
				GetPrivateProfileString("Item", "X", "", ItemXBuf, sizeof(ItemXBuf), StringToLPCSTR(Dir));
				ItemX = atoi(ItemXBuf);

				GetPrivateProfileString("Item", "Y", "", ItemYBuf, sizeof(ItemYBuf), StringToLPCSTR(Dir));
				ItemY = atoi(ItemYBuf);
			}
		}
		// Pointers to Read even when not logged in

		if (fMapBase && fMapOffset)
		{
			GetPrivateProfileString("Map", "ID", "Unknown", MapIDBuf, sizeof(MapIDBuf), StringToLPCSTR(Dir));
			MapID = atoi(MapIDBuf);

			if (CurMap != MapID)
			{
					
				CurMap = MapID;

				TCHAR Buf[32];
				_itot(MapID, Buf, 10);

				GetPrivateProfileString(Buf, "island", "Unknown", IslandBuf, sizeof(IslandBuf), StringToLPCSTR(Map));
				IslandName = IslandBuf;
				GUI::Zuko->islandlabel->Text = LPCSTRToString(IslandName);

				GetPrivateProfileString(Buf, "topName", "Unknown", TopNameBuf, sizeof(TopNameBuf), StringToLPCSTR(Map));
				TopName = TopNameBuf;
				GUI::Zuko->topnamelabel->Text = LPCSTRToString(TopName);

				GetPrivateProfileString(Buf, "mapName", "Unknown", MapBuf, sizeof(MapBuf), StringToLPCSTR(Map));
				MapName = MapBuf;
				GUI::Zuko->maplabel->Text = LPCSTRToString(MapName) + " - " + MapID.ToString();

				//GUI::Zuko->MiniMap->Image = GetMiniMap(MapID);
			
				GUI::Zuko->npclabel->Text = "NPC's: " + NPC;
			} 
		}

		if (fMouseBase)
		{
			if (fMouseOffset && fMouseX && fMouseY)
			{
				GetPrivateProfileString("Mouse", "X", "", mXBuf, sizeof(mXBuf), StringToLPCSTR(Dir));
				MouseX = atoi(mXBuf);

				GetPrivateProfileString("Mouse", "Y", "", mYBuf, sizeof(mYBuf), StringToLPCSTR(Dir));
				MouseY = atoi(mYBuf);
		
				GUI::Zuko->mouselabel->Text = "Mouse: " + MouseX.ToString() + "," + MouseY.ToString();
			}
		}

		if (fServerBase)
		{
			if (fServerOffset)
			{
				GetPrivateProfileString("Server", "World", "", ServerBuf, sizeof(ServerBuf), StringToLPCSTR(Dir));
				World = atoi(ServerBuf);
				switch (World)
				{
					case 0:
						GUI::Zuko->worldlabel->Text = "World: Scania";
					break;
					case 1:
						GUI::Zuko->worldlabel->Text = "World: Bera";
					break;
					case 2:
						GUI::Zuko->worldlabel->Text = "World: Broa";
					break;
					case 3:
						GUI::Zuko->worldlabel->Text = "World: Windia";
					break;
					case 4:
						GUI::Zuko->worldlabel->Text = "World: Khaini";
					break;
					case 5:
						GUI::Zuko->worldlabel->Text = "World: Bellocan";
					break;
					case 6:
						GUI::Zuko->worldlabel->Text = "World: Mardia";
					break;
					case 7:
						GUI::Zuko->worldlabel->Text = "World: Kradia";
					break;
					case 8:
						GUI::Zuko->worldlabel->Text = "World: Yellonde";
					break;
					case 9:
						GUI::Zuko->worldlabel->Text = "World: Demethos";
					break;
					case 10:
						GUI::Zuko->worldlabel->Text = "World: Galicia";
					break;
					case 11:
						GUI::Zuko->worldlabel->Text = "World: ElNido";
					break;
					case 12:
						GUI::Zuko->worldlabel->Text = "World: Zenith";
					break;
					case 13:
						GUI::Zuko->worldlabel->Text = "World: Arcania";
					break;
					case 14:
						GUI::Zuko->worldlabel->Text = "World: Chaos";
					break;
					case 15:
						GUI::Zuko->worldlabel->Text = "World: Nova";
					break;
					case 16:
						GUI::Zuko->worldlabel->Text = "World: Renegades";
					break;
				}

				GetPrivateProfileString("Server", "Channel", "", ChannelBuf, sizeof(ChannelBuf), StringToLPCSTR(Dir));
				Channel = atoi(ChannelBuf);
				Channel += 1;
				GUI::Zuko->channellabel->Text = "Channel: " + Channel.ToString();
			}
		}

		Sleep(100); 
	}
}

bool CheckLogin()
{
	for (;;)
	{

		while (MapleOpen)
		{
			if (LoggedIn) // If Map ID is 0 Check if we're logged in.
			{
				if (MapID <= 0)
				{
					Sleep(5000);
					WaitForResponse();
					if (MapID <= 0) // Check again, if someone was going through a portal, this would indicate false
					{
						LoggedIn = false;
						GetMapleSize();
						SetMSSize(369,5, MapleRect.right, MapleRect.bottom); // Resize the game incase its 1024x768
						if (Embeded)
						{
							if (MapleRect.right > 800 && MapleRect.bottom > 600)
								SetWindowPos(Gui, HWND_TOP,10,10,1402,815, SWP_FRAMECHANGED);
							else
								SetWindowPos(Gui, HWND_TOP,10,10,1185,652, SWP_FRAMECHANGED);
						}
						SendKey(VK_ESCAPE); // Make sure all Disconnected from maple windows are closed before reattempting login

						GUI::Zuko->Status->Text = "Waiting for Login...";
						zLog("Disconnected!");
						GUI::Zuko->ProgressBar->Value = 75;
					}
				}
			}

			if (!LoggedIn)
			{
				if (MapID > 0)
				{
					LoggedIn = true;
					GetMapleSize();
					SetMSSize(369,5,MapleRect.right, MapleRect.bottom); 
					if (Embeded)
					{
						if (MapleRect.right > 800 && MapleRect.bottom > 600)
							SetWindowPos(Gui, HWND_TOP,10,10,1402,815, SWP_FRAMECHANGED); //Auto detect correct window sizes for different resolution settings.
						else
							SetWindowPos(Gui, HWND_TOP,10,10,1185,658, SWP_FRAMECHANGED);
					}
					GUI::Zuko->Status->Text = "MapleStory Connected!";
					zLog("Logged In!");
					GUI::Zuko->ProgressBar->Value = 100;
				}
			}
			Sleep(100);
		}
		Sleep(100);
	}
	return true;
}

bool Login()
{
	do 
	{
		Sleep(10000);
		GUI::Zuko->Status->Text = "MapleStory logging in...";
		Press(PW);
		Sleep(500);
		Press("{ENTER}");
		Sleep(100);
		WaitForResponse(); WaitForResponse();
		zLog("Selecting Server...");
		GUI::Zuko->ProgressBar->Value = 80;
		Sleep(3000);
		//TODO: We shouldn't be using this info, we should be using what's in the loaded profile.
		if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Wizet\\MapleStory", &skey) == ERROR_SUCCESS || RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Wow6432Node\\MapleStory", &skey) == ERROR_SUCCESS)
		{
			RegQueryValueEx(skey, "LCWN", NULL, 0,(LPBYTE)&WorldBuf, &dWorld);

			if (strncmp(WorldBuf, "Scania", 6) == 0)
				wSelected = 0;
			if (strncmp(WorldBuf, "Bera", 4) == 0)
				wSelected = 1;
			if (strncmp(WorldBuf, "Broa", 4) == 0)
				wSelected = 2;
			if (strncmp(WorldBuf, "Windia", 6) == 0)
				wSelected = 3;
			if (strncmp(WorldBuf, "Khaini", 6) == 0)
				wSelected = 4;
			if (strncmp(WorldBuf, "Bellocan", 8) == 0)
				wSelected = 5;
			if (strncmp(WorldBuf, "Mardia", 6) == 0)
				wSelected = 6;
			if (strncmp(WorldBuf, "Kradia", 6) == 0)
				wSelected = 7;
			if (strncmp(WorldBuf, "Yellonde", 8) == 0)
				wSelected = 8;
			if (strncmp(WorldBuf, "Demethos", 8) == 0)
				wSelected = 9;
			if (strncmp(WorldBuf, "Galicia", 7) == 0)
				wSelected = 10;
			if (strncmp(WorldBuf, "ElNido", 7) == 0)
				wSelected = 11;
			if (strncmp(WorldBuf, "Zenith", 6) == 0)
				wSelected = 12;
			if (strncmp(WorldBuf, "Arcania", 7) == 0)
				wSelected = 13;
			if (strncmp(WorldBuf, "Chaos", 5) == 0)
				wSelected = 14;
			if (strncmp(WorldBuf, "Nova", 4) == 0)
				wSelected = 15;
			if (strncmp(WorldBuf, "Renegades", 9) == 0)
				wSelected = 16;
			RegCloseKey(skey);
		
			if (wSelected != wNeeded)
			{
				for( int i=wSelected; i!=-1; i--) //Brings us to Last Server
				{
					Press("{TAB}");
					Sleep(100);
				}

				int Distance = TotalWorlds - wNeeded; // Calculate distance to our server
				for (int i=0; i<Distance; i++) // Brings us to Our Server
				{
					Press("{TAB}");
					Sleep(100);
				}
			}
			GUI::Zuko->ProgressBar->Value = 85;
			Press("{ENTER}"); //Enter the Server Channel Selection
			zLog("Selecting Channel...");
			Sleep(1500);
			for (int i=2; i!=sChannel; i++)
			{
				Press("VKEY 27");
				Sleep(200);
			}
			Press("{ENTER}"); //Enter the Character Selection
			zLog("Selecting Character...");
			while (Result == NULL) // Wait For Maple To Respond
			{
				Result = SendMessageTimeout(MapleHWND, WM_NULL, 0, 0, SMTO_ABORTIFHUNG, 500, &Respond);
				Sleep(100);
			}
			Result = NULL; // Set Result to Null Again
			GUI::Zuko->ProgressBar->Value = 90;
			Sleep(2000);

			for (int i=0; i!=16; i++) // Select First Character
			{
				Press("VKEY 25"); // Left Button
				Sleep(250);
			}
			for (int i=1; i!=sChar; i++) // Goto Our Char 
			{
				Press("VKEY 27");
				Sleep(100);
			}
			Press("{ENTER}"); //Enter PIC Type
			zLog("Entering PIC...");
			GUI::Zuko->ProgressBar->Value = 95;
			Sleep(1000);
			Press(PIC);
			Sleep(1000);
			Press("{ENTER}");
			while (Result == NULL && MapleHWND != NULL)
			{
				Result = SendMessageTimeout(MapleHWND, WM_NULL, 0, 0, SMTO_ABORTIFHUNG, 500, &Respond);
				Sleep(100);
			}
			Result = NULL; // Set Result to Null Again
			Sleep(3000);
			if (MapID > 0) // Check if Were in.
			{
				LoggedIn = true;
				return true;
			}
		}
		else
		{
			TerminateThread(StartThread, 0);
			GUI::Zuko->Status->Text = "Could Not Get Information Required!";
			GUI::Zuko->ProgressBar->Value = 0;
			return false;
		}
	} while (FindWindow(MSClass, ProfName) != NULL && LoggedIn == false);

	if (FindWindow(MSClass, ProfName) == NULL && isProcessRunning(MapleHandle) != 0 )
	{
		TerminateProcess(MapleHandle, 0);
	}
	return false;
}

bool StartMaple()
{
	System::String ^ZSDir = Directory::GetCurrentDirectory();
	ZSDir = ZSDir + "\\ZS.dll";

	System::String ^MSDir = Directory::GetCurrentDirectory();
	MSDir = MSDir + "\\MS.dll";

	GUI::Zuko->Status->Text = "Checking Game Servers..."; //TODO: Maybe move server check to trainer startup not MS startup.
	if (!ServerCheck())
	{
		zLog("Servers Active!");
		GUI::Zuko->ProgressBar->Value = 5;

		if(ProfileLoaded)
		{
			if (MapleDir.length() > 0)
			{
				if(!MapleOpen) //while loop and continuely check?
				{
					#ifdef BYPASS //Testing purposes
					HSBypassed = NULL; // Null These out cause Maple is reopening
					CRCBypassed = NULL;
					CipherBypassed = NULL;
					#endif

					if (File::Exists(LPCSTRToString(MapleDir.c_str())))
					{
						if (Embeded)
						GUI::Zuko->EmbedCB->Checked = true;
					
						if (!Embeded)
						GUI::Zuko->EmbedCB->Checked = false;

						GUI::Zuko->ProgressBar->Value = 10;

						STARTUPINFO info = { sizeof(info) };
						PROCESS_INFORMATION processInfo;

						if (!FindWindow(NULL, "MapleStory"))
						{
							TerminateProcess(GetProcessHandle("MapleStory.exe"), 0);
							ShellExecute(MapleHWND,"open", MapleDir.c_str(), 0, 0, SW_SHOWNORMAL);
							//CreateProcess(MapleDir.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &info, &processInfo);
						}
						else
						{
							TerminateProcess(MapleHandle, 0);
							TerminateProcess(HSHandle, 0);
							Starting = false;
							GUI::Zuko->Status->Text = "MapleStory is Already Running!";
							GUI::Zuko->ProgressBar->Value = 0;
							return false;
						}

						GUI::Zuko->Status->Text = ("Waiting for Maple..");
						while (MaplePID == NULL)
						MaplePID = WaitForProcess("MapleStory.exe"); // Get Process ID

						Starting = true;
						GUI::Zuko->ProgressBar->Value = 15;
						if (MaplePID != NULL)
						{
							MapleHandle = GetProcessHandle("MapleStory.exe"); // Make Sure MapleStory has been launched
							GUI::Zuko->ProgressBar->Value = 20;

							GUI::Zuko->Status->Text = "Connecting...";
							if (InjectDLL(MaplePID, (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ZSDir)))
							{
								GUI::Zuko->ProgressBar->Value = 25;
								zLog("Connected!");
								while (FindWindow(NULL, MsgWndTitle) == NULL) // Wait for Our Message Window to Come Up
								{
									Sleep(50);
									MsgWnd = FindWindow(NULL, MsgWndTitle);
								} 
								ShowWindow(MsgWnd, SW_HIDE);
								Sleep(2000);
								Msg(_UpdateLogoSkip);
								
								if (fLogoSkip && LogoSkipOn)
								{
									zLog("Logo Skipped!");
									Msg(_LogoSkip); Sleep(1000);  Msg(_LogoSkip); Sleep(1000); Msg(_LogoSkip);
								}

								SendMessage(MapleHWND,WM_CLOSE, 0, 0);// Close Launcher, Launches Maple
							
								GUI::Zuko->ProgressBar->Value = 30;
							
								MaplePID = WaitForProcess("MapleStory.exe"); // Check For Patches, If Maple Closes a patch needs to be done
								if (MaplePID != NULL)
								{
									if (/*MapleHWND != NULL*/0==0)
									{
										if (MapleHandle != NULL)
										{
											if (GetProcessHandle("MapleStory.exe") == NULL)
											{
												TerminateProcess(MapleHandle, 0);
												TerminateProcess(HSHandle, 0);
												Starting = false;
												GUI::Zuko->Status->Text = "MapleStory Was Terminated!";
												GUI::Zuko->ProgressBar->Value = 0;
												return false;
											}
											GUI::Zuko->ProgressBar->Value = 35;
											GUI::Zuko->ProgressBar->Value = 40;

											if (CipherBypassed && HSBypassed && CRCBypassed)
											{
												GUI::Zuko->ProgressBar->Value = 50;
												GUI::Zuko->Status->Text = "Waiting For MapleStory...";

												MapleHWND = NULL;

												while (MapleHWND == NULL && WaitForProcess("MapleStory.exe"))
												MapleHWND = FindWindow(MSClass, NULL); //*************

												if (!WaitForProcess("MapleStory.exe"))
												{
													TerminateProcess(MapleHandle, 0);
													TerminateProcess(HSHandle, 0);
													Starting = false;
													GUI::Zuko->Status->Text = "Maple Took too Long Start, or Was Terminated!";
													GUI::Zuko->ProgressBar->Value = 0;
													MSWaitTime = 0;
													return false;
												}

												GUI::Zuko->Status->Text = "Waiting for Game Window...";

												SetWindowTextA(MapleHWND, "MapleStory");
												MapleHWND = FindWindow(MSClass, "MapleStory");
												while(!IsWindowVisible(MapleHWND) && FindWindow(MSClass, "MapleStory"))
												{
													Sleep(1000);
												}
				
												if (!FindWindow(MSClass, "MapleStory"))
												{
														TerminateProcess(MapleHandle, 0);
														TerminateProcess(HSHandle, 0);
														Starting = false;
														GUI::Zuko->Status->Text = "Maple Took too Long Start, or Was Terminated!";
														GUI::Zuko->ProgressBar->Value = 0;
														MSWaitTime = 0;
														return false;
													
												}
												MapleOpen = true;

												MSWaitTime = 0; // Set the Time it Waited to 0 Again incase of Re Launch
												GUI::Zuko->ProgressBar->Value = 60;
												GUI::Zuko->Status->Text = "Waiting For Login Screen...";
												while (Result == NULL && MapleHWND != NULL)
												{
													Sleep(500);
													MapleHWND = FindWindow(MSClass, "MapleStory");
													Result = SendMessageTimeout(MapleHWND, WM_NULL, 0, 0, SMTO_ABORTIFHUNG, WaitTime, &Respond);
												}
												Result = NULL; // Nullify Result Again

												GUI::Zuko->ProgressBar->Value = 70;
												if (IsWindow(MapleHWND))
												{
													while (!IsWindow(MapleHWND))
														Sleep(1000);
													
													MapleHWND = FindWindow(MSClass, "MapleStory");
													SetWindowText(MapleHWND, ProfName);
													MapleHWND = FindWindow(MSClass, ProfName);
													GUI::Zuko->ProgressBar->Value = 80;


													SetWindowPos(MapleHWND, HWND_TOP,495,5,800,600, SWP_FRAMECHANGED);
													if (Embeded)
														EmbedMaple(true); // If Embeded than Embed Maple and Set it as ForeGround Window

													if (MapleHidden) //If MapleStory Set Hidden Than Hide it
														HideMaple();

													if (MapleHWND != NULL)
													{
														WaitForResponse();

														Msg(_UpdateAddys);
														
														while (!Updated && MapleHWND != NULL)
														{
															GUI::Zuko->Status->Text = "Updating...";
															MapleHWND = FindWindow(MSClass, ProfName);
															Sleep(100);
														}
														GUI::Zuko->ProgressBar->Value = 90;

														if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadPointers, 0, 0, NULL))
															zLog("Pointers Scanning");
														else
															zLog("ERROR! RESTART TRAINER AND MAPLESTORY");

														if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartupHacks, 0, 0, NULL))
															zLog("Hacks Enabled");
														else
															zLog("ERROR! RESTART TRAINER AND MAPLESTORY");

														if (GUI::Zuko->LoginCB->Checked )
														{
															if (fPic)
															{
																CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Login, 0, 0, NULL);
																return true;
															}
															else
															{
																zLog("Cannot Auto Login Without Updated PIC Type!");
															}
														}
															//LoggedIn = false; // Testing Purposes.. Should be Normally commented out
															GUI::Zuko->Status->Text = "Waiting for Login...";
															GUI::Zuko->ProgressBar->Value = 95;
															return true;
													}
													else
													{
														TerminateProcess(MapleHandle, 0);
														TerminateProcess(HSHandle, 0);
														GUI::Zuko->ProgressBar->Value = 0;
														Starting = false;
														return false;
													}
												}
												else // If MapleStory Window Isn't Shown
												{
													GUI::Zuko->Status->Text = "MapleStory Failed to Start!";
													TerminateProcess(MapleHandle, 0);
													TerminateProcess(HSHandle, 0);
													GUI::Zuko->ProgressBar->Value = 0;
													Starting = false;
													return false;
												}
											}
											else // Bypass Cipher Failed
											{
												GUI::Zuko->Status->Text = "Failed to Bypass MapleStory Shields";
												TerminateProcess(MapleHandle, 0);
												TerminateProcess(HSHandle, 0);
												GUI::Zuko->ProgressBar->Value = 0;
												Starting = false;
												return false;
											}
										} 
										else // If Cant Get Maple Process Handle 
										{
											GUI::Zuko->Status->Text = "Failed to Get Process Handle, MS Did Not Opened";
											TerminateProcess(MapleHandle, 0);
											TerminateProcess(HSHandle, 0);
											GUI::Zuko->ProgressBar->Value = 0;
											Starting = false;
											return false;
										}
									}
									else // If Maple Isnt Open
									{
										GUI::Zuko->Status->Text = "Failed to Get Window Handle, Try Again";
										TerminateProcess(MapleHandle, 0);
										TerminateProcess(HSHandle, 0);
										GUI::Zuko->ProgressBar->Value = 0;
										Starting = false;
										return false;
									}
								}
								else // MapleStory Was Killed Due to Patch Or Just Failed to Respond in amount of time
								{
									GUI::Zuko->Status->Text = "Failed to Connect, Check For Any Patches!";
									TerminateProcess(MapleHandle, 0);
									TerminateProcess(HSHandle, 0);
									GUI::Zuko->ProgressBar->Value = 0;
									Starting = false;
									return false;
								}
							}
							else // Failed to Inject DLL
							{
								GUI::Zuko->Status->Text = "Failed to Connect ZS!";
								TerminateProcess(MapleHandle, 0);
								TerminateProcess(HSHandle, 0);
								GUI::Zuko->ProgressBar->Value = 0;
								Starting = false;
								return false;
							}
						}
						else // Couldnt get MapleStory Process Handle
						{
							GUI::Zuko->Status->Text = "Failed Get MapleStory PID!";
							TerminateProcess(MapleHandle, 0);
							GUI::Zuko->ProgressBar->Value = 0;
							Starting = false;
							return false;
						}
					}
					else // If MapleStory doesn't Exist in the directory
					{
						::MessageBox(Gui, "MapleStory Not Found in the Directory Set", Title, MB_OK);
						GUI::Zuko->Status->Text = "Not Connected";
						GUI::Zuko->ProgressBar->Value = 0;
						Starting = false;
						return false;
					}
				}
				else // If Maple is Already Open
				{
					::MessageBox(Gui, "MapleStory is Already Open", Title, MB_OK);
					GUI::Zuko->ProgressBar->Value = 0;
					Starting = false;
					return false;
				}
			}
			else // If MapleStory directory isn't Set
			{
				::MessageBox(Gui, "No MapleStory Directory Set", Title, MB_OK);
				GUI::Zuko->Status->Text = "Not Connected";
				GUI::Zuko->ProgressBar->Value = 0;
				Starting = false;
				return false;
			}
		}
		else // If No Profile is Loaded
		{
			::MessageBox(Gui, "No Profile Loaded", Title, MB_OK);
			GUI::Zuko->Status->Text = "Not Connected";
			GUI::Zuko->ProgressBar->Value = 0;
			Starting = false;
			return false;
		}
	}
	else
	{
		GUI::Zuko->Status->Text = "MapleStory Servers Detected Offline!";
		Starting = false;
		return false;
	}
}

void GUI::StartButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (!GetProcessHandle("MapleStory.exe") || FindWindow(NULL, "MapleStory") && !Starting)
	{
		StartThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartMaple, 0, 0, NULL);
	
		if (StartThread == NULL)
		{
			::MessageBox(Gui, "Failed to Start MapleStory Thread! \nPlease Reinstall ZSTrainer", Title, MB_OK);
			ExitProcess(0);
		}


		Checker = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MSCheck, 0, 0, NULL);

		if (Checker == NULL)
		{
			::MessageBox(Gui, "Could Not Find Start MSChecker Routine! \nPlease Reinstall ZSTrainer", Title, MB_OK);
			ExitProcess(0);
		}

		LoginChecker = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckLogin, 0, 0, NULL);

		if (LoginChecker == NULL)
		{
			::MessageBox(Gui, "Could Not Find Start Login Check Routine! \nPlease Reinstall ZSTrainer", Title, MB_OK);
			ExitProcess(0);
		}
	}
	else
	{
		::MessageBox(Gui, "MapleStory is Already Open", Title, MB_OK);
	}
}	

void GUI::SettingsButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	AllSetting().ShowDialog();
}

bool EmbedMaple(bool x)
{
	if(x)
	{
		hParent = SetParent(MapleHWND, Gui);
		if (hParent)
		{
			if (MapleRect.right > 0 && MapleRect.bottom  > 0)
			{
				if (MapleRect.right > 800 && MapleRect.bottom > 600 && !MapleHidden)
					SetWindowPos(Gui, HWND_TOP,10,10,1392,802, SWP_FRAMECHANGED);
				else
					SetWindowPos(Gui, HWND_TOP,10,10,1185,632, SWP_FRAMECHANGED);

				SetWindowPos(MapleHWND, HWND_TOP,353,5,MapleRect.right, MapleRect.bottom, SWP_FRAMECHANGED);
			}
			else
			{
				if (!MapleHidden)
				SetWindowPos(Gui, HWND_TOP,10,10,1165,638, SWP_FRAMECHANGED);

				SetWindowPos(MapleHWND, HWND_TOP,353,5,800,600, SWP_FRAMECHANGED);			
			}
			eMapleHWND = FindWindowEx(Gui, FindWindow(MSClass, NULL), MSClass, ProfName);

			Embeded = true;
		}
		else
		{
			SetWindowPos(Gui, HWND_TOP,10,10,475,751, SWP_FRAMECHANGED);
			GUI::Zuko->EmbedCB->Checked = false;
			return false;
		}
	}
	
	else if (!x)
	{
		SetParent(MapleHWND, hParent);
		SetWindowPos(Gui, HWND_TOP,10,10,475,751, SWP_FRAMECHANGED);
		Embeded = false;
	}
	return true;
}


void GUI::EmbedCB_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
		if (EmbedCB->Checked == true) 
		{
			if (!Embeded)
			{
				if (MapleOpen)
				{
					EmbedMaple(true);
				}
				else //If Maple Isnt Open Then Set Embeded to True, It will Embed Automatically on Game Start
				{
					Embeded = true; 
				}
			}
		}
		else // If Not Checked
		{
			if (Embeded)
			{
				if (MapleOpen)
				{
					EmbedMaple(false);
				}
				else // If Maple Isnt Open then Set Embed to False, It Wont Embed Automatically On Start
				{
					Embeded = false;
				}
			}
		}
}

void HideMaple()
{
	if (Embeded) // If Maple is Open then To the Embeded Procedure
	SetWindowPos(Gui, HWND_TOP,10,10,475,751, SWP_FRAMECHANGED);

	ShowWindow(MapleHWND, SW_HIDE);
	MapleHidden = true;
}

void ShowMaple()
{
	if (Embeded)
	{
		if (MapleRect.right > 800 && MapleRect.bottom > 600)
		SetWindowPos(Gui, HWND_TOP,10,10,1392,802, SWP_FRAMECHANGED);
		else
		SetWindowPos(Gui, HWND_TOP,10,10,1185,652, SWP_FRAMECHANGED);
	}

	ShowWindow(MapleHWND, SW_SHOW);
	MapleHidden = false;
}

void GUI::HideCB_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
		if (HideCB->Checked == true)
		{
			if (!MapleOpen) // If Maple is Not Open Yet than Set Maplehidden to True, and Hide it Upon Game Start // Used for Auto Login
			{
				MapleHidden = true; // The Game Start button Will Hide it Upon Startup
			}
			else // If Maple Is Open Do the Hide Maple Proceduere
			{
				HideMaple();
			}
		}
		else // If Not Checked
		{
				if (!MapleOpen) // If Maple is Not Open Yet than Set Maplehidden to False, and Show it Upon Game Start // Used for Auto Login
				{
					MapleHidden =  false; //The Game Start button will Hide it Upon Startup
				}

				else // If Maple Is Open Then do the De-Embeding Procedure
				{
					ShowMaple();
				}
		}
}

void GUI::LoginCB_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (LoginCB->Checked)
		typePIC = true;
	else
		typePIC = false;
}

void GUI::WndProc(System::Windows::Forms::Message %m) 
{
	switch (m.Msg)
    {
		case _UpdateAddys:
			Updated = true;	
		break;
		case _BlackCipher:
			if (!CipherBypassed)
			{
				CipherBypassed = true;
				zLog("BlackCipher Bypassed!");
			}
		break;
		case _fBlackCipher:
			CipherBypassed = false;
			zLog("Failed to Bypass BlackCipher");
		break;
		case _iHackShield:
			zLog("Failed to Get HS Data");
		break;
		case _HackShield:
			HSBypassed = true;
			zLog("Hack Shield Bypassed!");
		break;
		case _fHackShield:
			HSBypassed = false;
		break;
		case _CRCMain:
			zLog("Update CRC Err.1");
		break;
		case _CRCSecondary:
			zLog("Update CRC Err.2");
		break;
		case _CRCTriatery:
			zLog("Update CRC Err.3");
		break;
		case _CRCBypassed:
			zLog("CRC Bypassed!");
			CRCBypassed = true;
		break;
		case _CRCFail:
			zLog("Failed to Bypass MS CRC");
			CRCBypassed = false;
		break;
		case _CRCInternalDetection:
			zLog("Update CRC Err.4");
		break;
		case _HackShield1:
			fHSAddy1 = false;
			zLog("Failed to Update Main HS Address");
		break;
		case _HackShield2:
			fHSAddy2 = false;
			zLog("Failed to Update Secondary HS Address");
		break;
		case _HS_CallBack_1:
			zLog("Update HS Err.1");
		break;
		case _HS_CallBack_2:
			zLog("Update HS Err.2");
		break;
		case _HS_NanoScan_1:
			zLog("Update HS Err.3");
		break;
		case _HS_NanoScan_2:
			zLog("Update HS Err.4");
		break;
		case _HS_Detection:
			zLog("Update HS Err.5");
		break;
		case _HS_Assembly:
			zLog("Update HS Err.6");
		break;
		case _HS_Anti_Crash:
			zLog("Update HS Err.7");
		break;
		case _MobError:
			zLog("Thread Err.1");
		break;
		case _ReadPointer:
				zLog("Could Not Start Reading Pointers, Please Retry");
				TerminateThread(StartThread, 0);
				TerminateThread(Checker, 0);
				TerminateThread(LoginChecker, 0);
				GUI::Zuko->Status->Text = "Not Connected";
				GUI::Zuko->ProgressBar->Value = 0;
		break;
		case _GetFunction:
			FunctionFound = true;
		break;
		case _UpdateHacks:
			zLog("Could Not Start Reading Hack Addresses, Please Retry");
			TerminateThread(StartThread, 0);
			TerminateThread(Checker, 0);
			TerminateThread(LoginChecker, 0);
			GUI::Zuko->Status->Text = "Not Connected";
			GUI::Zuko->ProgressBar->Value = 0;
		break;
		//CHARACTER
		case _CharacterBase:
			zLog("Character Base Not Found");
			fCharBase = false;
		break;
		case _StatHook:
			zLog("Stat Hook Not Found");
			fStatHook = false;
		break;
		case _XOffset:
			if (fMiniBase && fMiniX)
			{
				zLog("X/Y Position Offset Not Found, Back Up Addresses Will Be Used To Get Character X/Y!");
			}
			else
			{
				zLog("X/Y Position Offset Not Found");
			}
			fCharX = false;
			fCharY = false;
		break;
		case _TeleportOffset:
			zLog("Teleport Toggle Offset Not Found");
			fTeleport = false;
		break;
		case _TeleportXOffset:
			zLog("Teleport X Offset Not Found");
			fTeleportX = false;
			fTeleportY = false;
		break;
		case _BreathOffset:
			zLog("Breath Offset Not Found");
			fBreath = false;
		break;
		case _AttackOffset:
			zLog("Attack Offset Not Found");
			fAttack = false;
		break;
		// MOUSE
		case _MouseBase: 
			zLog("Mouse Base Not Found");
			fMouseBase = false;
		break;
		case _MouseOff:
			zLog("Mouse Base Offset Not Found");
			fMouseOffset = false;
		break;
		case _AnimationOffset:
			zLog("Animation Offset Not Found");
		break;

		//SERVER
		case _ServerBase:
			zLog("Server Base Not Found");
			fServerBase = false;
		break;
		case _ServerOffset:
			zLog("Server Offset Not Found");
			fServerOffset = false;
		break;
		//INFO
		case _InfoBase:
			zLog("Info Base Not Found");
			fInfoBase = false;
		break;
		case _HPAlert:
			zLog("HP Alert Not Found");
			fHpAlertOffset = false;
		break;
		case _MPAlert:
			zLog("MP Alert Not Found");
			fMpAlertOffset = false;
		break;
		// GUI
		case _StatBase:
			zLog("Gui Base Not Found");
			fStatBase = false;
		break;
		case _HPOffset:
			zLog("HP Offset Not Found");
			fHPOffset = false;
		break;
		case _MPOffset:
			zLog("MP Offset Not Found");
			fMPOffset = false;
		break;
		case _EXPOffset:
			zLog("EXP Offset Not Found");
			fEXPOffset = false;
		break;
		// Mob
		case _MobBase:
			zLog("Mob Base Not Found");
			fMobBase = false;
		break;
		case _CountOffset:
			zLog("Mob Count Not Found");
			fMobOffset = false;
		break;
		case _Mob1Offset:
			zLog("Mob 1 Not Found");
			fm1 = false;
		break;
		case _Mob2Offset:
			zLog("Mob 2 Not Found");
			fm2 = false;
		break;
		case _Mob3Offset:
			zLog("Mob 3 Not Found");
			fm3 = false;
		break;
		case _Mob4Offset:
			zLog("Mob 4 Not Found");
			fm4 = false;
		break;
		case _MobX:
			zLog("Mob X Not Found");
			fMobX = false;
		break;
		case _MobY:
			zLog("Mob Y Not Found");
			fMobY = false;
		break;
		case _MobDeath:
			zLog("Mob Death Not Found");
			fMobDeath = false;
		break;
		// People
		case _PeopleBase:
			zLog("People Base Not Found");
			fPeopleBase = false;
		break;
		case _PeopleOffset:
			zLog("People Offset Not Found");
			fPeopleOffset = false;
		break;
		case _MapBase:
			zLog("Map Base Not Found");
			fMapBase = false;
		break;
		case _MapOffset: 
			zLog("Map Offset Not Found");
			fMapOffset = false;
		break;
		// Item
		case _ItemBase:
			zLog("Item Base Not Found");
			fItemBase = false;
		break;
		case _ItemOffset:
			zLog("Item Offset Not Found");
			fItemOffset = false;
		break;
		case _ItemHook:
			zLog("Item Hook Not Found");
			fItemHook = false;
		break;
		//Wall
		case _WallBase:
			zLog("Wall Base Not Found");

		break;
		case _WallLeft:
			zLog("Wall Left Not Found");

		break;
		case _WallRight:
			zLog("Wall Right Not Found");

		break;
		case _WallTop:
			zLog("Wall Top Not Found");

		break;
		case _WallBottom:
			zLog("Wall Bottom Not Found");

		break;
		// Mini Map
		case _MiniMapBase:
			zLog("Mini Map Base Not Found");
			fMiniBase = false;
		break;
		case _MiniMapX:
			zLog("MiniMap X Not Found");
			fMiniX = false;
		break;
		case _MiniMapY: 
			zLog("MiniMap Y Not Found");
			fMiniY = false;
		break;
		// Rope
		case _RopeBase:
			zLog("Rope Base Not Found");

		break;
		case _RopeOffset:
			zLog("Rope Offset Not Found");

		break;
		//PIC
		case _PIC:
			zLog("PIC Address Not Found");
			GUI::LoginCB->Checked = false;
			GUI::LoginCB->Enabled = false;
			fPic = false;
		break;

		case _Kami:
			zLog("Kami Address Not Found");
			GUI::VacCB->Enabled = false;
			GUI::VacCB->Checked = false;
			fKami = false;
		break;

		case _Tele:
			zLog("Tele Address Not Found");
			GUI::VacCB->Enabled = false;
			GUI::VacCB->Checked = false;
			fKami = false;
		break;

		case _Filter:
			zLog("Filter Address Not Found");
			GUI::FilterCB->Checked = false;
			GUI::FilterCB->Enabled = false;
			fFilter = false;
		break;

		case _LogoSkip:
			zLog("Logo Skip Address Not Found");
			fLogoSkip = false;
		break; 
    }
	System::Windows::Forms::Form::WndProc(m);
}

/////////////CHECKBOXES/////////////////
void GUI::VacCB_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (fKami)
		Msg(_Kami);
}
void GUI::FilterCB_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (fFilter)
		Msg(_Filter);
}