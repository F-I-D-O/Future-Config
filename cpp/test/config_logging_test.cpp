#include "gtest/gtest.h"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>

#include "future-config/configuration.h"
#include "future-config/loading.h"
#include "future-config/resources.h"

namespace fc {

namespace {

const std::string sources_header = "Future Config sources (in load order; a source with a higher number has higher "
	"priority and overrides the lower ones):\n";

const std::string values_header = "Future Config values ([n] = source the value comes from):\nConfiguration\n";

const std::string chain_header = "Future Config values ([n] = source the value comes from; in the overwrite chains, "
	"the highest number wins):\nConfiguration\n";

// config class that accepts any config
struct Any_config {
	explicit Any_config(const Config_object&) {}
};

/**
 * Redirects the default logger to a string stream, so that the log can be checked.
 */
class Config_logging: public ::testing::Test {
protected:
	std::ostringstream log_stream;

	std::shared_ptr<spdlog::logger> original_logger;

	const fs::path master_config_path = get_resource_path("test/binding/master.yaml");

	const fs::path local_config_path = get_resource_path("test/binding/network_local.yaml");

	void SetUp() override {
		original_logger = spdlog::default_logger();
		auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(log_stream);
		auto logger = std::make_shared<spdlog::logger>("config_logging_test", sink);
		logger->set_pattern("%v");
		spdlog::set_default_logger(logger);
	}

	void TearDown() override {
		spdlog::set_default_logger(original_logger);
	}

	[[nodiscard]] std::string get_log() const {
		auto log = log_stream.str();

		// the end of line used by the logger is platform dependent
		std::string::size_type position = 0;
		while((position = log.find("\r\n", position)) != std::string::npos) {
			log.replace(position, 2, "\n");
		}
		return log;
	}

	// master config, local config bound to server.network, and command line bound to server.network.
	// The arguments have to outlive the returned config definitions.
	[[nodiscard]] Config_definitions get_config_definitions(const std::vector<const char*>& argv) const {
		Config_definitions config_definitions;
		config_definitions.add(std::make_unique<Config_definition>(master_config_path));
		config_definitions.add(std::make_unique<Config_definition>(
			Config_type::LOCAL,
			local_config_path,
			"server.network"
		));
		config_definitions.add(std::make_unique<Command_line_config_definition>(
			static_cast<int>(argv.size()),
			const_cast<const char**>(argv.data()),
			std::vector<std::string>{},
			"server.network"
		));
		return config_definitions;
	}

	[[nodiscard]] std::string get_expected_sources() const {
		const auto master_path_string = master_config_path.string();
		const auto local_path_string = local_config_path.string();
		const auto location_width = std::max(master_path_string.size(), local_path_string.size());

		return sources_header
			+ "[1] main          " + master_path_string + "\n"
			+ "[2] local         " + local_path_string
			+ std::string(location_width - local_path_string.size(), ' ') + "  -> server.network\n"
			+ "[3] command line  " + std::string(location_width, ' ') + "  -> server.network\n";
	}
};

const std::vector<const char*> port_arguments{"program_name", "--port", "5"};

const std::string expected_values =
	"├── name: app                 [1]\n"
	"├── port: 1                   [1]\n"
	"├── server\n"
	"│   ├── network\n"
	"│   │   ├── label: app-local  [2] ${name}-local\n"
	"│   │   ├── port: 5           [3]\n"
	"│   │   ├── secure: false     [1]\n"
	"│   │   └── timeout: 30       [2]\n"
	"│   └── port: 8080            [1]\n"
	"└── tags: [a, b]              [1]\n";

const std::string expected_chain =
	"├── name: app                 [1]\n"
	"├── port: 1                   [1]\n"
	"├── server\n"
	"│   ├── network\n"
	"│   │   ├── label: app-local  [2]\n"
	"│   │   │   ┆╌╌ [1] default\n"
	"│   │   │   ┆╌╌ [2] ${name}-local\n"
	"│   │   ├── port: 5           [3]\n"
	"│   │   │   ┆╌╌ [1] 80\n"
	"│   │   │   ┆╌╌ [3] 5\n"
	"│   │   ├── secure: false     [1]\n"
	"│   │   └── timeout: 30       [2]\n"
	"│   │       ┆╌╌ [1] 10\n"
	"│   │       ┆╌╌ [2] 30\n"
	"│   └── port: 8080            [1]\n"
	"└── tags: [a, b]              [1]\n";

} // namespace


TEST_F(Config_logging, nothing_logged_by_default) {
	const auto config_definitions = get_config_definitions(port_arguments);

	load_config(config_definitions.config_definitions);

	EXPECT_EQ(get_log(), "");
}

TEST_F(Config_logging, sources) {
	const auto config_definitions = get_config_definitions(port_arguments);

	load_config(config_definitions.config_definitions, Config_log_level::SOURCES);

	EXPECT_EQ(get_log(), get_expected_sources());
}

TEST_F(Config_logging, values) {
	const auto config_definitions = get_config_definitions(port_arguments);

	load_config(config_definitions.config_definitions, Config_log_level::VALUES);

	EXPECT_EQ(get_log(), get_expected_sources() + values_header + expected_values);
}

TEST_F(Config_logging, chain) {
	const auto config_definitions = get_config_definitions(port_arguments);

	load_config(config_definitions.config_definitions, Config_log_level::CHAIN);

	EXPECT_EQ(get_log(), get_expected_sources() + chain_header + expected_chain);
}

TEST_F(Config_logging, single_source) {
	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(get_resource_path("test/config.yaml")));

	load_config(config_definitions.config_definitions, Config_log_level::CHAIN);

	EXPECT_EQ(
		get_log(),
		sources_header + "[1] main  " + get_resource_path("test/config.yaml").string() + "\n"
			+ chain_header + "└── test_par: 1  [1]\n"
	);
}

TEST_F(Config_logging, level_from_command_line) {
	const std::vector<const char*> arguments{"program_name", "--fconfig_log", "chain", "--port", "5"};
	const auto config_definitions = get_config_definitions(arguments);

	load_config(config_definitions.config_definitions);

	EXPECT_EQ(get_log(), get_expected_sources() + chain_header + expected_chain);
}

TEST_F(Config_logging, level_from_command_line_has_priority) {
	const std::vector<const char*> sources_arguments{"program_name", "--port", "5", "--fconfig_log", "sources"};
	auto config_definitions = get_config_definitions(sources_arguments);
	load_config(config_definitions.config_definitions, Config_log_level::CHAIN);
	EXPECT_EQ(get_log(), get_expected_sources());

	log_stream.str("");
	const std::vector<const char*> none_arguments{"program_name", "--fconfig_log", "none", "--port", "5"};
	config_definitions = get_config_definitions(none_arguments);
	load_config(config_definitions.config_definitions, Config_log_level::CHAIN);
	EXPECT_EQ(get_log(), "");
}

TEST_F(Config_logging, level_from_load_options) {
	load<Any_config>({
		.config_definitions = get_config_definitions(port_arguments),
		.config_log_level = Config_log_level::VALUES
	});

	EXPECT_EQ(get_log(), get_expected_sources() + values_header + expected_values);
}

TEST_F(Config_logging, level_from_command_line_in_load_options) {
	const char* argv[] = {"program_name", "--fconfig_log", "sources"};
	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(master_config_path));

	load<Any_config>({
		.config_definitions = config_definitions,
		.command_line_arguments = std::make_pair(3, argv)
	});

	EXPECT_EQ(
		get_log(),
		sources_header + "[1] main          " + master_config_path.string() + "\n[2] command line\n"
	);
}

TEST_F(Config_logging, invalid_level_on_command_line_throws) {
	// unknown level name
	const std::vector<const char*> unknown_level_arguments{"program_name", "--fconfig_log", "everything"};
	auto config_definitions = get_config_definitions(unknown_level_arguments);
	EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error);

	// missing level name
	const std::vector<const char*> missing_level_arguments{"program_name", "--port", "5", "--fconfig_log"};
	config_definitions = get_config_definitions(missing_level_arguments);
	EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error);

	EXPECT_EQ(get_log(), "");
}

TEST_F(Config_logging, sources_logged_when_loading_fails) {
	// the name key is not present in the object the command line is bound to
	const std::vector<const char*> arguments{"program_name", "--fconfig_log", "chain", "--name", "other"};
	const auto config_definitions = get_config_definitions(arguments);

	EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error);

	EXPECT_EQ(get_log(), get_expected_sources());
}

} // namespace fc
