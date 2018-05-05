#include "fakelatency.h"

int __fastcall HOOKS::hkSendDatagram( sdk::NetChannel* netchan, void* edx, void* datagram ) {
	if ( !ctx::engine->IsInGame( ) || !ctx::engine->IsConnected( ) || datagram )
		return HOOKS::original_send_datagram( netchan, datagram );

	if ( !Vars::options.fakelatency )
		return HOOKS::original_send_datagram( netchan, datagram );

	g_FakeLatency->m_netchan = netchan;

	auto instate = netchan->m_nInReliableState;
	auto in_sequencenr = netchan->m_nInSequenceNr;

	auto lag_s = Vars::options.fakelatency_amt / 1000.f;
	auto lag_delta = lag_s - ctx::engine->GetNetChannelInfo( )->GetLatency( FLOW_OUTGOING );

	g_FakeLatency->AddLatency( netchan, lag_delta );

	int ret = HOOKS::original_send_datagram( netchan, datagram );

	netchan->m_nInReliableState = instate;
	netchan->m_nInSequenceNr = in_sequencenr;

	return ret;
}