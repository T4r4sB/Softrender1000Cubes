#include "tbsound.h"
#include "tbal.h"

#ifdef _WINDOWS
namespace tbsound
{
	void Sound::Load (const char* filename)
	{
		(void)filename;
	}

	void Sound::Play ()
	{
	}

	void Sound::Stop ()
	{
	}

	void Sound::Destroy ()
	{
	}


	SoundManager::SoundManager ()
	{
	}

	void SoundManager::Destroy ()
	{
	}
	
	void SoundManager::Init ()
	{
	}
};
#endif

#ifdef ANDROID

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace tbsound 
{
	void FreeObj (SLObjectItf& obj)
	{
		if (obj)
		{
			(*obj)->Destroy(obj);
			obj=NULL;
		}
	}

	void SoundManager::Init ()
	{
		const SLInterfaceID pIDs[1] = {SL_IID_ENGINE};
		const SLboolean pIDsRequired[1] = {SL_BOOLEAN_TRUE};
		SLresult result;
		result = slCreateEngine (&engineObj, 0, NULL, 1, pIDs, pIDsRequired);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in slCreateEngine!");
    result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in Realize engineObj!");
		result = (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in GetInterface!");
		const SLInterfaceID pOutputMixIDs[] = {};
		const SLboolean pOutputMixRequired[] = {};
		result = (*engine)->CreateOutputMix(engine, &outputMixObj, 0, pOutputMixIDs, pOutputMixRequired);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in CreateOutputMix!");
    result = (*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in Realize outputMixObj!");
	}

	void SoundManager::Destroy ()
	{
		FreeObj(outputMixObj);
		FreeObj(engineObj);
	}	

	void Sound::Load (const char* fileName)
	{
		tbal::LogI(tblib::Format("Loading file %").s(fileName));

		tbal::FileDescriptor fileDescriptor;

		{
			tbal::Asset asset;
			asset.Open(fileName);
			asset.GetDescriptor(fileDescriptor);		
		}

		SLDataLocator_AndroidFD locatorIn = {
			SL_DATALOCATOR_ANDROIDFD,
			fileDescriptor.descr,
			fileDescriptor.start,
			fileDescriptor.length
		};		

		SLDataFormat_MIME dataFormat = {
			SL_DATAFORMAT_MIME,
			NULL,
			SL_CONTAINERTYPE_UNSPECIFIED
		};
		SLDataSource audioSrc = {
			&locatorIn, 
			&dataFormat
		};
		SLDataLocator_OutputMix dataLocatorOut = {
			SL_DATALOCATOR_OUTPUTMIX,
			soundManager.outputMixObj
		};
		SLDataSink audioSnk = {
			&dataLocatorOut,
			NULL
		};
		const SLInterfaceID pIDs[2] = {
			SL_IID_PLAY, 
			SL_IID_SEEK
		};
		const SLboolean pIDsRequired[2] = {
			SL_BOOLEAN_TRUE, 
			SL_BOOLEAN_TRUE
		};

		SLresult result;
		result = (*soundManager.engine)->CreateAudioPlayer(soundManager.engine, &playerObj, &audioSrc, &audioSnk, 2, pIDs, pIDsRequired);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in CreateAudioPlayer!");
		result = (*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in Realize playerObj!");
	}

	void Sound::Play ()
	{
		SLPlayItf player;
		SLresult result;
		SLSeekItf seek;
		result = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in GetInterface SL_IID_PLAY!");
		result = (*playerObj)->GetInterface(playerObj, SL_IID_SEEK, &seek);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in GetInterface SL_IID_SEEK!");
		(*seek)->SetLoop(seek, SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN);

		result = (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in SetPlayState SL_PLAYSTATE_STOPPED!");
		result = (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in SetPlayState SL_PLAYSTATE_PLAYING!");
	}

	void Sound::Stop ()
	{
		SLPlayItf player;
		SLresult result;
		SLSeekItf seek;
		result = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in GetInterface SL_IID_PLAY!");
		result = (*playerObj)->GetInterface(playerObj, SL_IID_SEEK, &seek);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in GetInterface SL_IID_SEEK!");
		(*seek)->SetLoop(seek, SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN);

		result = (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED);
		if (result != SL_RESULT_SUCCESS) tbal::LogW("Error in SetPlayState SL_PLAYSTATE_STOPPED!");
	}

	void Sound::Destroy ()
	{
		FreeObj(playerObj);
	}
};
#endif