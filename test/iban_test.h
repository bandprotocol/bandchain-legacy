#include <cxxtest/TestSuite.h>
#include <vector>

#include "inc/essential.h"
#include "util/iban.h"

class IBANTest : public CxxTest::TestSuite
{
public:
  void testParseFromAddress()
  {
    auto addr = Address::from_hex("269e974f44b178baa444db07e5726de74a5b8679");
    TS_ASSERT_EQUALS(
        IBAN(addr, IBANType::Account).to_string(),
        std::string("AX02 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF ZBV3"));
    TS_ASSERT_EQUALS(
        IBAN(addr, IBANType::Contract).to_string(),
        std::string("CX81 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF ZBV3"));
    addr = Address::from_hex("0000000000000000000000000000000000000000");
    TS_ASSERT_EQUALS(
        IBAN(addr, IBANType::Revenue).to_string(),
        std::string("RX16 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA"));
  }

  void testParseAccountFromIBAN()
  {
    IBAN iban("AX02 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF ZBV3",
              IBANType::Account);
    TS_ASSERT_EQUALS(
        iban.as_addr(),
        Address::from_hex("269e974f44b178baa444db07e5726de74a5b8679"));
  }

  void testParseContractFromIBAN()
  {
    IBAN iban("CX81 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF ZBV3",
              IBANType::Contract);
    TS_ASSERT_EQUALS(
        iban.as_addr(),
        Address::from_hex("269e974f44b178baa444db07e5726de74a5b8679"));
  }

  void testBandToken()
  {
    TS_ASSERT_EQUALS(
        IBAN(Address(), IBANType::Contract).to_string(),
        std::string("CX54 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA"));
    TS_ASSERT_EQUALS(
        IBAN("CX54 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA", IBANType::Contract)
            .as_addr(),
        Address());
    TS_ASSERT_EQUALS(
        IBAN("RX70 RRRR RRRR RRRR RRRR RRRR RRRR RRRR RRRR", IBANType::Revenue)
            .as_addr(),
        Address::from_hex("7bdef7bdef7bdef7bdef7bdef7bdef7bdef7bdef"));
    TS_ASSERT_EQUALS(
        IBAN("CX09 CCCC CCCC CCCC CCCC CCCC CCCC CCCC CCCC", IBANType::Contract)
            .as_addr(),
        Address::from_hex("1084210842108421084210842108421084210842"));
    TS_ASSERT_EQUALS(
        IBAN("AX72 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA", IBANType::Account)
            .as_addr(),
        Address::from_hex("0000000000000000000000000000000000000000"));

    TS_ASSERT_EQUALS(
        IBAN("SX87 SSSS SSSS SSSS SSSS SSSS SSSS SSSS SSSS", IBANType::Stake)
            .as_addr(),
        Address::from_hex("8421084210842108421084210842108421084210"));
  }
};
