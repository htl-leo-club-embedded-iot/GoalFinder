#include <unity.h>

#include <system/Settings.h>

#include "../../src/Singleton.h"

class ExampleSingleton : public Singleton<ExampleSingleton> {
    friend class Singleton<ExampleSingleton>;

  public:
    int value;

  private:
    ExampleSingleton() : value(7) {}
};

void setUp(void) {}

void tearDown(void) {}

void test_singleton_returns_same_instance(void) {
    ExampleSingleton *first = ExampleSingleton::GetInstance();
    ExampleSingleton *second = ExampleSingleton::GetInstance();

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_EQUAL_PTR(first, second);
    TEST_ASSERT_EQUAL(7, first->value);

    first->value = 42;
    TEST_ASSERT_EQUAL(42, second->value);
}

void test_dummy_settings_api_surface(void) {
    System::Settings settings;

    TEST_ASSERT_FALSE(settings.Begin("test_namespace", false, nullptr));

    TEST_ASSERT_FALSE(settings.Clear());
    TEST_ASSERT_FALSE(settings.Remove("missing_key"));

    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutChar("k", static_cast<int8_t>('a'))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutUChar("k", static_cast<uint8_t>('a'))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutShort("k", static_cast<int16_t>(1))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutUShort("k", static_cast<uint16_t>(1))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutInt("k", static_cast<int32_t>(1))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutUInt("k", static_cast<uint32_t>(1))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutLong("k", static_cast<int32_t>(1))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutULong("k", static_cast<uint32_t>(1))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutLong64("k", static_cast<int64_t>(1))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutULong64("k", static_cast<uint64_t>(1))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutFloat("k", 1.5F)));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutDouble("k", 2.5)));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutBool("k", true)));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutString("k", "value")));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutString("k", String("value"))));

    int payload = 7;
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.PutBytes("k", &payload, sizeof(payload))));

    TEST_ASSERT_FALSE(settings.IsKey("k"));
    TEST_ASSERT_EQUAL(System::PT_INVALID, settings.GetType("k"));

    TEST_ASSERT_EQUAL_CHAR('0', settings.GetChar("k", 'x'));
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>('0'), settings.GetUChar("k", static_cast<uint8_t>('x')));
    TEST_ASSERT_EQUAL_INT16(0, settings.GetShort("k", static_cast<int16_t>(9)));
    TEST_ASSERT_EQUAL_UINT16(0u, settings.GetUShort("k", static_cast<uint16_t>(9)));
    TEST_ASSERT_EQUAL_INT32(0, settings.GetInt("k", static_cast<int32_t>(9)));
    TEST_ASSERT_EQUAL_UINT32(0u, settings.GetUInt("k", static_cast<uint32_t>(9)));
    TEST_ASSERT_EQUAL_INT32(0, settings.GetLong("k", static_cast<int32_t>(9)));
    TEST_ASSERT_EQUAL_UINT32(0u, settings.GetULong("k", static_cast<uint32_t>(9)));
    TEST_ASSERT_EQUAL_INT64(0LL, settings.GetLong64("k", static_cast<int64_t>(9)));
    TEST_ASSERT_EQUAL_UINT64(0ULL, settings.GetULong64("k", static_cast<uint64_t>(9)));
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.0F, settings.GetFloat("k", 1.0F));
    TEST_ASSERT_DOUBLE_WITHIN(0.0001, 0.0, settings.GetDouble("k", 1.0));
    TEST_ASSERT_FALSE(settings.GetBool("k", true));

    char buffer[16] = {0};
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.GetString("k", buffer, sizeof(buffer))));

    String stringResult = settings.GetString("k", String("fallback"));
    TEST_ASSERT_EQUAL_STRING("no data", stringResult.c_str());

    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.GetBytesLength("k")));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.GetBytes("k", &payload, sizeof(payload))));
    TEST_ASSERT_EQUAL_UINT(0u, static_cast<unsigned int>(settings.FreeEntries()));

    settings.End();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_singleton_returns_same_instance);
    RUN_TEST(test_dummy_settings_api_surface);
    return UNITY_END();
}
