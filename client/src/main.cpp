#include "termox/termox.hpp"

int main() {
	auto const app = std::make_unique<ox::System>(ox::Mouse_mode::Drag);
	return app->run();
}
