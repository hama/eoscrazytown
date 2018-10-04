#include "eoscrazytown.hpp"

auto eoscrazytown::checkBets( const asset eos, const string memo,
                              vector<int64_t> &vbets, int64_t &totalBets  ) {  // check eos.amount == total bets
    vbets = getBets( memo, ' ' ) ;
    totalBets = getTotalBets( vbets ) ;    
    return eos.amount == totalBets ;
}

// input
void eoscrazytown::onTransfer(account_name from, account_name to, asset eos, string memo) {        
    if (to != _self) return ;

    require_auth(from);
    eosio_assert(eos.is_valid(), "Invalid token transfer");
    eosio_assert(eos.symbol == EOS_SYMBOL, "only EOS token is allowed");
    eosio_assert(eos.amount > 0, "must bet a positive amount");

    // todo: input check non-num
    if ( memo != "" && memo.size() > 11 ) { // need protect
        vector<int64_t> vbets ;
        int64_t totalBets = 0 ;
        eosio_assert( checkBets( eos, memo, vbets, totalBets ), "Bets not equal to amount.");
        auto itr = players.find(from);
        if (itr == players.end()) {
            players.emplace(_self, [&](auto& p) {
                p.account = from;
                p.bets = memo;
                p.vbets = vbets ;
                p.quantity = eos;
            });
        } else {
            eosio_assert( false, "Already bet.");
            return ;
            /*
            player.modify(itr, 0, [&](auto &p) {
                
                p.quantity += eos;
            });*/
        }
/*
        global.modify(global.begin(), 0, [&](auto &g) {
            g.reserve += eos;
        });*/
    
    }
      
    
}

auto eoscrazytown::getResult( const card a,  const card b ) { // todo: fix to new type form
    string result = "" ;
    if ( a.points > b.points ) result += "a" ; // (1)
    else if ( b.points > a.points ) result += "b" ; // (2)
    else result += "x" ; // (3)

    if ( a.suit ==  HEART || a.suit == DIAMOND ) result += "aR" ; // (4) red
    else result += "aB" ; // (6)

    if ( b.suit ==  HEART || b.suit == DIAMOND ) result += "bR" ; // (5) red
    else result += "bB" ; // (7)

    if ( ( a.points & 1 ) == 1 ) result += "aO" ; // (8) odd
    else result += "aE" ; // (9)

    if ( ( b.points & 1 ) == 1 ) result += "bO" ; // (10) odd
    else result += "bE" ; // (11)

    return result ;
}

const vector<int64_t> eoscrazytown::getBets(const string& s, const char& c) { // need protect
    vector<int64_t> vbets;
    auto vs = explode( s, c ) ;
    for ( auto n:vs ) {
        vbets.push_back( (int64_t)string_to_price( n ) ) ;
    }

    return vbets ;
}

auto eoscrazytown::getBeton( const vector<int64_t> v ) {
    string beton = "" ;

    for(auto n:v) {
        if ( n != 0 ) beton+='O';
        else beton+='X' ;
    }
    
    return beton;
}

const int64_t eoscrazytown::getTotalBets(const vector<int64_t> v) {
    int64_t totalBets = 0 ;
    for (auto n:v) totalBets += n ;

    return totalBets ;
}


/*
void eoscrazytown::give_out_bonus ( const string result ) {
 
}*/


void eoscrazytown::reveal() {
    require_auth(_self);

    auto a = global.begin()->a ;
    auto b = global.begin()->b ;
    auto result = getResult( a, b ) ;
    
    string beton ;
    // string presult ;
    vector<int64_t> bets ;
    int64_t bonus ;
    for ( auto p = begin(players) ; p != end(players) ; ++p ) {
        beton = getBeton( p->vbets ) ;
        bets = p->vbets ;
        bonus = 0 ;
        // exp:
        // r:     xaRbBaObE
        // new r: O X X X X X X X X X X // no space !
        // beton: O X X O X X O X O O O // no space !
        if ( result[0] == 'x') { // draw
            // presult += 'x' ; 

            return ;
        }
        else { 
            if ( beton[0] == result[0] )
                ;// presult += 'w' ; // win
            else
                ;// presult += 'l' ; // lose
        }

        if ( result[3] == beton[3] ) bonus += bets[3] + bets[3] * COLOR ; // (4)
        if ( result[4] == beton[4] ) bonus += bets[4] + bets[4] * COLOR ; // (5)

        if ( result[5] == beton[5] ) bonus += bets[5] + bets[5] * COLOR ; // (6)
        if ( result[6] == beton[6] ) bonus += bets[6] + bets[6] * COLOR ; // (7)

        if ( result[7] == beton[7] )  bonus += bets[7] + bets[7] * ODD ; // (8)
        if ( result[8] == beton[8] )  bonus += bets[8] + bets[8] * EVEN ; // (9)

        if ( result[9] == beton[9] )  bonus += bets[9] + bets[9] * ODD ; // (10)
        if ( result[10] == beton[10] )  bonus += bets[10] + bets[10] * EVEN ; // (11)

        
    }

}