// ver 1.0 : Only permitted corperation to execute rate transaction
// for recording all the ratings on EOS blockchain.
//
// Corperation treat all fees for Txs and storage. (CPU/NET/RAM)
//
// Integrity of rating between web and blockchain can be secured
// additional information such as Tx ID, timestamp, etc.(memo form)

#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/symbol.hpp>
#include <eosio/system.hpp>

#include <string>
#include <ctime>
#include <time.h>

using std::string;

using namespace eosio;

CONTRACT de1repsystem : public contract {
  public:
    using contract::contract;
    de1repsystem(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds) {}

    [[eosio::action]]
    void rate(const uint64_t& ratee,
              const uint64_t& rater,
              const asset& rating,
              const uint64_t& reviewId);
              
    [[eosio::action]]
    void createtoken(const symbol& sym);
  
  private:
    struct [[eosio::table]] token {
      symbol tokenSym;
      
      auto primary_key() const { return tokenSym.code().raw(); }
    };
    struct [[eosio::table]] ratee {
      // uint64_t ratee;
      asset sumRep;
      int numOfReviewer;
      asset avgRep;
      
      auto primary_key() const { return sumRep.symbol.code().raw(); }
    };
    
    struct [[eosio::table]] rater {
      asset sumRep;
      int numOfReview;
      asset avgRep;
      double relWeight;
      
      auto primary_key() const { return sumRep.symbol.code().raw(); }
    };
    typedef eosio::multi_index<"ratees"_n, ratee> ratees;
    typedef eosio::multi_index<"raters"_n, rater> raters;
    typedef eosio::multi_index<"tokens"_n, token> tokens;
    
    void appendRating(const uint64_t& ratee, const uint64_t& rater, const asset& rating, const name& ramPayer);
    uint32_t now(){return current_time_point().sec_since_epoch();}
};

EOSIO_DISPATCH(de1repsystem, (rate) (createtoken))
