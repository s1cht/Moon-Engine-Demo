export module Pawn.Thirdparty.spdlog;

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/ostr.h>

export namespace spdlog 
{
    using ::spdlog::info;
    using ::spdlog::warn;
    using ::spdlog::error;
    using ::spdlog::debug;
    using ::spdlog::trace;
    using ::spdlog::critical;

    using ::spdlog::get;
    using ::spdlog::set_level;
    using ::spdlog::level::level_enum;
    using ::spdlog::set_pattern;
    using ::spdlog::set_default_logger;
    using ::spdlog::default_logger;
    using ::spdlog::logger;

    using ::spdlog::shutdown;

    using ::spdlog::stdout_color_mt;
    using ::spdlog::stderr_color_mt;
    using ::spdlog::basic_logger_mt;
    using ::spdlog::rotating_logger_mt;
    using ::spdlog::daily_logger_mt;
    using ::spdlog::stdout_color_st;
    using ::spdlog::stderr_color_st;

    using ::spdlog::init_thread_pool;
    using ::spdlog::thread_pool;
    using ::spdlog::async_logger;

    using ::spdlog::default_factory;
    using ::spdlog::async_factory;

    using ::spdlog::sinks::sink;
    using ::spdlog::sinks::stdout_color_sink_mt;
    using ::spdlog::sinks::stderr_color_sink_mt;
    using ::spdlog::sinks::basic_file_sink_mt;
    using ::spdlog::sinks::rotating_file_sink_mt;
    using ::spdlog::sinks::daily_file_sink_mt;

    using ::spdlog::to_hex;
    using ::spdlog::memory_buf_t;

    namespace level
    {
        using ::spdlog::level::level_enum;
    }
}
