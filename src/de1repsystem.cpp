#include <de1repsystem.hpp>
#include <time.h>

void de1repsystem::createtoken(const symbol& sym) // symbol input type : "4,EOS"
{
  require_auth("repsyscoracc"_n);
  eosio::check(sym.is_valid(), "Invalid symbol name");
  eosio::check(sym.precision() == 4, "symbol precision should be 4");
  
  tokens tokenTable(get_self(), get_self().value);
  auto existing = tokenTable.find(sym.code().raw());
  eosio::check(existing == tokenTable.end(), "token with symbol already exists");
  
  auto ramPayer = "repsyscoracc"_n;
  
  tokenTable.emplace(ramPayer, [&](auto& tokenRow) {
    tokenRow.tokenSym = sym;
  });
}

void de1repsystem::rate(const uint64_t& ratee,
                        const uint64_t& rater,
                        const asset& rating,
                        const uint64_t& reviewId) 
{
  // require auth of target corporation (in this example, testaccount1)
  require_auth("repsyscoracc"_n);
  
  // ratee & rater existance check
  // * Should not be zero, because NULL = 0
  eosio::check(ratee != NULL, "Insert ratee input, not zero");
  eosio::check(rater != NULL, "Insert rater input, not zero");
  
  // symbol validity check
  auto sym = rating.symbol;
  eosio::check(sym.is_valid(), "Invalid symbol name");
  eosio::check(sym.precision() == 4, "should be 4 precision");
  
  // symbol existance check 
  tokens tokenTable(get_self(), get_self().value);
  auto existing = tokenTable.find(rating.symbol.code().raw());
  eosio::check(existing != tokenTable.end(), "Token doesn't exist. Check token symbol");
  
  // 5 star rating check
  eosio::check(rating.amount >= 0 && rating.amount <= 50000, "Rating should be between 0 and 5");
  
  auto ramPayer = "repsyscoracc"_n;
  appendRating(ratee, rater, rating, ramPayer);
  
  // auto epochtime = now();
  // tm = gmtime(&epochtime);
  // print("timestamp : ", std::asctime(std::localtime(&epochtime)));
  // print("epoch timestamp in sec  : ", now());
  // float tfp = (0.000032);
  // print(tfp);
}

void de1repsystem::appendRating(const uint64_t& ratee,
                                const uint64_t& rater,
                                const asset& rating,
                                const name& ramPayer)
{
  // ratee table update
  ratees rateeTable(get_self(), ratee);
  auto itrRepToken = rateeTable.find(rating.symbol.code().raw());
  if(itrRepToken == rateeTable.end())
  {
    rateeTable.emplace(ramPayer, [&](auto& repRow) {
      repRow.sumRep = rating;
      repRow.numOfReviewer = 1;
      repRow.avgRep = repRow.sumRep/repRow.numOfReviewer;
      
    });
  }
  else
  {
    rateeTable.modify(itrRepToken, ramPayer, [&](auto& repRow) {
      repRow.sumRep += rating;
      repRow.numOfReviewer += 1;
      repRow.avgRep = repRow.sumRep/repRow.numOfReviewer;
      
    });
  }
  
  // rater table update
  raters raterTable(get_self(), rater);
  auto itrRepToken2 = raterTable.find(rating.symbol.code().raw());
  if(itrRepToken2 == raterTable.end())
  {
    raterTable.emplace(ramPayer, [&](auto& repRow2) {
      repRow2.sumRep = rating;
      repRow2.numOfReview = 1;
      repRow2.avgRep = repRow2.sumRep/repRow2.numOfReview;
      repRow2.relWeight = 0.000001;
    });
  }
  else
  {
    raterTable.modify(itrRepToken2, ramPayer, [&](auto& repRow2) {
      repRow2.sumRep += rating;
      repRow2.numOfReview += 1;
      repRow2.avgRep = repRow2.sumRep/repRow2.numOfReview;
      repRow2.relWeight += 0.000001;
    });
  }
}

// timestamp UTC로 출력하여 저장하기.
