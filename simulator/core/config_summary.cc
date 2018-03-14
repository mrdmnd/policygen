#include "simulator/core/config_summary.h"
#include "simulator/core/constants.h"
#include "simulator/core/event.h"

#include "proto/encounter_config.pb.h"
#include "proto/simulation.pb.h"

using std::chrono::milliseconds;

using simulator::core::enums::EventTag;

using simulatorproto::EncounterConfig;
using simulatorproto::SimulationConfig;

namespace simulator {
namespace core {

namespace {
// Put additional RaidEvent builder definitions here.
Event BuildSpawnEvent(const simulatorproto::EncounterEvent& proto) {
  std::function<void(SimulationState*)> cb = [&proto](SimulationState* s) {
    const auto enemy_proto = proto.spawn().enemy();
    const auto name = enemy_proto.name();
    const auto est = enemy_proto.health_estimator();

    if (est == simulatorproto::HealthEstimator::UNIFORM) {
      s->enemies.push_back(std::make_unique<Enemy>(
          name, HealthEstimator::UniformHealthEstimator()));
    } else if (est == simulatorproto::HealthEstimator::BURST) {
      s->enemies.push_back(std::make_unique<Enemy>(
          name, HealthEstimator::BurstHealthEstimator()));
    } else if (est == simulatorproto::HealthEstimator::EXECUTE) {
      s->enemies.push_back(std::make_unique<Enemy>(
          name, HealthEstimator::ExecuteHealthEstimator()));
    } else if (est == simulatorproto::HealthEstimator::BURST_AND_EXECUTE) {
      s->enemies.push_back(std::make_unique<Enemy>(
          name, HealthEstimator::BurstExecuteHealthEstimator()));
    } else {
      LOG(INFO) << "Health estimator " << est << " not found.";
      LOG(INFO) << "Proceeding with BurstAndExecute default.";
      s->enemies.push_back(std::make_unique<Enemy>(
          name, HealthEstimator::BurstExecuteHealthEstimator()));
    }

  };
  return Event(milliseconds(proto.timestamp()), cb, EventTag::ENEMY_SPAWN);
}

// From an encounter proto, get a vector of raid events.
std::vector<Event> BuildRaidEvents(const EncounterConfig& encounter_proto) {
  size_t n_events = encounter_proto.events_size();
  std::vector<Event> events;
  events.reserve(n_events);

  for (size_t i = 0; i < n_events; ++i) {
    const auto& event_proto = encounter_proto.events(i);
    switch (event_proto.event_case()) {
      case simulatorproto::EncounterEvent::kSpawn: {
        events.push_back(BuildSpawnEvent(event_proto));
        break;
      }
      case simulatorproto::EncounterEvent::kMovement: {
        break;
      }
      case simulatorproto::EncounterEvent::kLust: {
        break;
      }
      case simulatorproto::EncounterEvent::kStun: {
        break;
      }
      case simulatorproto::EncounterEvent::kDamage: {
        break;
      }
      case simulatorproto::EncounterEvent::kInvuln: {
        break;
      }
      default: { LOG(INFO) << "Attempting to add an unknown raid event."; }
    }
  }
  return events;
}
}  // namespace

// Constructor - this does a lot of work!
ConfigSummary::ConfigSummary(const SimulationConfig& sim_proto) :
  time_min_(sim_proto.encounter_config().min_time_millis()),
  time_max_(sim_proto.encounter_config().max_time_millis()),
  raid_events_(BuildRaidEvents(sim_proto.encounter_config())) {
  LOG(INFO) << "Constructing config summary.";
}
}  // namespace core
}  // namespace simulator
