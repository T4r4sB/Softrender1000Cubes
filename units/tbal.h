#pragma once

/*
ƒопустимые флаги
TBAL_MOUSE_OVER
TBAL_HIDE_CURSOR
TBAL_ALLOW_BIG_SIZE
TBAL_16_BITS
TBAL_EMULATE_DOUBLE_BUFFER
TBAL_VERTICAL
TBAL_HORIZONTAL
*/

#include "stdint.h"
#include "tbslice.h"
#include "tbarr2.h"
#include "tbstream.h"
#include "tbstring.h"
#include "bitmaps.h"

#ifdef ANDROID
	#include <jni.h>
	#include <android_native_app_glue.h> 
	#include <android/log.h>
#endif

#ifdef _WINDOWS
	#include "windows.h" 
#endif
namespace tbal {  

	enum Action {
		ACTION_DOWN=0,
		ACTION_MOVE=1,
		ACTION_UP=2,
		ACTION_LOST=3 };

	tblib::StrBuf SetProjectName (tblib::StringRef newName);
	tblib::StrBuf SetHardCmdLine (tblib::StringRef newHardCmdLine);
	tblib::StrBuf GetExternalFilesDir ();
	void RunUrl (tblib::StringRef url);
	float GetXDPI ();
	float GetYDPI ();
	void SetTimer (int interval);

	void LogI(tblib::StringRef c);
	void LogW(tblib::StringRef c);


	class Buffer : public Bitmap 
	{
		Buffer operator = (const Buffer&);
		Buffer (const Buffer&);// нельз€ копировать
	public : 
		Buffer  ();
		~Buffer ();	
	}; 

	#ifdef ANDROID
		struct FileDescriptor 
		{
			int32_t descr;
			off_t start;
			off_t length;
		};
	#endif
	#ifdef _WINDOWS
		typedef tblib::IFStream FileDescriptor;
	#endif

	struct Asset
	{
		Asset ();
		~Asset ();
		void Open (tblib::StringRef fileName);
		tblib::StringRef GetBuffer();
		void GetDescriptor(FileDescriptor& fd);
	private :
		#ifdef ANDROID
			AAsset *asset;
		#endif
		#ifdef _WINDOWS
			tblib::StrBuf fileName;
			tblib::HeapCArray<char> buffer;
			bool loaded;
			void Load();
		#endif
		Asset (const Asset&);
		Asset& operator = (const Asset&);
	};

#ifdef _WINDOWS
	HWND GetWindowHandle();
#endif

	bool SetSingleBuffer (bool newSingle);
	bool IsSingleBuffer ();
	#ifdef _WINDOWS
	// коды клавиш ну и не только
		enum Code {
			CODE_UP      = VK_UP,
			CODE_DOWN    = VK_DOWN,
			CODE_LEFT    = VK_LEFT,
			CODE_RIGHT   = VK_RIGHT,
			CODE_CTRL    = VK_CONTROL,
			CODE_TAB     = VK_TAB,
			CODE_ESCAPE  = VK_ESCAPE, 
			CODE_HOME    = VK_HOME,
			CODE_ENTER   = VK_RETURN ,
			
			CODE_F1   = VK_F1,
			CODE_F2   = VK_F2,
			CODE_F3   = VK_F3,
			CODE_F4   = VK_F4,
			CODE_F5   = VK_F5,
			CODE_F6   = VK_F6,
			CODE_F7   = VK_F7,
			CODE_F8   = VK_F8,
			CODE_F9   = VK_F9,
			CODE_F10  = VK_F10,
			CODE_F11  = VK_F11,
			CODE_F12  = VK_F12,

			CODE_BACKSPACE = VK_BACK,
		
			CODE_POINTER   = 256,
			CODE_TIMER     = 257,
			CODE_START	   = 258,
			CODE_EXIT      = 259,
			CODE_LOSTFOCUS = 260,
			CODE_GETNAME   = 261,

			CODE_CUSTOM    = 512, 

			CODE_0		 = '0',
			CODE_1		 = '1',
			CODE_2		 = '2',
			CODE_3		 = '3',
			CODE_4		 = '4',
			CODE_5		 = '5',
			CODE_6		 = '6',
			CODE_7		 = '7',
			CODE_8		 = '8',
			CODE_9		 = '9',
			CODE_A		 = 'A',
			CODE_B		 = 'B',
			CODE_C		 = 'C',
			CODE_D		 = 'D',
			CODE_E		 = 'E',
			CODE_F		 = 'F',
			CODE_G		 = 'G',
			CODE_H		 = 'H',
			CODE_I		 = 'I',
			CODE_J		 = 'J',
			CODE_K		 = 'K',
			CODE_L		 = 'L',
			CODE_M		 = 'M',
			CODE_N		 = 'N',
			CODE_O		 = 'O',
			CODE_P		 = 'P',
			CODE_Q		 = 'Q',
			CODE_R		 = 'R',
			CODE_S		 = 'S',
			CODE_T		 = 'T',
			CODE_U		 = 'U',
			CODE_V		 = 'V',
			CODE_W		 = 'W',
			CODE_X		 = 'X',
			CODE_Y		 = 'Y',
			CODE_Z		 = 'Z',
			CODE_SPACE = VK_SPACE};
	#endif

	#ifdef ANDROID
		enum Code	{
			CODE_UP      = AKEYCODE_DPAD_UP    ,
			CODE_DOWN    = AKEYCODE_DPAD_DOWN  ,
			CODE_LEFT    = AKEYCODE_DPAD_LEFT  ,
			CODE_RIGHT   = AKEYCODE_DPAD_RIGHT ,
			CODE_POINTER = 256,
			CODE_TIMER   = 257,
			CODE_ESCAPE  = AKEYCODE_BACK,
			CODE_HOME    = AKEYCODE_HOME,
			CODE_ENTER   = AKEYCODE_ENTER,
			CODE_START	 = 258,
			CODE_EXIT    = 259,
			CODE_LOSTFOCUS = 260,
			CODE_GETNAME   = 261,
			CODE_CUSTOM    = 512, 
			// јЌƒ–ќ»ƒќЅЋяƒ—“¬ќ
			CODE_0		 = AKEYCODE_0,
			CODE_1		 = AKEYCODE_1,
			CODE_2		 = AKEYCODE_2,
			CODE_3		 = AKEYCODE_3,
			CODE_4		 = AKEYCODE_4,
			CODE_5		 = AKEYCODE_5,
			CODE_6		 = AKEYCODE_6,
			CODE_7		 = AKEYCODE_7,
			CODE_8		 = AKEYCODE_8,
			CODE_9		 = AKEYCODE_9,
			CODE_A		 = AKEYCODE_A,
			CODE_B		 = AKEYCODE_B,
			CODE_C		 = AKEYCODE_C,
			CODE_D		 = AKEYCODE_D,
			CODE_E		 = AKEYCODE_E,
			CODE_F		 = AKEYCODE_F,
			CODE_G		 = AKEYCODE_G,
			CODE_H		 = AKEYCODE_H,
			CODE_I		 = AKEYCODE_I,
			CODE_J		 = AKEYCODE_J,
			CODE_K		 = AKEYCODE_K,
			CODE_L		 = AKEYCODE_L,
			CODE_M		 = AKEYCODE_M,
			CODE_N		 = AKEYCODE_N,
			CODE_O		 = AKEYCODE_O,
			CODE_P		 = AKEYCODE_P,
			CODE_Q		 = AKEYCODE_Q,
			CODE_R		 = AKEYCODE_R,
			CODE_S		 = AKEYCODE_S,
			CODE_T		 = AKEYCODE_T,
			CODE_U		 = AKEYCODE_U,
			CODE_V		 = AKEYCODE_V,
			CODE_W		 = AKEYCODE_W,
			CODE_X		 = AKEYCODE_X,
			CODE_Y		 = AKEYCODE_Y,
			CODE_Z		 = AKEYCODE_Z,
			CODE_SPACE = AKEYCODE_SPACE};
	#endif
			
	int MSec (); 
	int CreateScreenButton (Code code, int px, int py, int sx, int sy, Color transparentColor, const Picture& image, const Picture& imagePushed, bool creepable = false);
	bool ShowButton (int number, bool show);
	void GetButtonPosition (int number, int &px, int &py, int &sx, int &sy);
	void SetButtonPosition (int number, int  px, int  py, int  sx, int  sy);
	void SetButtonImages(int number, Color transparentColor, const Picture& image, const Picture& imagePushed);	
	void DrawButton (const Bitmap& buffer, int number);
	void EnableButtonDraw (bool enable);
	void EnableMouse (bool enable);
	void DrawAllButtons (const Bitmap& buffer);
	bool Pressed (Code code);		
};

bool TbalMain (tbal::Action action, tbal::Code code, int x, int y);
