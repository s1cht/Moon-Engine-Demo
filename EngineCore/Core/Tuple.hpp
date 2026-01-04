#pragma once

namespace ME::Core
{
    namespace Helper
    {
        template<size_t Index, typename T, typename... Rest>
        struct ArgType
        {
            using Type = typename ArgType<Index - 1, Rest...>::Type;
        };

        template<typename T, typename... Rest>
        struct ArgType<0, T, Rest...>
        {
            using Type = T;
        };
    }

    template <typename... Args>
    struct Tuple
    {
        template <size_t index, typename... List>
        struct TupleMember;

        template <size_t index, typename Head, typename... Tail>
        struct TupleMember<index, Head, Tail...>
        {
            Head Member;
            TupleMember<index + 1, Tail...> NextMember;

            static constexpr size_t Index() { return index; }
        };

        template <size_t index>
        struct TupleMember<index>
        {
            static constexpr size_t Index() { return index; }
        };

        TupleMember<0, Args...> Members;

        template<size_t Index>
        typename Helper::ArgType<Index, Args...>::Type& Member()
        {
            return GetMemberFromHolder<Index, typename Helper::ArgType<Index, Args...>::Type>(Members);
        }

    private:
        template<size_t TargetIndex, typename ReturnType, typename Node>
        ReturnType& GetMemberFromHolder(Node& node)
        {
            if constexpr (Node::Index() == TargetIndex)
                return node.Member;
            else
                return GetMemberFromHolder<TargetIndex, ReturnType>(node.NextMember);
        };
    };
}