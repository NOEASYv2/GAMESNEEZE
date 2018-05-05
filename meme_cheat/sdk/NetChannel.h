#pragma once


namespace sdk
{
	class bf_read;
	class bf_write;
	struct SndInfo_t {
		// Sound Guid
		int			m_nGuid;
		void* m_filenameHandle;		// filesystem filename handle - call IFilesystem to conver this to a string
		int			m_nSoundSource;
		int			m_nChannel;
		// If a sound is being played through a speaker entity (e.g., on a monitor,), this is the
		//  entity upon which to show the lips moving, if the sound has sentence data
		int			m_nSpeakerEntity;
		float		m_flVolume;
		float		m_flLastSpatializedVolume;
		// Radius of this sound effect (spatialization is different within the radius)
		float		m_flRadius;
		int			m_nPitch;
		Vector		*m_pOrigin;
		Vector		*m_pDirection;

		// if true, assume sound source can move and update according to entity
		bool		m_bUpdatePositions;
		// true if playing linked sentence
		bool		m_bIsSentence;
		// if true, bypass all dsp processing for this sound (ie: music)	
		bool		m_bDryMix;
		// true if sound is playing through in-game speaker entity.
		bool		m_bSpeaker;
		// for snd_show, networked sounds get colored differently than local sounds
		bool		m_bFromServer;
	};

	enum soundlevel_t {
		SNDLVL_NONE = 0,

		SNDLVL_20dB = 20,			// rustling leaves
		SNDLVL_25dB = 25,			// whispering
		SNDLVL_30dB = 30,			// library
		SNDLVL_35dB = 35,
		SNDLVL_40dB = 40,
		SNDLVL_45dB = 45,			// refrigerator

		SNDLVL_50dB = 50,	// 3.9	// average home
		SNDLVL_55dB = 55,	// 3.0

		SNDLVL_IDLE = 60,	// 2.0	
		SNDLVL_60dB = 60,	// 2.0	// normal conversation, clothes dryer

		SNDLVL_65dB = 65,	// 1.5	// washing machine, dishwasher
		SNDLVL_STATIC = 66,	// 1.25

		SNDLVL_70dB = 70,	// 1.0	// car, vacuum cleaner, mixer, electric sewing machine

		SNDLVL_NORM = 75,
		SNDLVL_75dB = 75,	// 0.8	// busy traffic

		SNDLVL_80dB = 80,	// 0.7	// mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile
		SNDLVL_TALKING = 80,	// 0.7
		SNDLVL_85dB = 85,	// 0.6	// average factory, electric shaver
		SNDLVL_90dB = 90,	// 0.5	// screaming child, passing motorcycle, convertible ride on frw
		SNDLVL_95dB = 95,
		SNDLVL_100dB = 100,	// 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
		SNDLVL_105dB = 105,			// helicopter, power mower
		SNDLVL_110dB = 110,			// snowmobile drvrs seat, inboard motorboat, sandblasting
		SNDLVL_120dB = 120,			// auto horn, propeller aircraft
		SNDLVL_130dB = 130,			// air raid siren

		SNDLVL_GUNFIRE = 140,	// 0.27	// THRESHOLD OF PAIN, gunshot, jet engine
		SNDLVL_140dB = 140,	// 0.2

		SNDLVL_150dB = 150,	// 0.2

		SNDLVL_180dB = 180,			// rocket launching

									// NOTE: Valid soundlevel_t values are 0-255.
									//       256-511 are reserved for sounds using goldsrc compatibility attenuation.
	};
	class IRecipientFilter {
	public:
		virtual			~IRecipientFilter( ) { }

		virtual bool	IsReliable( void ) const = 0;
		virtual bool	IsInitMessage( void ) const = 0;

		virtual int		GetRecipientCount( void ) const = 0;
		virtual int		GetRecipientIndex( int slot ) const = 0;
	};

#define	PITCH_NORM		100			// non-pitch shifted
#define PITCH_LOW		95			// other values are possible - 0-255, where 255 is very high
#define PITCH_HIGH		120

	class IEngineSound {
	public:
		// Precache a particular sample
		virtual bool PrecacheSound( const char *pSample, bool bPreload = false, bool bIsUISound = false ) = 0;
		virtual bool IsSoundPrecached( const char *pSample ) = 0;
		virtual void PrefetchSound( const char *pSample ) = 0;

		// Just loads the file header and checks for duration (not hooked up for .mp3's yet)
		// Is accessible to server and client though
		virtual float GetSoundDuration( const char *pSample ) = 0;

		// Pitch of 100 is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
		// down to 1 is a lower pitch.   150 to 70 is the realistic range.
		// EmitSound with pitch != 100 should be used sparingly, as it's not quite as
		// fast (the pitchshift mixer is not native coded).

		// NOTE: setting iEntIndex to -1 will cause the sound to be emitted from the local
		// player (client-side only)
		virtual void EmitSound( IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
			float flVolume, float flAttenuation, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0,
			const Vector *pOrigin = NULL, const Vector *pDirection = NULL, std::vector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1 ) = 0;

		virtual void EmitSound( IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
			float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0,
			const Vector *pOrigin = NULL, const Vector *pDirection = NULL, std::vector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1 ) = 0;

		virtual void EmitSentenceByIndex( IRecipientFilter& filter, int iEntIndex, int iChannel, int iSentenceIndex,
			float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0,
			const Vector *pOrigin = NULL, const Vector *pDirection = NULL, std::vector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1 ) = 0;

		virtual void StopSound( int iEntIndex, int iChannel, const char *pSample ) = 0;

		// stop all active sounds (client only)
		virtual void StopAllSounds( ) = 0;

		// Set the room type for a player (client only)
		virtual void SetRoomType( IRecipientFilter& filter, int roomType ) = 0;

		// Set the dsp preset for a player (client only)
		virtual void SetPlayerDSP( IRecipientFilter& filter, int dspType, bool fastReset ) = 0;

		// emit an "ambient" sound that isn't spatialized
		// only available on the client, assert on server
		virtual void EmitAmbientSound( const char *pSample, float flVolume, int iPitch = PITCH_NORM, int flags = 0, float soundtime = 0.0f ) = 0;


		//	virtual EntChannel_t	CreateEntChannel() = 0;

		virtual float GetDistGainFromSoundLevel( soundlevel_t soundlevel, float dist ) = 0;

		// Client .dll only functions
		virtual int		GetGuidForLastSoundEmitted( ) = 0;
		virtual bool	IsSoundStillPlaying( int guid ) = 0;
		virtual void	StopSoundByGuid( int guid ) = 0;
		// Set's master volume (0.0->1.0)
		virtual void	SetVolumeByGuid( int guid, float fvol ) = 0;

		// Retrieves list of all active sounds
		virtual void	GetActiveSounds( std::vector < SndInfo_t >& sndlist ) = 0;

		virtual void	PrecacheSentenceGroup( const char *pGroupName ) = 0;
		virtual void	NotifyBeginMoviePlayback( ) = 0;
		virtual void	NotifyEndMoviePlayback( ) = 0;
	};
	class INetMessage {
	public:
		virtual	~INetMessage( ) { };

		// Use these to setup who can hear whose voice.
		// Pass in client indices (which are their ent indices - 1).

		virtual void	SetNetChannel( NetChannel * netchan ) = 0; // netchannel this message is from/for
		virtual void	SetReliable( bool state ) = 0;	// set to true if it's a reliable message

		virtual bool	Process( void ) = 0; // calles the recently set handler to process this message

		virtual	bool	ReadFromBuffer( bf_read &buffer ) = 0; // returns true if parsing was OK
		virtual	bool	WriteToBuffer( bf_write &buffer ) = 0;	// returns true if writing was OK

		virtual bool	IsReliable( void ) const = 0;  // true, if message needs reliable handling

		virtual int				GetType( void ) const = 0; // returns module specific header tag eg svc_serverinfo
		virtual int				GetGroup( void ) const = 0;	// returns net message group of this message
		virtual const char		*GetName( void ) const = 0;	// returns network message name, eg "svc_serverinfo"
		virtual NetChannel		*GetNetChannel( void ) const = 0;
		virtual const char		*ToString( void ) const = 0; // returns a human readable string about message content
	};
	class NetChannel
	{
	public:
		char pad_0000 [ 24 ]; //0x0000
		int m_nOutSequenceNr; //0x0018
		int m_nInSequenceNr; //0x001C
		int m_nOutSequenceNrAck; //0x0020
		int m_nOutReliableState; //0x0024
		int m_nInReliableState; //0x0028
		int m_nChokedPackets; //0x002C
		bool SendNetMsg( INetMessage* msg, bool bForceReliable, bool bVoice ) {
			typedef bool( __thiscall* func1 )( void*, INetMessage*, bool, bool );
			return VMT::VMTHookManager::GetFunction<func1>( this, 42 )( this, msg, bForceReliable, bVoice );
		}
		char pad_0030 [ 108 ]; //0x0030
		int m_Socket; //0x009C
		int m_StreamSocket; //0x00A0
		int m_MaxReliablePayloadSize; //0x00A4
		char pad_00A8 [ 100 ]; //0x00A8
		float last_received; //0x010C
		float connect_time; //0x0110
		char pad_0114 [ 4 ]; //0x0114
		int m_Rate; //0x0118
		char pad_011C [ 4 ]; //0x011C
		float m_fClearTime; //0x0120
		char pad_0124 [ 16688 ]; //0x0124
		char m_Name [ 32 ]; //0x4254
		unsigned int m_ChallengeNr; //0x4274
		float m_flTimeout; //0x4278
		char pad_427C [ 32 ]; //0x427C
		float m_flInterpolationAmount; //0x429C
		float m_flRemoteFrameTime; //0x42A0
		float m_flRemoteFrameTimeStdDeviation; //0x42A4
		int m_nMaxRoutablePayloadSize; //0x42A8
		int m_nSplitPacketSequence; //0x42AC
		char pad_42B0 [ 40 ]; //0x42B0
		bool m_bIsValveDS; //0x42D8
		char pad_42D9 [ 65 ]; //0x42D9
	};
}