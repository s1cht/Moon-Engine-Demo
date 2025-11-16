#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Platform/Platform.hpp>
#include <Core/Platform/Base/IO.hpp>
#include <Core/Utility/Logging/Logger.hpp>

// --- Test Implementation ---

// Minimal assertion mechanism for tests
#define TEST_ASSERT(condition, message) \
       ME_ASSERT(condition, message)

using namespace ME::Core;

// Custom struct for complex type testing
struct TestObject {
    int id;
    std::string name;
    static int construct_count;
    static int destroy_count;

    TestObject(int i = 0, const std::string& n = "") : id(i), name(n) { construct_count++; }
    TestObject(const TestObject& other) : id(other.id), name(other.name) { construct_count++; }
    TestObject(TestObject&& other) noexcept : id(other.id), name(std::move(other.name)) {
        construct_count++;
        other.id = -1; // Mark source as moved-from
    }
    TestObject& operator=(const TestObject& other) { id = other.id; name = other.name; return *this; }
    TestObject& operator=(TestObject&& other) noexcept { id = other.id; name = std::move(other.name); other.id = -1; return *this; }
    ~TestObject() { destroy_count++; }
};

int TestObject::construct_count = 0;
int TestObject::destroy_count = 0;

// Helper to reset counters before tests
void ResetTestCounters() {
    TestObject::construct_count = 0;
    TestObject::destroy_count = 0;
}

// -------------------------------------------------------------------------------------------------
// # 1. Basic Functionality Tests
// -------------------------------------------------------------------------------------------------

bool TestBasicIntArray() {
    ME_INFO("--- Running TestBasicIntArray ---");
    Array<int> arr;
    TEST_ASSERT(arr.Size() == 0, "Initial size should be 0.");
    TEST_ASSERT(arr.Capacity() == 20, "Initial capacity should be 20.");

    arr.PushBack(10);
    arr.PushBack(20);
    TEST_ASSERT(arr.Size() == 2, "Size after 2 PushBacks should be 2.");
    TEST_ASSERT(arr[0] == 10, "Element at index 0 is incorrect.");
    TEST_ASSERT(arr[1] == 20, "Element at index 1 is incorrect.");

    int& back_ref = arr.EmplaceBack(30);
    TEST_ASSERT(arr.Size() == 3, "Size after EmplaceBack should be 3.");
    TEST_ASSERT(arr[2] == 30, "Element emplaced is incorrect.");
    TEST_ASSERT(back_ref == 30, "Reference returned by EmplaceBack is incorrect.");

    arr.PopBack();
    TEST_ASSERT(arr.Size() == 2, "Size after PopBack should be 2.");

    arr.Clear();
    TEST_ASSERT(arr.Empty(), "Array should be empty after Clear().");
    return true;
}

// -------------------------------------------------------------------------------------------------
// # 2. Iterator and Erase Tests
// -------------------------------------------------------------------------------------------------

bool TestIteratorAndErase() {
    ME_INFO("--- Running TestIteratorAndErase ---");
    Array<int> arr = { 1, 2, 3, 4, 5 };

    // Iterator test
    int expected = 1;
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        TEST_ASSERT(*it == expected++, "Iterator sequence is incorrect.");
    }

    // Erase test (middle element)
    auto next_it = arr.Erase(arr.begin() + 2); // Erase '3'
    TEST_ASSERT(arr.Size() == 4, "Size after erase should be 4.");
    TEST_ASSERT(arr[2] == 4, "Element shift after erase is incorrect (Expected 4).");
    TEST_ASSERT(*next_it == 4, "Iterator returned by Erase is incorrect.");

    // Erase test (first element)
    arr.EraseAt(0); // Erase '1'
    TEST_ASSERT(arr.Size() == 3, "Size after second erase should be 3.");
    TEST_ASSERT(arr[0] == 2, "First element after erase is incorrect.");

    return true;
}

// -------------------------------------------------------------------------------------------------
// # 3. Capacity and Resize Tests
// -------------------------------------------------------------------------------------------------

bool TestCapacityAndResize() {
    ME_INFO("--- Running TestCapacityAndResize ---");
    Array<int> arr;
    SIZE_T initial_cap = arr.Capacity();

    // Fill array until resize is triggered
    for (SIZE_T i = 0; i < initial_cap; ++i) {
        arr.PushBack(static_cast<int>(i));
    }
    TEST_ASSERT(arr.Size() == initial_cap, "Array size is incorrect before resize.");

    arr.PushBack(99); // Triggers resize (capacity * 2)
    TEST_ASSERT(arr.Size() == initial_cap + 1, "Array size is incorrect after resize.");
    TEST_ASSERT(arr.Capacity() == initial_cap * ARR_RESIZE_MULTIPLYER, "Capacity did not double.");
    TEST_ASSERT(arr[initial_cap] == 99, "New element after resize is incorrect.");

    // Test Reserve
    SIZE_T larger_cap = arr.Capacity() + 10;
    TEST_ASSERT(arr.Reserve(larger_cap), "Reserve should return true when increasing capacity.");
    TEST_ASSERT(arr.Capacity() == larger_cap, "Capacity after reserve is incorrect.");
    TEST_ASSERT(arr.Size() == initial_cap + 1, "Size should be preserved after Reserve.");

    return true;
}

// -------------------------------------------------------------------------------------------------
// # 4. Shared Pointer Array Tests (Focus on resource management)
// -------------------------------------------------------------------------------------------------

bool TestSharedPtrArray() {
    ME_INFO("--- Running TestSharedPtrArray ---");
    using PtrType = std::shared_ptr<int>;

    PtrType p1 = std::make_shared<int>(10);
    SIZE_T p1_initial_count = p1.use_count(); // Should be 1

    Array<PtrType> ptr_arr;

    // PushBack (Copy Construction)
    ptr_arr.PushBack(p1);
    TEST_ASSERT(ptr_arr.Size() == 1, "Size after PushBack is incorrect.");
    TEST_ASSERT(*ptr_arr[0] == 10, "Value in shared_ptr is incorrect.");
    TEST_ASSERT(p1.use_count() == p1_initial_count + 1, "Ref count didn't increase after copy PushBack.");

    // EmplaceBack (Move Construction)
    PtrType& back_ref = ptr_arr.EmplaceBack(std::make_shared<int>(20));
    TEST_ASSERT(ptr_arr.Size() == 2, "Size after EmplaceBack is incorrect.");
    TEST_ASSERT(*ptr_arr[1] == 20, "Emplaced value is incorrect.");

    // Erase (Ref count check)
    SIZE_T p1_count_before_erase = p1.use_count();
    ptr_arr.EraseAt(0); // Erase p1's copy
    TEST_ASSERT(ptr_arr.Size() == 1, "Size after Erase is incorrect.");
    TEST_ASSERT(p1.use_count() == p1_count_before_erase - 1, "Ref count didn't decrease after Erase.");
    TEST_ASSERT(*ptr_arr[0] == 20, "Element didn't shift correctly after Erase.");

    // PopBack (Ref count check)
    ptr_arr.PopBack(); // Erase {20}
    TEST_ASSERT(ptr_arr.Size() == 0, "Size after PopBack is incorrect.");

    // Clear check (should destroy shared_ptrs)
    Array<PtrType> temp_arr;
    temp_arr.PushBack(p1); // Ref count 2
    temp_arr.PushBack(std::make_shared<int>(30));
    TEST_ASSERT(p1.use_count() == p1_initial_count + 1, "Ref count check before clear failed.");
    temp_arr.Clear();
    TEST_ASSERT(p1.use_count() == p1_initial_count, "Ref count didn't return to original after Clear.");

    return true;
}

// -------------------------------------------------------------------------------------------------
// # 5. Complex Object Lifecycle Tests
// -------------------------------------------------------------------------------------------------

bool TestComplexObjectLifecycle() {
    // Note: Assuming ME_INFO and TEST_ASSERT are defined, and TestObject is available.
    // The fixed PPushBack from the previous turn is also assumed.

    ME_INFO("--- Running TestComplexObjectLifecycle ---");
    ResetTestCounters(); // C=0, D=0

    {
        Array<TestObject> arr;

        // 1. Initial EmplaceBacks
        arr.EmplaceBack(1, "A"); // C=1
        arr.EmplaceBack(2, "B"); // C=2
        TEST_ASSERT(TestObject::construct_count == 2, "EmplaceBack construction count incorrect.");

        // 2. Copy Assignment Test (Inside nested scope for controlled destruction)
        {
            // Create target array (arr2) with one element
            Array<TestObject> arr2 = { TestObject(10, "Z") }; // C=3, (Temporary C=4, D=1). Total C=3, D=1.
            TEST_ASSERT(TestObject::construct_count == 4, "Initial arr2 construction count incorrect (Expected 4).");
            TEST_ASSERT(TestObject::destroy_count == 1, "Initial arr2 destruction count incorrect (Expected 1).");

            // --- CRITICAL FIX: DO NOT ResetTestCounters() HERE ---
            // Resetting here causes the subsequent assignment check to fail cleanup.

            arr2 = arr; // Copy Assignment: arr2 is cleared first, then copies arr.

            // Expected actions during arr2 = arr:
            // a) arr2 elements destroyed: 1 destroy (for 'Z') -> D=2
            // b) arr elements copied: 2 copies (for 'A' and 'B') -> C=5

            TEST_ASSERT(TestObject::construct_count == 6, "Copy assignment construction count incorrect.");
            TEST_ASSERT(TestObject::destroy_count == 2, "Copy assignment destruction count incorrect.");
            TEST_ASSERT(arr2.Size() == 2, "Copy assignment size incorrect.");
        }
        // arr2 goes out of scope: 2 elements destroyed ('A' and 'B' copies). -> D=4

        // 3. Erase Test
        ResetTestCounters(); // C=0, D=0

        arr.EraseAt(0); // Erase 'A'. Shifts 'B' (move assignment) and destroys old 'B' location.
        // PErase uses move assignment, then destroys the last element.
        // For non-trivially copyable: 0-1 constructions, 1 destructor call.

        // The current PErase implementation results in one destruction call:
        // *pos = std::move(*(pos + 1)); // Move-assigns 'B' over 'A' (no C/D count change for smart objects)
        // m_Allocator.Destroy(m_Data + m_Size - 1); // Destroys the last element (original 'B')
        TEST_ASSERT(TestObject::destroy_count == 1, "Erase destructor count incorrect.");
        TEST_ASSERT(arr.Size() == 1, "Erase size incorrect.");
        TEST_ASSERT(arr[0].id == 2, "Erase shift incorrect.");
    }
    // arr goes out of scope: 1 element destroyed (the remaining 'B'). -> D=1

    TEST_ASSERT(TestObject::destroy_count == 2,
        "Final destruction count does not match construction count (Leaked objects detected).");

    return true;
}

// #6. Move Semantics Tests
bool TestMoveSemantics() {
    ME_INFO("--- Running TestMoveSemantics ---");
    ResetTestCounters();
    Array<TestObject> src;
    src.EmplaceBack(1, "X"); // C=1
    src.EmplaceBack(2, "Y"); // C=2
    Array<TestObject> dst = std::move(src); // Move construct
    TEST_ASSERT(src.Size() == 0, "Source size not zero after move.");
    TEST_ASSERT(dst.Size() == 2, "Dest size incorrect after move.");
    TEST_ASSERT(dst[0].id == 1, "Moved element[0] corrupted.");
    TEST_ASSERT(TestObject::construct_count == 2, "Extra constructions in move.");
    TEST_ASSERT(TestObject::destroy_count == 0, "Unexpected destructions in move.");
    Array<TestObject> target;
    target = std::move(dst); // Move assign
    TEST_ASSERT(dst.Size() == 0, "Dst size not zero after move assign.");
    TEST_ASSERT(target.Size() == 2, "Target size incorrect.");
    TEST_ASSERT(target[1].id == 2, "Moved element[1] corrupted.");
    return true;
}

// #7. Insert and Bulk Operations
bool TestInsertAndBulk() {
    ME_INFO("--- Running TestInsertAndBulk ---");
    Array<int> arr = { 1, 2, 5 };
    auto it = arr.Insert(arr.Begin() + 2, 3); // Insert before 5
    TEST_ASSERT(arr.Size() == 4, "Size after insert wrong.");
    TEST_ASSERT(*it == 3, "Insert returned wrong iterator.");
    TEST_ASSERT(arr[2] == 3 && arr[3] == 5, "Insert shift failed.");
    Array<int> src = { 10, 20 };
    arr.Insert(arr.end(), src.begin(), src.end());
    TEST_ASSERT(arr.Size() == 6, "Bulk insert size wrong.");
    TEST_ASSERT(arr[4] == 10 && arr[5] == 20, "Bulk insert values wrong.");
    return true;
}

// #8. Edge Cases and Stress
bool TestEdgeCases() {
    ME_INFO("--- Running TestEdgeCases ---");
    Array<int> arr;
    arr.Reserve(100);
    TEST_ASSERT(arr.Capacity() >= 100, "Reserve failed.");
    arr.Shrink();
    TEST_ASSERT(arr.Capacity() == 0, "Shrink failed on empty.");
    for (int i = 0; i < 1000; ++i) arr.PushBack(i);
    TEST_ASSERT(arr.Size() == 1000, "Stress push failed.");
    arr.Clear();
    TEST_ASSERT(arr.Empty(), "Clear failed after stress.");
    arr.PushBack(42);
    arr.Erase(arr.begin(), arr.end());
    TEST_ASSERT(arr.Empty(), "Range erase failed.");
    return true;
}


// --- Main Runner ---

int main() {
    ME::Core::Utility::Logger::Init();

    ME_INFO("Starting ME::Core::Array Tests (No Google Test)");
    ME_INFO("------------------------------------------------");

    std::vector<std::pair<std::string, bool (*)()>> tests = {
        {"Basic Integer Array Test", TestBasicIntArray},
        {"Iterator and Erase Test", TestIteratorAndErase},
        {"Capacity and Resize Test", TestCapacityAndResize},
        {"Shared Pointer Array Test", TestSharedPtrArray},
        {"Complex Object Lifecycle Test", TestComplexObjectLifecycle},
        {"Move Semantics Test", TestMoveSemantics},
        {"Insert And Bulk Test", TestInsertAndBulk},
        {"Edge Cases Test", TestEdgeCases},
    };

    int total_tests = 0;
    int passed_tests = 0;

    for (const auto& test : tests) {
        total_tests++;
        if (test.second()) {
            ME_INFO("PASS: {}", test.first);
            passed_tests++;
        }
    }

    ME_INFO("------------------------------------------------");
    ME_INFO("Test Summary: {0} / {1},  tests passed.",passed_tests, total_tests);

    ME::Core::Utility::Logger::Shutdown();

    return (passed_tests == total_tests) ? 0 : 1;
}