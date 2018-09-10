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
        IBAN(addr, IBANType::Token).to_string(),
        std::string("BX90 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF ZBV3"));
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
    IBAN iban("BX90 E4RK QV4E YF6M XKCE 5ND8 L6VP 67FF ZBV3", IBANType::Token);
    TS_ASSERT_EQUALS(
        iban.as_addr(),
        Address::from_hex("269e974f44b178baa444db07e5726de74a5b8679"));
  }

  void testBandToken()
  {
    TS_ASSERT_EQUALS(
        IBAN(Address(), IBANType::Token).to_string(),
        std::string("BX63 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA"));
    TS_ASSERT_EQUALS(
        IBAN("BX63 AAAA AAAA AAAA AAAA AAAA AAAA AAAA AAAA", IBANType::Token)
            .as_addr(),
        Address());
  }
};
