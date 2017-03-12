#include "tbal.h"

#ifdef _WINDOWS
namespace tbsound
{
	struct SoundManager
	{
		void Init ();
		void Destroy ();
		SoundManager ();
		~SoundManager () {Destroy();}
	private: SoundManager operator =(const SoundManager&);
	};
	
	struct Sound
	{
		SoundManager &soundManager;
		void Load (const char* filename);
		void Play ();
		void Stop ();
		void Destroy ();
		Sound (SoundManager& soundManager) : soundManager(soundManager) {}
		~Sound ()  {Destroy();}
	private: Sound operator =(const Sound&);
	};
};
#endif

#ifdef ANDROID
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
namespace tbsound
{
	struct SoundManager
	{
		SLObjectItf engineObj;
		SLEngineItf engine;
		SLObjectItf outputMixObj;

		void Init ();
		void Destroy ();
		SoundManager () : engineObj(NULL), engine(NULL), outputMixObj(NULL) {}
		~SoundManager () {Destroy();}
	};
	
	struct Sound
	{
		SLObjectItf playerObj;

		SoundManager &soundManager;
		void Load (const char* filename);
		void Play ();
		void Stop ();
		void Destroy ();
		Sound (SoundManager& soundManager) : soundManager(soundManager), playerObj(NULL) {}
		~Sound () {Destroy();}
	};
};
#endif