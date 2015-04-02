/*

   Copyright [2008] [Trevor Hogan]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   CODE PORTED FROM THE ORIGINAL GHOST PROJECT: http://ghost.pwner.org/

*/

#include "ghost.h"
#include "util.h"
#include "language.h"
#include "socket.h"
#include "commandpacket.h"
#include "bnet.h"
#include "map.h"
#include "gameplayer.h"
#include "gameprotocol.h"
#include "gcbiprotocol.h"
#include "gpsprotocol.h"
#include "game_base.h"
#include "ghostdb.h"

//
// CPotentialPlayer
//

CPotentialPlayer :: CPotentialPlayer( CGameProtocol *nProtocol, CBaseGame *nGame, CTCPSocket *nSocket ) : m_Protocol( nProtocol ), m_Game( nGame ), m_Socket( nSocket ), m_DeleteMe( false ), m_Error( false ), m_IncomingJoinPlayer( NULL ), m_IncomingGarenaUser( NULL ), m_ConnectionState( 0 ), m_ConnectionTime( GetTicks( ) ), m_Banned( false )
{
	if( nSocket )
		m_CachedIP = nSocket->GetIPString( );
	else
		m_CachedIP = string( ); // 01032013 EntBot
		
	m_Protocol = nProtocol;
	m_Game = nGame;
	m_Socket = nSocket;
	m_DeleteMe = false;
	m_Error = false;
	m_IncomingJoinPlayer = NULL;
	m_LAN = false;
	m_LANSet = false;
}

CPotentialPlayer :: ~CPotentialPlayer( )
{
	if( m_Socket )
		delete m_Socket;

	while( !m_Packets.empty( ) )
	{
		delete m_Packets.front( );
		m_Packets.pop( );
	}

	delete m_IncomingJoinPlayer;
	delete m_IncomingGarenaUser;
}
BYTEARRAY CPotentialPlayer :: GetGarenaIP( )
{
	if( m_IncomingGarenaUser == NULL ) {
		return UTIL_CreateByteArray( (uint32_t) 0, true );
	} else {
		return UTIL_CreateByteArray( m_IncomingGarenaUser->GetIP( ), true );
	}
}
BYTEARRAY CPotentialPlayer :: GetExternalIP( )
{
	unsigned char Zeros[] = { 0, 0, 0, 0 };
	BYTEARRAY IP;

	if( m_Socket )
	{	
		if( m_IncomingGarenaUser != NULL )
			return GetGarenaIP( );
		else {
			bool local=false;
			IP=	m_Socket->GetIP( );
			if (IP.size()>=2)
			{
				if (IP[0]==10)
				local=true;
				else if (IP[0]==192 && IP[1]==168)
				local=true;
				else if (IP[0]==169 && IP[1]==254)
				local=true;
				else if (IP[0]==172 && IP[1]==16)
				local=true;
				else if (IP[0]==172 && IP[1]==17)
				local=true;
				else if (IP[0]==172 && IP[1]==18)
				local=true;
				else if (IP[0]==172 && IP[1]==19)
				local=true;
				else if (IP[0]==172 && IP[1]==20)
				local=true;
				else if (IP[0]==172 && IP[1]==21)
				local=true;
				else if (IP[0]==172 && IP[1]==22)
				local=true;
				else if (IP[0]==172 && IP[1]==23)
				local=true;
				else if (IP[0]==172 && IP[1]==24)
				local=true;
				else if (IP[0]==172 && IP[1]==25)
				local=true;
				else if (IP[0]==172 && IP[1]==26)
				local=true;
				else if (IP[0]==172 && IP[1]==27)
				local=true;
				else if (IP[0]==172 && IP[1]==28)
				local=true;
				else if (IP[0]==172 && IP[1]==29)
				local=true;
				else if (IP[0]==172 && IP[1]==30)
				local=true;
				else if (IP[0]==172 && IP[1]==31)
				local=true;
			}
			if (local && m_Game->m_GHost->m_ExternalIP!="")
			{
				IP=UTIL_CreateByteArray(m_Game->m_GHost->m_ExternalIPL,true);
			}
			return IP;
		}
	}

	return UTIL_CreateByteArray( Zeros, 4 );
}

string CPotentialPlayer :: GetExternalIPString( )
{
	BYTEARRAY IP;
	string EIP;
	if( m_Socket )
	{
		if( m_IncomingGarenaUser != NULL ) 
		{
			BYTEARRAY GarenaIP = GetGarenaIP( );
			return UTIL_ToString(GarenaIP[0]) + "." + UTIL_ToString(GarenaIP[1]) + "." + UTIL_ToString(GarenaIP[2]) + "." + UTIL_ToString(GarenaIP[3]);
		} else {	
			bool local=m_LAN;
			if (!m_LANSet)
			{				
				IP=	m_Socket->GetIP( );
				if (IP.size()>=2)
				{
					if (IP[0]==10)
					local=true;
					else if (IP[0]==192 && IP[1]==168)
					local=true;
					else if (IP[0]==169 && IP[1]==254)
					local=true;
					else if (IP[0]==172 && IP[1]==16)
					local=true;
					else if (IP[0]==172 && IP[1]==17)
					local=true;
					else if (IP[0]==172 && IP[1]==18)
					local=true;
					else if (IP[0]==172 && IP[1]==19)
					local=true;
					else if (IP[0]==172 && IP[1]==20)
					local=true;
					else if (IP[0]==172 && IP[1]==21)
					local=true;
					else if (IP[0]==172 && IP[1]==22)
					local=true;
					else if (IP[0]==172 && IP[1]==23)
					local=true;
					else if (IP[0]==172 && IP[1]==24)
					local=true;
					else if (IP[0]==172 && IP[1]==25)
					local=true;
					else if (IP[0]==172 && IP[1]==26)
					local=true;
					else if (IP[0]==172 && IP[1]==27)
					local=true;
					else if (IP[0]==172 && IP[1]==28)
					local=true;
					else if (IP[0]==172 && IP[1]==29)
					local=true;
					else if (IP[0]==172 && IP[1]==30)
					local=true;
					else if (IP[0]==172 && IP[1]==31)
					local=true;
				}
				if (UTIL_IsLocalIP(IP, m_Game->m_GHost->m_LocalAddresses))
					local = true;
				m_LANSet = true;
				m_LAN = local;
			}
			EIP=m_Socket->GetIPString( );
			if (local && m_Game->m_GHost->m_ExternalIP!="")
			{
			EIP=m_Game->m_GHost->m_ExternalIP;
			}
			if( !EIP.empty( ) && EIP != "0.0.0.0" )
				return EIP;
			else
				return m_CachedIP;
		}
	}
	return m_CachedIP;
}

bool CPotentialPlayer :: Update( void *fd )
{
	if( m_DeleteMe )
		return true;

	if( !m_Socket )
		return false;

	m_Socket->DoRecv( (fd_set *)fd );
	ExtractPackets( );
	ProcessPackets( );
	
	// make sure we don't keep this socket open forever (disconnect after five seconds)
	if( m_ConnectionState == 0 && GetTicks( ) - m_ConnectionTime > m_Game->m_GHost->m_DenyMaxReqjoinTime && !m_Banned )
	{
		CONSOLE_Print( "[DENY] Kicking player: REQJOIN not received within a few seconds" );
		m_DeleteMe = true;
		m_Game->m_GHost->DenyIP( GetExternalIPString( ), m_Game->m_GHost->m_DenyReqjoinDuration, "REQJOIN not received within a few seconds" );
		
	}

	// don't call DoSend here because some other players may not have updated yet and may generate a packet for this player
	// also m_Socket may have been set to NULL during ProcessPackets but we're banking on the fact that m_DeleteMe has been set to true as well so it'll short circuit before dereferencing

	return m_DeleteMe || m_Error || m_Socket->HasError( ) || !m_Socket->GetConnected( );
}

void CPotentialPlayer :: ExtractPackets( )
{
	if( !m_Socket )
		return;

	// extract as many packets as possible from the socket's receive buffer and put them in the m_Packets queue

	string *RecvBuffer = m_Socket->GetBytes( );
	BYTEARRAY Bytes = UTIL_CreateByteArray( (unsigned char *)RecvBuffer->c_str( ), RecvBuffer->size( ) );

	// a packet is at least 4 bytes so loop as long as the buffer contains 4 bytes

	while( Bytes.size( ) >= 4 )
	{
		if( Bytes[0] == W3GS_HEADER_CONSTANT || Bytes[0] == GPS_HEADER_CONSTANT || Bytes[0] == GCBI_HEADER_CONSTANT )
		{
			// bytes 2 and 3 contain the length of the packet

			uint16_t Length = UTIL_ByteArrayToUInt16( Bytes, false, 2 );

			if( Length >= 4 )
			{
				if( Bytes.size( ) >= Length )
				{
					m_Packets.push( new CCommandPacket( Bytes[0], Bytes[1], BYTEARRAY( Bytes.begin( ), Bytes.begin( ) + Length ) ) );
					*RecvBuffer = RecvBuffer->substr( Length );
					Bytes = BYTEARRAY( Bytes.begin( ) + Length, Bytes.end( ) );
				}
				else
					return;
			}
			else
			{
				m_Error = true;
				m_ErrorString = "received invalid packet from player (bad length)";
				return;
			}
		}
		else
		{
			m_Error = true;
			m_ErrorString = "received invalid packet from player (bad header constant)";
			return;
		}
	}
}

void CPotentialPlayer :: ProcessPackets( )
{
	if( !m_Socket )
		return;

	// process all the received packets in the m_Packets queue

	while( !m_Packets.empty( ) )
	{
		CCommandPacket *Packet = m_Packets.front( );
		m_Packets.pop( );

		if( Packet->GetPacketType( ) == W3GS_HEADER_CONSTANT )
		{
			// the only packet we care about as a potential player is W3GS_REQJOIN, ignore everything else

			switch( Packet->GetID( ) )
			{
			case CGameProtocol :: W3GS_REQJOIN:
				delete m_IncomingJoinPlayer;
				m_IncomingJoinPlayer = m_Protocol->RECEIVE_W3GS_REQJOIN( Packet->GetData( ) );

				if( m_IncomingJoinPlayer && !m_Banned )
					m_Game->EventPlayerJoined( this, m_IncomingJoinPlayer );

				// don't continue looping because there may be more packets waiting and this parent class doesn't handle them
				// EventPlayerJoined creates the new player, NULLs the socket, and sets the delete flag on this object so it'll be deleted shortly
				// any unprocessed packets will be copied to the new CGamePlayer in the constructor or discarded if we get deleted because the game is full

				delete Packet;
				return;
			}
		}		
		
		else if( Packet->GetPacketType( ) == GCBI_HEADER_CONSTANT )
		{
//			if( Packet->GetID( ) == CGCBIProtocol :: GCBI_INIT && m_Game->m_GHost->IsLocal( GetExternalIPString( ) ) )
			if( Packet->GetID( ) == CGCBIProtocol :: GCBI_INIT )
			{
				delete m_IncomingGarenaUser;
				m_IncomingGarenaUser = m_Game->m_GHost->m_GCBIProtocol->RECEIVE_GCBI_INIT( Packet->GetData( ) );
				string RoomID = UTIL_ToString(m_IncomingGarenaUser->GetRoomID( ));
				m_RoomName = m_Game->m_GHost->GetRoomName( string( RoomID.begin( ), RoomID.end( ) ) );
				CONSOLE_Print( "[GCBI] Garena user detected; userid=" + UTIL_ToString( m_IncomingGarenaUser->GetUserID( ) ) + ", roomid=" + RoomID + ", RoomName=" + m_RoomName + ", experience=" + UTIL_ToString( m_IncomingGarenaUser->GetUserExp( ) ) + ", country=" + m_IncomingGarenaUser->GetCountryCode( ) );
			}
		}

		delete Packet;
	}
}
bool CGamePlayer :: IsSpammer( )
{	
	if( m_LastMessages.size() < 7 )		
		return false;

	string message = m_LastMessages.front( );

	//CONSOLE_Print("7 messages in queue: comparing %s", message.c_str( ) );

	for(list<string>::iterator i = m_LastMessages.begin( ); i != m_LastMessages.end( ); i++ )
	{
		if( message != *i)
		{
	//		count << message << " does not match " << *i << endl;
			return false;
		}
		
	}
	
	return true;
}

bool CGamePlayer :: IsAbuser( )
{	
	if( m_LastCMDMessages.size() < 8 )		
		return false;

	string message = m_LastCMDMessages.front( );

	//CONSOLE_Print("7 messages in queue: comparing %s", message.c_str( ) );

	for(list<string>::iterator i = m_LastCMDMessages.begin( ); i != m_LastCMDMessages.end( ); i++ )
	{
		if( message != *i)
		{
	//		count << message << " does not match " << *i << endl;
			return false;
		}
		
	}
	
	return true;
}

void CGamePlayer :: PushMessage( string message, bool check )
{	
	if( m_LastMessages.size( ) >= 7)
		m_LastMessages.pop_front( );
	m_LastMessages.push_back(message);
	if( m_LastMessages.size( ) == 6 )
		if(!(check || m_Game->IsOwner(GetName()))){
		//	if (!m_Game->IsOwner(GetName())){
				m_Game->SendAllChat( m_Game->m_GHost->m_Language->KickMsgForSpammer( GetName( ) ) );
				CONSOLE_Print("Spam control detects a spammer "+GetName()+" will be KICKED for flooding");
			/* } else if( !message.empty( ) && message[0] == m_Game->m_GHost->m_CommandTrigger && ( message.find("close") != string ::npos || message.find("open") != string ::npos )){
				m_Game->m_GHost->m_Callables.push_back( m_Game->m_GHost->m_DB->ThreadedBanAdd( GetJoinedRealm( ), GetName( ), GetExternalIPString(), m_Game->GetGameName( ), "AUTOWARN", "Abusing commands", m_Game->m_GHost->m_WarnTimeOfWarnedPlayer, 1 ));
				m_DeleteMe = true;				
				SetLeftReason( "Player "+GetName()+" appears to be a command abuser!" );				
				SetLeftCode( PLAYERLEAVE_LOBBY );
				m_Game->OpenSlot( m_Game->GetSIDFromPID( GetPID( ) ), false );
			} */
		}	
}

void CGamePlayer :: PushCMDMessage( string message, bool check )
{	
	if( m_LastCMDMessages.size( ) >= 8)
		m_LastCMDMessages.pop_front( );
	m_LastCMDMessages.push_back(message);
	if( m_LastCMDMessages.size( ) == 7 )
		if(!check){
			m_Game->SendAllChat( m_Game->m_GHost->m_Language->KickMsgForAbuser( GetName( ) ) );
			CONSOLE_Print("[" + m_Game->GetGameName( )+ "] Player "+GetName()+" appears to be a command abuser!");
		}	
}

void CPotentialPlayer :: Send( BYTEARRAY data )
{
	if( m_Socket )
		m_Socket->PutBytes( data );
}

//
// CGamePlayer
//

CGamePlayer :: CGamePlayer( CGameProtocol *nProtocol, CBaseGame *nGame, CTCPSocket *nSocket, unsigned char nPID, string nJoinedRealm, string nName, BYTEARRAY nInternalIP, bool nReserved ) : CPotentialPlayer( nProtocol, nGame, nSocket )
{
	m_PID = nPID;
	m_Name = nName;
	m_InternalIP = nInternalIP;
	m_JoinedRealm = nJoinedRealm;
	m_TotalPacketsSent = 0;
	m_TotalPacketsReceived = 0;
	m_LeftCode = PLAYERLEAVE_LOBBY;
	m_LoginAttempts = 0;
	m_SyncCounter = 0;
	m_JoinTime = GetTime( );
	m_LastMapPartSent = 0;
	m_LastMapPartAcked = 0;
	m_StartedDownloadingTicks = 0;
	m_FinishedDownloadingTime = 0;
	m_FinishedLoadingTicks = 0;
	m_StartedLaggingTicks = 0;
	m_TotalLaggingTicks = 0;
	m_StatsSentTime = 0;
	m_StatsDotASentTime = 0;
	m_LastGProxyWaitNoticeSentTime = 0;
	m_Score = -100000.0;
	m_LoggedIn = false;
	m_Spoofed = false;
	m_Reserved = nReserved;
	m_WhoisShouldBeSent = false;
	m_WhoisSent = false;
	m_DownloadAllowed = false;
	m_DownloadStarted = false;
	m_DownloadFinished = false;
	m_FinishedLoading = false;
	m_Lagging = false;
	m_DropVote = false;
	m_KickVote = false;
	m_StartVote = false;
	m_RmkVote = false;
	m_Muted = false;
	m_LeftMessageSent = false;
	m_GProxy = false;
	m_GProxyDisconnectNoticeSent = false;
	m_GProxyReconnectKey = GetTicks( );
	m_LastGProxyAckTime = 0;
	m_TimeActive = 0;
	m_DownloadInfo = string();
	m_ScoreSet = false;
	m_RankSet = false;
	m_ScoreS = "0";
	m_RankS = "0";
	m_Censors = 0;
	m_Silence = false;
	m_DOTAKills = 0;
	m_DOTADeaths = 0;
	m_DOTAAssists = 0;
	m_Provider =string();
	m_Country =string();
	m_Switched = false;
	m_Switching = false;
	m_Switchok = false;
	m_WarnChecked = false;	
	m_ConnectionState = 1;
}

CGamePlayer :: CGamePlayer( CPotentialPlayer *potential, unsigned char nPID, string nJoinedRealm, string nName, BYTEARRAY nInternalIP, bool nReserved ) : CPotentialPlayer( potential->m_Protocol, potential->m_Game, potential->GetSocket( ) )
{
	// todotodo: properly copy queued packets to the new player, this just discards them
	// this isn't a big problem because official Warcraft III clients don't send any packets after the join request until they receive a response

	// m_Packets = potential->GetPackets( );
	m_PID = nPID;
	m_Name = nName;
	m_InternalIP = nInternalIP;
	m_JoinedRealm = nJoinedRealm;
	m_TotalPacketsSent = 0;

	// hackhack: we initialize this to 1 because the CPotentialPlayer must have received a W3GS_REQJOIN before this class was created
	// to fix this we could move the packet counters to CPotentialPlayer and copy them here
	// note: we must make sure we never send a packet to a CPotentialPlayer otherwise the send counter will be incorrect too! what a mess this is...
	// that said, the packet counters are only used for managing GProxy++ reconnections
	m_ConnectionState = 1;

	m_TotalPacketsReceived = 1;
	m_LeftCode = PLAYERLEAVE_LOBBY;
	m_LoginAttempts = 0;
	m_SyncCounter = 0;
	m_JoinTime = GetTime( );
	m_LastMapPartSent = 0;
	m_LastMapPartAcked = 0;
	m_StartedDownloadingTicks = 0;
	m_FinishedDownloadingTime = 0;
	m_FinishedLoadingTicks = 0;
	m_StartedLaggingTicks = 0;
	m_TotalLaggingTicks = 0;
	m_StatsSentTime = 0;
	m_StatsDotASentTime = 0;
	m_LastGProxyWaitNoticeSentTime = 0;
	m_Score = -100000.0;
	m_LoggedIn = false;
	m_Spoofed = false;
	m_Reserved = nReserved;
	m_WhoisShouldBeSent = false;
	m_WhoisSent = false;
	m_DownloadAllowed = false;
	m_DownloadStarted = false;
	m_DownloadFinished = false;
	m_FinishedLoading = false;
	m_Lagging = false;
	m_DropVote = false;
	m_KickVote = false;
	m_Muted = false;
	m_LeftMessageSent = false;
	m_GProxy = false;
	m_GProxyDisconnectNoticeSent = false;
	m_GProxyReconnectKey = GetTicks( );
	m_LastGProxyAckTime = 0;
	m_TimeActive = 0;
	m_DownloadInfo = string();
	m_ScoreSet = false;
	m_RankSet = false;
	m_ScoreS = "0";
	m_RankS = "0";
	m_Censors = 0;
	m_Silence = false;
	m_DOTAKills = 0;
	m_DOTADeaths = 0;
	m_Provider =string();
	m_Country =string();
	m_ScoreSet = false;
	m_Switched = false;
	m_Switching = false;
	m_Switchok = false;
	m_WarnChecked = false;
}

CGamePlayer :: ~CGamePlayer( )
{

}

string CGamePlayer :: GetNameTerminated( )
{
	// if the player's name contains an unterminated colour code add the colour terminator to the end of their name
	// this is useful because it allows you to print the player's name in a longer message which doesn't colour all the subsequent text

	string LowerName = m_Name;
	transform( LowerName.begin( ), LowerName.end( ), LowerName.begin( ), (int(*)(int))tolower );
	string :: size_type Start = LowerName.find( "|c" );
	string :: size_type End = LowerName.find( "|r" );

	if( Start != string :: npos && ( End == string :: npos || End < Start ) )
		return m_Name + "|r";
	else
		return m_Name;
}

uint32_t CGamePlayer :: GetPing( bool LCPing )
{
	// just average all the pings in the vector, nothing fancy

	// fake pings if needed
	if (m_Game->m_GHost->ShouldFakePing(GetName()))
		return 24;

	if( m_Pings.empty( ) )
		return 0;

	uint32_t AvgPing = 0;

	for( unsigned int i = 0; i < m_Pings.size( ); i++ )
		AvgPing += m_Pings[i];

	AvgPing /= m_Pings.size( );

	if( LCPing )
		return AvgPing / 2;
	else
		return AvgPing;
}

string CGamePlayer :: GetProvider()
{
	if (m_Provider=="")
	{
		if (GetCountry()!="??" && GetCountry()!="Ga")
			m_Provider=m_Game->m_GHost->UDPChatWhoIs(GetCountry(), GetExternalIPString( ));
	}
	return m_Provider;
}

string CGamePlayer :: GetCountry()
{
	if (m_Country=="")
	{
		if (GetExternalIPString()=="127.0.0.1")
			m_Country = "Ga";
		else
			m_Country = m_Game->m_GHost->m_DBLocal->FromCheck( UTIL_ByteArrayToUInt32( GetExternalIP( ), true ) );
	}
	return m_Country;
}

/* string CGamePlayer :: GetCountryCity()
{
	if (m_CountryCity.empty())
	{
		if (GetExternalIPString()=="127.0.0.1")
			m_CountryCity = string();
		else{
			// CONSOLE_Print( "[DEBUG] ExternalIP " + UTIL_ToString( UTIL_ByteArrayToUInt32( GetExternalIP( ), true ) ) + " ExternalIP decimal " + UTIL_ByteArrayToDecString( GetExternalIP( ) ));
			m_CountryCity = m_Game->m_GHost->m_DBLocal->CountryCityCheck( UTIL_ByteArrayToUInt32( GetExternalIP( ), true ) );
		}
	}
	return m_CountryCity;
} */

bool CGamePlayer :: GetIsIgnoring( string username )
{
	transform( username.begin( ), username.end( ), username.begin( ), (int(*)(int))tolower );
	
	for( vector<string> :: iterator i = m_IgnoreList.begin( ); i != m_IgnoreList.end( ); ++i )
	{
		if( (*i) == username )
			return true;
	}
	
	return false;
}

void CGamePlayer :: Ignore( string username )
{
	if( GetIsIgnoring( username ) ) return;
	transform( username.begin( ), username.end( ), username.begin( ), (int(*)(int))tolower );
	
	m_IgnoreList.push_back( username );
}

void CGamePlayer :: UnIgnore( string username )
{
	transform( username.begin( ), username.end( ), username.begin( ), (int(*)(int))tolower );
	
	for( vector<string> :: iterator i = m_IgnoreList.begin( ); i != m_IgnoreList.end( ); )
	{
		if( (*i) == username )
		{
			m_IgnoreList.erase( i );
			continue;
		}
		
		++i;
	}
}

bool CGamePlayer :: Update( void *fd )
{
	// wait 4 seconds after joining before sending the /whois or /w
	// if we send the /whois too early battle.net may not have caught up with where the player is and return erroneous results

	if( m_WhoisShouldBeSent && !m_Spoofed && !m_WhoisSent && !m_JoinedRealm.empty( ) && GetTime( ) - m_JoinTime >= 4 )
	{
		// todotodo: we could get kicked from battle.net for sending a command with invalid characters, do some basic checking

		// check if we have this ip in our spoofed cached ip list
		bool isspoofed = m_Game->m_GHost->IsSpoofedIP(GetName(), GetExternalIPString());

		if (isspoofed)
		{
			CONSOLE_Print("[OPT] Player "+GetName()+" is in the cached spoof checked list");
			m_Game->AddToSpoofed(m_Game->m_Server, GetName(), false);
		}

		if (!isspoofed && GetExternalIPString()!="127.0.0.1")
		for( vector<CBNET *> :: iterator i = m_Game->m_GHost->m_BNETs.begin( ); i != m_Game->m_GHost->m_BNETs.end( ); i++ )
		{
			if( (*i)->GetServer( ) == m_JoinedRealm )
			{
				if( m_Game->GetGameState( ) == GAME_PUBLIC )
				{
					if ((*i)->GetWhereis() || (*i)->GetPasswordHashType( ) == "pvpgn" )
						(*i)->QueueChatCommand( "/whereis " + m_Name );
					else
						(*i)->QueueChatCommand( "/whois " + m_Name );
				}
				else if( m_Game->GetGameState( ) == GAME_PRIVATE )
					(*i)->QueueChatCommand( m_Game->m_GHost->m_Language->SpoofCheckByReplying( ), m_Name, true );
			}
		}

		m_WhoisSent = true;
	}

	// check for socket timeouts
	// if we don't receive anything from a player for 30 seconds we can assume they've dropped
	// this works because in the lobby we send pings every 5 seconds and expect a response to each one
	// and in the game the Warcraft 3 client sends keepalives frequently (at least once per second it looks like)

	if( m_Socket && GetTime( ) - m_Socket->GetLastRecv( ) >= 30 )
		m_Game->EventPlayerDisconnectTimedOut( this );
	// make sure we're not waiting too long for the first MAPSIZE packet
	
	if( m_ConnectionState == 1 && GetTicks( ) - m_ConnectionTime > m_Game->m_GHost->m_DenyMaxMapsizeTime && !m_Game->GetGameLoaded() && !m_Game->GetGameLoading() )
	{
		CONSOLE_Print( "[DENY] Kicking player: MAPSIZE not received within a few seconds" );
		m_DeleteMe = true;
		SetLeftReason( "MAPSIZE not received within a few seconds" );
		SetLeftCode( PLAYERLEAVE_LOBBY );
		m_Game->OpenSlot( m_Game->GetSIDFromPID( GetPID( ) ), false );
		m_Game->m_GHost->DenyIP( GetExternalIPString( ), m_Game->m_GHost->m_DenyMapsizeDuration, "MAPSIZE not received within a few seconds" );
	}
	
	// disconnect if the player is downloading too slowly
	
	if( m_DownloadStarted && !m_DownloadFinished && !m_Game->GetGameLoaded() && !m_Game->GetGameLoading() && GetLastMapPartSent( ) != 0 )
	{
		uint32_t downloadingTime = GetTicks( ) - m_StartedDownloadingTicks;
		uint32_t Seconds = downloadingTime / 1000;
		uint32_t Rate = GetLastMapPartSent( ) / 1024; // Rate in miliseconds
		if (Seconds>0)
			Rate /= Seconds;
		
		if(!(m_Game->IsSafe(GetName()) || m_Game->IsAdmin(GetName()) || m_Game->IsRootAdmin(GetName()) || m_Game->IsOwner(GetName()) )){			
		//	uint32_t iRate = (uint32_t)Rate;
			if ( m_Game->m_GHost->m_DLRateLimit != 0 )
			if ( GetLastMapPartSent( ) <= 102400 )
			if ( 0 < Rate && Rate < m_Game->m_GHost->m_DLRateLimit ){
				m_Game->DelTempOwner( GetName() );
				m_Game->SendAllChat( m_Game->m_GHost->m_Language->KickMsgForSlowDL( GetName( ) , UTIL_ToString( Rate ), UTIL_ToString(m_Game->m_GHost->m_DLRateLimit) ) );							
				m_Game->m_GHost->m_Callables.push_back( m_Game->m_GHost->m_DB->ThreadedBanAdd( GetJoinedRealm( ), GetName( ), GetExternalIPString(), m_Game->GetGameName( ), "AUTOBAN", "Slow internet", 1, 0 ));
				if ( m_Game->m_GHost->m_BlacklistSlowDLder && !m_Game->IsBlacklisted(m_Game->GetBannedNames( ),GetName( )) )
					m_Game->AddBannedNames(GetName( ));
				m_DeleteMe = true;				
				SetLeftReason( "kicked for downloading too slowly " + UTIL_ToString( Rate ) + " KB/s" );				
				SetLeftCode( PLAYERLEAVE_LOBBY );
				m_Game->OpenSlot( m_Game->GetSIDFromPID( GetPID( ) ), false );
			}
			
			if( downloadingTime > m_Game->m_GHost->m_DenyMaxDownloadTime )
			{
				m_Game->DelTempOwner( GetName() );
				m_Game->SendAllChat( m_Game->m_GHost->m_Language->KickMsgForSlowDL( GetName( ),"","" ) );
				m_DeleteMe = true;
				SetLeftReason( "download time is too long" );
				SetLeftCode( PLAYERLEAVE_LOBBY );
				m_Game->OpenSlot( m_Game->GetSIDFromPID( GetPID( ) ), false );
				m_Game->m_GHost->DenyIP( GetExternalIPString( ), m_Game->m_GHost->m_DenyDownloadDuration, "("+ GetName( ) +") rejected because download time is too long" );
			}
		}
	}
	
	// unmute player
	if( GetMuted( ) && ( GetTicks( ) - m_MutedTicks > 60000 || ( m_MutedAuto && GetTicks( ) - m_MutedTicks > 15000 ) ) )
	{
		SetMuted( false );
		m_Game->SendAllChat( "[" + m_Name + "] has been automatically unmuted. (Don't spam or you'll be muted again!)" );
		m_MuteMessages.clear( );
	}

	// GProxy++ acks

	if( m_GProxy && GetTime( ) - m_LastGProxyAckTime >= 10 )
	{
		if( m_Socket )
			m_Socket->PutBytes( m_Game->m_GHost->m_GPSProtocol->SEND_GPSS_ACK( m_TotalPacketsReceived ) );

		m_LastGProxyAckTime = GetTime( );
	}

	// base class update

	CPotentialPlayer :: Update( fd );
	bool Deleting;

	if( m_GProxy && m_Game->GetGameLoaded( ) )
		Deleting = m_DeleteMe || m_Error;
	else
		Deleting = m_DeleteMe || m_Error || m_Socket->HasError( ) || !m_Socket->GetConnected( );

	// try to find out why we're requesting deletion
	// in cases other than the ones covered here m_LeftReason should have been set when m_DeleteMe was set

	if( m_Error ){
		m_Game->m_GHost->DenyIP( GetExternalIPString( ), 180000, "player error" );
		m_Game->EventPlayerDisconnectPlayerError( this );
		m_Socket->Reset( );
		return Deleting;
	}
	if( m_Socket )
	{
		if( m_Socket->HasError( ) ){
			m_Game->EventPlayerDisconnectSocketError( this );
			if( !m_GProxy )
				m_Game->m_GHost->DenyIP( GetExternalIPString( ), 20000, "socket error" );
		}
		else if( !m_Socket->GetConnected( ) ){
			m_Game->EventPlayerDisconnectConnectionClosed( this );
			if( !m_GProxy )
				m_Game->m_GHost->DenyIP( GetExternalIPString( ), 30000, "connection closed" );
		}
	}

	return Deleting;
}

void CGamePlayer :: ExtractPackets( )
{
	if( !m_Socket )
		return;

	// extract as many packets as possible from the socket's receive buffer and put them in the m_Packets queue

	string *RecvBuffer = m_Socket->GetBytes( );
	BYTEARRAY Bytes = UTIL_CreateByteArray( (unsigned char *)RecvBuffer->c_str( ), RecvBuffer->size( ) );

	// a packet is at least 4 bytes so loop as long as the buffer contains 4 bytes

	while( Bytes.size( ) >= 4 )
	{
		if( Bytes[0] == W3GS_HEADER_CONSTANT || Bytes[0] == GPS_HEADER_CONSTANT || Bytes[0] == GCBI_HEADER_CONSTANT )
		{
			// bytes 2 and 3 contain the length of the packet

			uint16_t Length = UTIL_ByteArrayToUInt16( Bytes, false, 2 );

			if( Length >= 4 )
			{
				if( Bytes.size( ) >= Length )
				{
					m_Packets.push( new CCommandPacket( Bytes[0], Bytes[1], BYTEARRAY( Bytes.begin( ), Bytes.begin( ) + Length ) ) );

					if( Bytes[0] == W3GS_HEADER_CONSTANT )
						m_TotalPacketsReceived++;

					*RecvBuffer = RecvBuffer->substr( Length );
					Bytes = BYTEARRAY( Bytes.begin( ) + Length, Bytes.end( ) );
				}
				else
					return;
			}
			else
			{
				m_Error = true;
				m_ErrorString = "received invalid packet from player (bad length)";
				return;
			}
		}
		else
		{
			m_Error = true;
			m_ErrorString = "received invalid packet from player (bad header constant)";
			return;
		}
	}
}

void CGamePlayer :: ProcessPackets( )
{
	if( !m_Socket )
		return;

	CIncomingAction *Action = NULL;
	CIncomingChatPlayer *ChatPlayer = NULL;
	CIncomingMapSize *MapSize = NULL;
	uint32_t CheckSum = 0;
	uint32_t Pong = 0;

	// process all the received packets in the m_Packets queue

	while( !m_Packets.empty( ) )
	{
		CCommandPacket *Packet = m_Packets.front( );
		m_Packets.pop( );

		if( Packet->GetPacketType( ) == W3GS_HEADER_CONSTANT )
		{
			switch( Packet->GetID( ) )
			{
			case CGameProtocol :: W3GS_LEAVEGAME:
				m_Game->EventPlayerLeft( this, m_Protocol->RECEIVE_W3GS_LEAVEGAME( Packet->GetData( ) ) );
				break;

			case CGameProtocol :: W3GS_GAMELOADED_SELF:
				if( m_Protocol->RECEIVE_W3GS_GAMELOADED_SELF( Packet->GetData( ) ) )
				{
					if( !m_FinishedLoading && m_Game->GetGameLoading( ) )
					{
						m_FinishedLoading = true;
						m_FinishedLoadingTicks = GetTicks( );
						m_Game->EventPlayerLoaded( this );
					}
					else
					{
						// we received two W3GS_GAMELOADED_SELF packets from this player!
					}
				}

				break;

			case CGameProtocol :: W3GS_OUTGOING_ACTION:
				Action = m_Protocol->RECEIVE_W3GS_OUTGOING_ACTION( Packet->GetData( ), m_PID );

				if( Action )
					m_Game->EventPlayerAction( this, Action );

				// don't delete Action here because the game is going to store it in a queue and delete it later

				break;

			case CGameProtocol :: W3GS_OUTGOING_KEEPALIVE:
				CheckSum = m_Protocol->RECEIVE_W3GS_OUTGOING_KEEPALIVE( Packet->GetData( ) );
				m_CheckSums.push( CheckSum );
				m_SyncCounter++;
				m_Game->EventPlayerKeepAlive( this, CheckSum );
				break;

			case CGameProtocol :: W3GS_CHAT_TO_HOST:
				ChatPlayer = m_Protocol->RECEIVE_W3GS_CHAT_TO_HOST( Packet->GetData( ) );

				if( ChatPlayer )
				{
					// determine if we should auto-mute this player
					if( ChatPlayer->GetType( ) == CIncomingChatPlayer :: CTH_MESSAGE || ChatPlayer->GetType( ) == CIncomingChatPlayer :: CTH_MESSAGEEXTRA )
					{
						m_MuteMessages.push_back( GetTicks( ) );
					
						if( m_MuteMessages.size( ) > 7 )
							m_MuteMessages.erase( m_MuteMessages.begin( ) );
					
						uint32_t RecentCount = 0;

						for( unsigned int i = 0; i < m_MuteMessages.size( ); ++i )
						{
							if( GetTicks( ) - m_MuteMessages[i] < 7000 )
								RecentCount++;
						}
					
						if( !GetMuted( ) && RecentCount >= 7 )
						{
							SetMuted( true );
							m_MutedAuto = true;
							m_Game->SendAllChat( "[" + m_Name + "] has been automatically muted for spamming. (You will be unmuted momentarily, but please do not spam again!)" );
							m_MuteMessages.clear( );
						}
					
						//now check for flamers
						if( m_Game->m_GHost->FlameCheck( ChatPlayer->GetMessage( ) ) )
						{
							m_FlameMessages.push_back( GetTicks( ) );
					
							if( m_FlameMessages.size( ) > 10 )
								m_FlameMessages.erase( m_FlameMessages.begin( ) );
					
							RecentCount = 0;

							for( unsigned int i = 0; i < m_FlameMessages.size( ); ++i )
							{
								if( GetTicks( ) - m_FlameMessages[i] < 80000 )
									RecentCount++;
							}
					
							if( RecentCount >= 4 )
							{
								m_Game->SendAllChat( "Use "+string(1, m_Game->m_GHost->m_CommandTrigger)+"ignore <playername> to ignore players (for example, if they are flaming); partial names work. Don't flame back!" );
								m_FlameMessages.clear( );
							}
							else if( RecentCount >= 3 )
							{
				            	m_Game->SendAllChat( "[Calm] has refilled [" + GetName() + "]'s cookie jar. [" + GetName() + "] now has three cookies (try "+string(1, m_Game->m_GHost->m_CommandTrigger)+"eat)!");
				            	SetCookies(3);
							}
						}
					}
					
					m_Game->EventPlayerChatToHost( this, ChatPlayer );
				}

				delete ChatPlayer;
				ChatPlayer = NULL;
				break;

			case CGameProtocol :: W3GS_DROPREQ:
				// todotodo: no idea what's in this packet

				if( !m_DropVote )
				{
					m_DropVote = true;
					m_Game->EventPlayerDropRequest( this );
				}

				break;

			case CGameProtocol :: W3GS_MAPSIZE:
				m_ConnectionState = 2;
				m_ConnectionTime = GetTicks( );
				MapSize = m_Protocol->RECEIVE_W3GS_MAPSIZE( Packet->GetData( ), m_Game->m_GHost->m_Map->GetMapSize( ) );

				if( MapSize )
					m_Game->EventPlayerMapSize( this, MapSize );

				delete MapSize;
				MapSize = NULL;
				break;

			case CGameProtocol :: W3GS_PONG_TO_HOST:
				Pong = m_Protocol->RECEIVE_W3GS_PONG_TO_HOST( Packet->GetData( ) );



				// we discard pong values of 1
				// the client sends one of these when connecting plus we return 1 on error to kill two birds with one stone

				if( Pong != 1 )
				{
					// we also discard pong values when we're downloading because they're almost certainly inaccurate
					// this statement also gives the player a 5 second grace period after downloading the map to allow queued (i.e. delayed) ping packets to be ignored

					if( !m_DownloadStarted || ( m_DownloadFinished && GetTime( ) - m_FinishedDownloadingTime >= 5 ) )
					{
						// we also discard pong values when anyone else is downloading if we're configured to

						if( m_Game->m_GHost->m_PingDuringDownloads || !m_Game->IsDownloading( ) )
						{
							m_Pings.push_back( GetTicks( ) - Pong );

							if( m_Pings.size( ) > 20 )
								m_Pings.erase( m_Pings.begin( ) );
						}
					}
				}

				m_Game->EventPlayerPongToHost( this, Pong );
				break;
			}
		}
		else if( Packet->GetPacketType( ) == GPS_HEADER_CONSTANT )
		{
			BYTEARRAY Data = Packet->GetData( );

			if( Packet->GetID( ) == CGPSProtocol :: GPS_INIT )
			{
				if( m_Game->m_GHost->m_Reconnect )
				{
					m_GProxy = true;
					m_Socket->PutBytes( m_Game->m_GHost->m_GPSProtocol->SEND_GPSS_INIT( m_Game->m_GHost->m_ReconnectPort, m_PID, m_GProxyReconnectKey, m_Game->GetGProxyEmptyActions( ) ) );
					CONSOLE_Print( "[GAME: " + m_Game->GetGameName( ) + "] player [" + m_Name + "] is using GProxy++" );
				}
				else
				{
					// todotodo: send notice that we're not permitting reconnects
					// note: GProxy++ should never send a GPS_INIT message if bot_reconnect = 0 because we don't advertise the game with invalid map dimensions
					// but it would be nice to cover this case anyway
				}
			}
			else if( Packet->GetID( ) == CGPSProtocol :: GPS_RECONNECT )
			{
				// this is handled in ghost.cpp
			}
			else if( Packet->GetID( ) == CGPSProtocol :: GPS_ACK && Data.size( ) == 8 )
			{
				uint32_t LastPacket = UTIL_ByteArrayToUInt32( Data, false, 4 );
				uint32_t PacketsAlreadyUnqueued = m_TotalPacketsSent - m_GProxyBuffer.size( );

				if( LastPacket > PacketsAlreadyUnqueued )
				{
					uint32_t PacketsToUnqueue = LastPacket - PacketsAlreadyUnqueued;

					if( PacketsToUnqueue > m_GProxyBuffer.size( ) )
						PacketsToUnqueue = m_GProxyBuffer.size( );

					while( PacketsToUnqueue > 0 )
					{
						m_GProxyBuffer.pop( );
						PacketsToUnqueue--;
					}
				}
			}
		}

		delete Packet;
	}
}

void CGamePlayer :: Send( BYTEARRAY data )
{
	// must start counting packet total from beginning of connection
	// but we can avoid buffering packets until we know the client is using GProxy++ since that'll be determined before the game starts
	// this prevents us from buffering packets for non-GProxy++ clients

	m_TotalPacketsSent++;

	if( m_GProxy && m_Game->GetGameLoaded( ) ){
		if( m_GProxyBuffer.size( ) <= 2000 )
			m_GProxyBuffer.push( data );
		else CONSOLE_Print( "[DENY] Warning: discarding GProxy++ packet from player [" + m_Name + "] due to large queue size (> 2000)" );
	}
	CPotentialPlayer :: Send( data );
}

void CGamePlayer :: EventGProxyReconnect( CTCPSocket *NewSocket, uint32_t LastPacket )
{
	delete m_Socket;
	m_Socket = NewSocket;
	m_Socket->PutBytes( m_Game->m_GHost->m_GPSProtocol->SEND_GPSS_RECONNECT( m_TotalPacketsReceived ) );

	uint32_t PacketsAlreadyUnqueued = m_TotalPacketsSent - m_GProxyBuffer.size( );

	if( LastPacket > PacketsAlreadyUnqueued )
	{
		uint32_t PacketsToUnqueue = LastPacket - PacketsAlreadyUnqueued;

		if( PacketsToUnqueue > m_GProxyBuffer.size( ) )
			PacketsToUnqueue = m_GProxyBuffer.size( );

		while( PacketsToUnqueue > 0 )
		{
			m_GProxyBuffer.pop( );
			PacketsToUnqueue--;
		}
	}

	// send remaining packets from buffer, preserve buffer

	queue<BYTEARRAY> TempBuffer;

	while( !m_GProxyBuffer.empty( ) )
	{
		m_Socket->PutBytes( m_GProxyBuffer.front( ) );
		TempBuffer.push( m_GProxyBuffer.front( ) );
		m_GProxyBuffer.pop( );
	}

	m_GProxyBuffer = TempBuffer;
	m_GProxyDisconnectNoticeSent = false;
	m_Game->SendAllChat( m_Game->m_GHost->m_Language->PlayerReconnectedWithGProxy( m_Name ) );
	m_CachedIP = m_Socket->GetIPString( );
}
