module;

#include <type_traits>

export module Pawn.Core.Misc.Pair;

export namespace Pawn::Core::Misc
{
	template <typename ValueType1, typename ValueType2>
	struct Pair
	{
	public:
		using ValueDataType1 = ValueType1;
		using ValueDataType2 = ValueType2;

	public:
		Pair() {};
		Pair(ValueDataType1 value1, ValueDataType2 value2)
			: Value1(value1), Value2(value2)
		{
		}

		Pair(const Pair& other)
			: Value1(std::move(other.Value1)), Value2(std::move(other.Value2))
		{
		}


	public:
		bool operator==(const Pair& other)
		{
			return (this->Value1 == other.Value1 && this->Value2 == other.Value2);
		}

	public:
		ValueDataType1 Value1;
		ValueDataType2 Value2;

	};
}