#pragma once

#include <memory>
#include <vector>
#include "buffer.hpp"

struct PySTKGraphicsConfig {
    int screen_width=600, screen_height=400, display_adapter=0;
	bool glow = false, bloom = true, light_shaft = true, dynamic_lights = true, dof = true;
	int particles_effects = 2;
    bool animated_characters = true;
	bool motionblur = true;
	bool mlaa = true;
	bool texture_compression = true;
	bool ssao = true;
	bool degraded_IBL = false;
	int high_definition_textures = 2 | 1;
	bool render = true;
	
	static const PySTKGraphicsConfig & hd();
	static const PySTKGraphicsConfig & sd();
	static const PySTKGraphicsConfig & ld();
	static const PySTKGraphicsConfig & none();
};
struct PySTKPlayerConfig {
	enum Controller: uint8_t {
		PLAYER_CONTROL,
		AI_CONTROL,
	};
	std::string kart;
	Controller controller;
	int team = 0;
};
struct PySTKRaceConfig {
	enum RaceMode: uint8_t {
		NORMAL_RACE,
		TIME_TRIAL,
		FOLLOW_LEADER,
		THREE_STRIKES,
		FREE_FOR_ALL,
		CAPTURE_THE_FLAG,
		SOCCER,
	};
	
	int difficulty = 2;
	RaceMode mode = NORMAL_RACE;
	std::vector<PySTKPlayerConfig> players = {{"",PySTKPlayerConfig::PLAYER_CONTROL}};
	std::string track;
	bool reverse = false;
	int laps = 3;
	int seed = 0;
	int num_kart = 1;
	float step_size = 0.1;
};

class PySTKRenderTarget;

#ifndef SERVER_ONLY
struct PySTKRenderData {
    std::shared_ptr<NumpyPBO> color_buf_, depth_buf_, instance_buf_;
};
#endif  // SERVER_ONLY

class KartControl;
class Controller;
struct PySTKAction {
	float steering_angle = 0;
	float acceleration = 0;
	bool brake = false;
	bool nitro = false;
	bool drift = false;
	bool rescue = false;
	bool fire = false;
	void set(KartControl * control) const;
	void get(const KartControl * control);
};

class PySTKRace {
protected: // Static methods
	static void initRest();
    static void initUserConfig(const std::string & data_dir);
	static void initGraphicsConfig(const PySTKGraphicsConfig & config);
	static void cleanSuperTuxKart();
	static void cleanUserConfig();
	static PySTKGraphicsConfig graphics_config_;

public: // Static methods
	static PySTKRace * running_kart;
	static void init(const PySTKGraphicsConfig & config, const std::string & data_dir);
	static void load();
	static void clean();
	static bool isRunning();
	static std::vector<std::string> listTracks();
	static std::vector<std::string> listKarts();

protected:
	void setupConfig(const PySTKRaceConfig & config);
	void setupRaceStart();
	void render(float dt);
#ifndef SERVER_ONLY
	std::vector<std::unique_ptr<PySTKRenderTarget> > render_targets_;
	std::vector<std::shared_ptr<PySTKRenderData> > render_data_;
#endif  // SERVER_ONLY
	PySTKRaceConfig config_;
	float time_leftover_ = 0;

public:
	PySTKRace(const PySTKRace &) = delete;
	PySTKRace& operator=(const PySTKRace &) = delete;
	PySTKRace(const PySTKRaceConfig & config);
	~PySTKRace();
	void restart();
	void start();
	bool step(const std::vector<PySTKAction> &);
	bool step(const PySTKAction &);
	bool step();
	void stop();
#ifndef SERVER_ONLY
	const std::vector<std::shared_ptr<PySTKRenderData> > & render_data() const { return render_data_; }
#endif  // SERVER_ONLY
	const PySTKRaceConfig & config() const { return config_; }
	PySTKAction getKartAction(std::size_t);
};
