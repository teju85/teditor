#pragma once

#include <utility>
#include <set>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>


namespace teditor {
namespace ledger {

/** all aliases of a given Account */
typedef std::set<std::string> Aliases;

/** Represents an account in the accounting world */
class Account {
public:
  /**
   * @brief ctor
   * @param n name of the account to be created
   * @param d short description of the account
   * @param a list of aliases associated with this account
   */
  Account(const std::string& n, const std::string& d="",
          const Aliases& a=Aliases()): nam(n), descr(d), alias(a), bal(0) {}

  const std::string& name() const { return nam; }
  const std::string& desc() const { return descr; }
  int64_t rawBalance() const { return bal; }
  double balance() const { return rawBalance() / 100.0; }

  bool isAnAlias(const std::string& a) const {
    return alias.find(a) != alias.end();
  }
  void insertAlias(const std::string& a) { alias.insert(a); }

  Account& operator+=(double val) {
    bal += (int64_t)(val * 100.0);
    return *this;
  }
  Account& operator+=(int64_t val) { bal += val; return *this; }

private:
  std::string nam, descr;
  Aliases alias;
  int64_t bal;
};

/** all accounts in the current ledger */
class Accounts: public std::vector<Account> {
public:
  Account& find(const std::string& a) {
    for(size_t i=0;i<size();++i) {
      auto& itr = at(i);
      if(itr.name() == a) return itr;
      if(itr.isAnAlias(a)) return itr;
    }
    push_back(Account(a));
    return back();
  }
};

/** date of transaction */
struct Date {
  Date(const std::string& str): year(0), month(0), day(0) {
    sscanf(str.c_str(), "%d/%d/%d", &year, &month, &day);
  }

  bool validate() const {
    if(year <= 0) return false;
    if(month <= 0 || month >= 13) return false;
    if(day <= 0) return false;
    if((month % 2 && month <= 7) || (!(month % 2) && month >= 8))
      return day <= 31;
    else if(month == 2) {
      if(year % 4 == 0) return day <= 29;
      return day <= 28;
    } else return day <= 30;
  }

  bool operator<(const Date& d) const {
    if(year < d.year) return true;
    if(year > d.year) return false;
    if(month < d.month) return true;
    if(month > d.month) return false;
    return day < d.day;
  }

  bool operator==(const Date& d) const {
    return year == d.year && month == d.month && day == d.day;
  }

  bool operator<=(const Date& d) const {
    return *this < d || *this == d;
  }

  int year, month, day;
};

/** account related info in a transaction */
typedef std::pair<std::string, int64_t> AccInfo;

/** all accounts involved in a transaction */
typedef std::vector<AccInfo> AccountsInfo;

/** Represents a transaction in the ledger */
class Transaction {
public:
  Transaction(const std::string& dateStr, const std::string& n):
    nam(n), dat(dateStr), accts() {}

  void add(const std::string& acct, double val) {
    accts.push_back(AccInfo(acct, int64_t(val * 100.0)));
  }

  /** default debit account which is the remaining of all the other credits */
  void add(const std::string& acct) {
    int64_t val = 0;
    for(const auto& a : accts) val += a.second;
    accts.push_back(AccInfo(acct, -val));
  }

  void updateAccounts(Accounts& acc) {
    for(const auto& a : accts) acc.find(a.first) += a.second;
  }

  const std::string& name() const { return nam; }
  const Date& date() const { return dat; }

  /** total of all credits/debits. On a correct transaction, this must be 0 */
  int64_t rawBalance() const {
    int64_t val = 0;
    for(const auto& a : accts) val += a.second;
    return val;
  }

  double balance() const { return rawBalance() / 100.0; }

private:
  std::string nam;
  Date dat;
  AccountsInfo accts;
};

typedef std::vector<Transaction> Transactions;

} // end namespace ledger
} // end namespace teditor
