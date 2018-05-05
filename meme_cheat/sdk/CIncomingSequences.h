#pragma once

namespace sdk {
	struct CIncomingSequence {
		CIncomingSequence::CIncomingSequence( int instate, int outstate, int seqnr, float time ) {
			inreliablestate = instate;
			outreliablestate = outstate;
			sequencenr = seqnr;
			curtime = time;
		}
		int inreliablestate;
		int outreliablestate;
		int sequencenr;
		float curtime;
	};
}