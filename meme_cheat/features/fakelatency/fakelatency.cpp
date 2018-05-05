#include "fakelatency.h"


CFakeLatency* g_FakeLatency = new CFakeLatency;
void CFakeLatency::AddLatency( sdk::NetChannel * netchan, float latency ) {
	for ( auto& seq : m_sequences ) {
		if ( ctx::globals->realtime - seq.realtime >= latency ) {
			netchan->m_nInReliableState = seq.in_reliable_state;
			netchan->m_nInSequenceNr = seq.in_sequence_nr;
			break;
		}
	}
}

void CFakeLatency::UpdateIncomingSequences( ClientFrameStage_t curStage ) {
	if ( curStage != FRAME_NET_UPDATE_POSTDATAUPDATE_START )
		return;

	if ( !( *ctx::client_state ) || !( *ctx::client_state )->m_NetChannel )
		return;

	if ( ( *ctx::client_state )->m_NetChannel->m_nInSequenceNr > lastincomingsequencenumber ) {
		lastincomingsequencenumber = ( *ctx::client_state )->m_NetChannel->m_nInSequenceNr;

		m_sequences.push_front( CIncomingSequence( ( *ctx::client_state )->m_NetChannel->m_nInReliableState, ( *ctx::client_state )->m_NetChannel->m_nOutReliableState, ( *ctx::client_state )->m_NetChannel->m_nInSequenceNr, ctx::globals->realtime ) );
	}

	if ( m_sequences.size( ) > 2048 )
		m_sequences.pop_back( );
}
int CFakeLatency::SendDatagram( sdk::NetChannel* netchan, void* datagram ) {
	if ( !ctx::engine->IsInGame( ) || !ctx::engine->IsConnected( ) || datagram )
		return HOOKS::original_send_datagram( netchan, datagram );

	if ( !Vars::options.fakelatency )
		return HOOKS::original_send_datagram( netchan, datagram );

	auto instate = netchan->m_nInReliableState;
	auto in_sequencenr = netchan->m_nInSequenceNr;

	AddLatency( netchan, Vars::options.fakelatency_amt );

	int ret = HOOKS::original_send_datagram( netchan, datagram );

	netchan->m_nInReliableState = instate;
	netchan->m_nInSequenceNr = in_sequencenr;

	return ret;
}